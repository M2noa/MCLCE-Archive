/*
MIT License

Copyright (c) 2026 Patoke

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "stdafx.h"
#include "Renderer.h"
#include "libpng/png.h"

unsigned char* dataStart;
unsigned char *dataCurr;
unsigned char *dataEnd;

DXGI_FORMAT Renderer::textureFormats[] = { 
    DXGI_FORMAT_R8G8B8A8_UNORM, 
    DXGI_FORMAT_UNKNOWN 
};

// these are here because they are used before they are defined
// its fine like that and the order of everything matches ida so we have to put these here
void user_flush_data(png_struct_def *png_ptr);
void user_write_data(png_struct_def *png_ptr, unsigned char *src, size_t length);

HRESULT Renderer::LoadTextureData(BYTE* pbData, DWORD dwBytes, D3DXIMAGE_INFO* pSrcInfo, int** ppDataOut)
{
    PROFILER_SCOPE("Renderer::LoadTextureData_Memory", "LoadTextureData_Memory", MP_PURPLE4);
    png_image image;
    memset(&image, 0, sizeof(image));
    image.version = PNG_IMAGE_VERSION;

    png_image_begin_read_from_memory(&image, pbData, dwBytes);

    if (PNG_IMAGE_FAILED(image))
        return E_FAIL;

    image.format = PNG_FORMAT_BGRA;

    *ppDataOut = new int[image.width * image.height];
    if (!*ppDataOut || !png_image_finish_read(&image, NULL, *ppDataOut, NULL, NULL))
        return E_FAIL;

    pSrcInfo->Width = image.width;
    pSrcInfo->Height = image.height;
    return S_OK;
}

HRESULT Renderer::LoadTextureData(const char* szFilename, D3DXIMAGE_INFO* pSrcInfo, int** ppDataOut)
{
    PROFILER_SCOPE("Renderer::LoadTextureData_File", "LoadTextureData_File", MP_PURPLE4);
    png_image image;
    memset(&image, 0, sizeof(image));
    image.version = PNG_IMAGE_VERSION;

    png_image_begin_read_from_file(&image, szFilename);

    if (PNG_IMAGE_FAILED(image))
        return E_FAIL;

    image.format = PNG_FORMAT_BGRA;

    *ppDataOut = new int[image.width * image.height];
    if (!*ppDataOut || !png_image_finish_read(&image, NULL, *ppDataOut, NULL, NULL))
        return E_FAIL;

    pSrcInfo->Width = image.width;
    pSrcInfo->Height = image.height;
    return S_OK;
}

HRESULT Renderer::SaveTextureData(const char* szFilename, D3DXIMAGE_INFO* pSrcInfo, int* ppDataOut)
{
    PROFILER_SCOPE("Renderer::SaveTextureData", "SaveTextureData", MP_PURPLE4);
    png_image image;
    memset(&image, 0, sizeof(image));

    image.width = pSrcInfo->Width;
    image.height = pSrcInfo->Height;
    image.version = PNG_IMAGE_VERSION;
    image.format = PNG_FORMAT_RGBA;

    png_image_write_to_file(&image, szFilename, NULL, ppDataOut, NULL, NULL);
    return S_OK;
}

HRESULT Renderer::SaveTextureDataToMemory(void* pOutput, int outputCapacity, int* outputLength, int width, int height, int* ppDataIn)
{
    PROFILER_SCOPE("Renderer::SaveTextureDataToMemory", "SaveTextureDataToMemory", MP_PURPLE4);
    png_image image;
    memset(&image, 0, sizeof(image));

    image.width = width;
    image.height = height;
    dataEnd = (BYTE *)pOutput + outputCapacity;
    image.version = PNG_IMAGE_VERSION;
    image.format = PNG_FORMAT_RGBA;
    dataStart = (BYTE*)pOutput;
    dataCurr = (BYTE*)pOutput;

    png_image_write_to_stdio(&image, NULL, NULL, ppDataIn, NULL, NULL, user_write_data, user_flush_data);

    *outputLength = (int)(dataCurr - dataStart);
    return S_OK;
}

void Renderer::TextureBind(int idx)
{
    PROFILER_SCOPE("Renderer::TextureBind", "TextureBind", MP_PURPLE4);
    if (idx == -1)
        idx = defaultTextureIndex;

    Context& c = getContext();

    if (c.commandBuffer && c.commandBuffer->isActive)
        c.commandBuffer->BindTexture(idx);

    c.textureIdx = idx;
    c.m_pDeviceContext->PSSetShaderResources(0, 1, &m_textures[idx].view);

    UpdateTextureState(false);
}

void Renderer::TextureBindVertex(int idx)
{
    PROFILER_SCOPE("Renderer::TextureBindVertex", "TextureBindVertex", MP_PURPLE4);
    if (idx == -1)
        idx = defaultTextureIndex;

    Context& c = getContext();

    c.textureIdx = idx;
    c.m_pDeviceContext->VSSetShaderResources(0, 1, &m_textures[idx].view);

    UpdateTextureState(true);
}

int Renderer::TextureCreate()
{
    PROFILER_SCOPE("Renderer::TextureCreate", "TextureCreate", MP_PURPLE4);
    for (int i = 0; i < MAX_TEXTURES; i++)
    {
        if (!m_textures[i].allocated)
        {
            m_textures[i].texture = NULL;
            m_textures[i].allocated = true;
            m_textures[i].mipLevels = 1;
            m_textures[i].samplerParams = 0;
            return i;
        }
    }
    return -1;
}

void Renderer::TextureData(int width, int height, void* data, int level, C4JRender::eTextureFormat format)
{
    PROFILER_SCOPE("Renderer::TextureData", "TextureData", MP_PURPLE4);
    Context& c = getContext();
    int idx = c.textureIdx;

    m_textures[idx].textureFormat = format;

    if (level == 0)
    {
        D3D11_TEXTURE2D_DESC desc;
        desc.Width = width;
        desc.Height = height;
        desc.MipLevels = m_textures[idx].mipLevels;
        desc.ArraySize = 1;
        desc.Format = textureFormats[format];
        desc.SampleDesc.Count = 1;
        desc.SampleDesc.Quality = 0;
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        desc.CPUAccessFlags = 0;
        desc.MiscFlags = 0;

        m_pDevice->CreateTexture2D(&desc, NULL, &m_textures[idx].texture);
        m_pDevice->CreateShaderResourceView(m_textures[idx].texture, NULL, &m_textures[idx].view);
    }

    c.m_pDeviceContext->UpdateSubresource(
        m_textures[idx].texture,
        level,
        NULL,
        data,
        static_cast<UINT>(width * 4),
        static_cast<UINT>(width * height * 4)
    );
}

void Renderer::TextureDataUpdate(int xoffset, int yoffset, int width, int height, void* data, int level)
{
    PROFILER_SCOPE("Renderer::TextureDataUpdate", "TextureDataUpdate", MP_PURPLE4);
    Context& c = getContext();
    int idx = c.textureIdx;

    D3D11_TEXTURE2D_DESC desc = {};
    m_textures[idx].texture->GetDesc(&desc);

    D3D11_BOX box = {};
    box.left = xoffset;
    box.top = yoffset;
    box.right = xoffset + width;
    box.bottom = yoffset + height;
    box.front = 0;
    box.back = 1;

    c.m_pDeviceContext->UpdateSubresource(
        m_textures[idx].texture,
        level,
        &box,
        data,
        static_cast<UINT>(width * 4),
        static_cast<UINT>(width * height * 4)
    );
}

void Renderer::TextureDynamicUpdateEnd() {}
void Renderer::TextureDynamicUpdateStart() {}

void Renderer::TextureFree(int idx)
{
    PROFILER_SCOPE("Renderer::TextureFree", "TextureFree", MP_PURPLE4);
    m_textures[idx].texture->Release();
    m_textures[idx].view->Release();
    m_textures[idx].view = NULL;
    m_textures[idx].allocated = false;
    m_textures[idx].texture = NULL;
}

void Renderer::TextureGetStats() {}

ID3D11ShaderResourceView* Renderer::TextureGetTexture(int idx)
{
    if (idx < MAX_TEXTURES) {
        if (m_textures[idx].allocated) return m_textures[idx].view;
    }
    return NULL;
}

int Renderer::TextureGetTextureLevels()
{
    Context& c = getContext();
    return m_textures[c.textureIdx].mipLevels;
}

void Renderer::TextureSetParam(int param, int value)
{
    Context& c = getContext();
    int idx = c.textureIdx;

    switch (param)
    {
    case GL_TEXTURE_MIN_FILTER:
        m_textures[idx].samplerParams &= ~4u;
        if (value == GL_LINEAR)
            m_textures[idx].samplerParams |= 4u;
        break;
    case GL_TEXTURE_MAG_FILTER:
        m_textures[idx].samplerParams &= ~8u;
        if (value == GL_LINEAR)
            m_textures[idx].samplerParams |= 8u;
        break;
    case GL_TEXTURE_WRAP_S:
        m_textures[idx].samplerParams &= ~1u;
        if (value == GL_CLAMP)
            m_textures[idx].samplerParams |= 1u;
        break;
    case GL_TEXTURE_WRAP_T:
        m_textures[idx].samplerParams &= ~2u;
        if (value == GL_CLAMP)
            m_textures[idx].samplerParams |= 2u;
        break;
    }
}

void Renderer::TextureSetTextureLevels(int levels)
{
    Context& c = getContext();
    m_textures[c.textureIdx].mipLevels = levels;
}

void Renderer::UpdateTextureState(bool bVertex)
{
    Context& c = getContext();
    ID3D11SamplerState* pSampler = GetManagedSamplerState();

    if (bVertex)
        c.m_pDeviceContext->VSSetSamplers(0, 1, &pSampler);
    else
        c.m_pDeviceContext->PSSetSamplers(0, 1, &pSampler);
}

void user_flush_data(png_struct_def* png_ptr) {}
void user_write_data(png_struct_def* png_ptr, unsigned char* src, size_t length)
{
    int bytesToWrite = static_cast<int>(dataEnd - dataCurr);
    if (static_cast<int>(length) < bytesToWrite)
        bytesToWrite = (int)length;

    std::memcpy(dataCurr, src, bytesToWrite);
    dataCurr += bytesToWrite;
}

int user_write_data_bytes_written()
{
    return static_cast<int>(dataCurr - dataStart);
}

void user_write_data_init(unsigned char* pBuffer, int size)
{
    dataStart = pBuffer;
    dataCurr = pBuffer;
    dataEnd = pBuffer + size;
}