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

void Renderer::DrawVertexBuffer(C4JRender::ePrimitiveType PrimitiveType, int count, ID3D11Buffer *buffer, C4JRender::eVertexType vType,
                                C4JRender::ePixelShaderType psType)
{
    PROFILER_SCOPE("Renderer::DrawVertexBuffer", "DrawVertexBuffer", MP_RED2);
    Renderer::Context &c = getContext();
    ID3D11DeviceContext *d3d11 = c.m_pDeviceContext;

    int drawCount = count;
    bool indexed = false;

    PROFILER_SCOPE("Renderer::DrawVertexBuffer", "DrawVertexSetup", MP_RED2);
    DrawVertexSetup(vType, psType, PrimitiveType, &drawCount, &indexed);
    StateUpdate();

    const UINT stride = vertexStrideTable[vType];
    const UINT offset = 0;
    d3d11->IASetVertexBuffers(0, 1, &buffer, &stride, &offset);

    if (indexed)
        d3d11->DrawIndexed(drawCount, 0, 0);
    else
        d3d11->Draw(count, 0);
}

void Renderer::DrawVertexSetup(C4JRender::eVertexType vType, C4JRender::ePixelShaderType psType, C4JRender::ePrimitiveType PrimitiveType, int *count,
                               bool *indexed)
{
    PROFILER_SCOPE("Renderer::DrawVertexSetup", "DrawVertexSetup", MP_RED2);
    Renderer::Context &c = getContext();
    ID3D11DeviceContext *d3d11 = c.m_pDeviceContext;

    C4JRender::eVertexType effectiveVertexType = vType;
    if (effectiveVertexType == C4JRender::VERTEX_TYPE_PF3_TF2_CB4_NB4_XW1 && c.lightingEnabled)
        effectiveVertexType = C4JRender::VERTEX_TYPE_PF3_TF2_CB4_NB4_XW1_LIT;

    if (effectiveVertexType != activeVertexType)
    {
        d3d11->VSSetShader(vertexShaderTable[effectiveVertexType], NULL, 0);
        d3d11->IASetInputLayout(inputLayoutTable[effectiveVertexType]);
        activeVertexType = effectiveVertexType;
    }

    if (psType != activePixelType)
    {
        d3d11->PSSetShader(pixelShaderTable[psType], NULL, 0);
        activePixelType = psType;
    }

    D3D11_MAPPED_SUBRESOURCE mapped = {};

    if (c.matrixDirty[MATRIX_MODE_MODELVIEW])
    {
        d3d11->Map(c.m_modelViewMatrix, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
        memcpy(mapped.pData, MatrixGet(MATRIX_MODE_MODELVIEW), sizeof(DirectX::XMMATRIX));
        d3d11->Unmap(c.m_modelViewMatrix, 0);
        c.matrixDirty[MATRIX_MODE_MODELVIEW] = false;
    }

    if (c.matrixDirty[MATRIX_MODE_MODELVIEW_PROJECTION])
    {
        d3d11->Map(c.m_projectionMatrix, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
        memcpy(mapped.pData, MatrixGet(MATRIX_MODE_MODELVIEW_PROJECTION), sizeof(DirectX::XMMATRIX));
        d3d11->Unmap(c.m_projectionMatrix, 0);
        c.matrixDirty[MATRIX_MODE_MODELVIEW_PROJECTION] = false;
    }

    if (c.matrixDirty[MATRIX_MODE_MODELVIEW_TEXTURE])
    {
        d3d11->Map(c.m_textureMatrix, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
        memcpy(mapped.pData, MatrixGet(MATRIX_MODE_MODELVIEW_TEXTURE), sizeof(DirectX::XMMATRIX));
        d3d11->Unmap(c.m_textureMatrix, 0);
        c.matrixDirty[MATRIX_MODE_MODELVIEW_TEXTURE] = false;
    }

    UpdateFogState();
    UpdateViewportState();
    UpdateLightingState();
    UpdateTexGenState();

    d3d11->IASetPrimitiveTopology(g_topologies[PrimitiveType]);

    if (PrimitiveType == C4JRender::PRIMITIVE_TYPE_QUAD_LIST)
    {
        d3d11->IASetIndexBuffer(quadIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
        *count = (*count * 6) / 4;
        *indexed = true;
        return;
    }

    if (PrimitiveType == C4JRender::PRIMITIVE_TYPE_TRIANGLE_FAN)
    {
        d3d11->IASetIndexBuffer(fanIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
        *count = (*count - 2) * 3;
        *indexed = true;
        return;
    }

    d3d11->IASetIndexBuffer(NULL, DXGI_FORMAT_R16_UINT, 0);
    *indexed = false;
}

void Renderer::DrawVertices(C4JRender::ePrimitiveType PrimitiveType, int count, void *vertices, C4JRender::eVertexType vType,
                            C4JRender::ePixelShaderType psType)
{
    PROFILER_SCOPE("Renderer::DrawVertices", "DrawVertices", MP_RED2);
    Renderer::Context &c = getContext();
    ID3D11DeviceContext *d3d11 = c.m_pDeviceContext;
    Renderer::CommandBuffer *commandBuffer = c.commandBuffer;

    if (commandBuffer != NULL)
    {
        C4JRender::eVertexType effectiveVertexType = vType;
        if (effectiveVertexType == C4JRender::VERTEX_TYPE_PF3_TF2_CB4_NB4_XW1 && c.lightingEnabled)
            effectiveVertexType = C4JRender::VERTEX_TYPE_PF3_TF2_CB4_NB4_XW1_LIT;

        c.recordingPrimitiveType = PrimitiveType;
        c.recordingVertexType = effectiveVertexType;
        const UINT stride = vertexStrideTable[effectiveVertexType];
        commandBuffer->AddVertices(stride, static_cast<UINT>(count), vertices, c);
        return;
    }

    int drawCount = count;
    bool indexed = false;

    PROFILER_SCOPE("Renderer::DrawVertices", "DrawVertexSetup", MP_RED2);
    DrawVertexSetup(vType, psType, PrimitiveType, &drawCount, &indexed);

    const UINT stride = vertexStrideTable[vType];
    const UINT vertexBytes = stride * static_cast<UINT>(count);

    assert(vertexBytes <= Context::VERTEX_BUFFER_SIZE);

    UINT vertexOffset = c.dynamicVertexOffset;
    if (vertexOffset + vertexBytes > Context::VERTEX_BUFFER_SIZE)
        vertexOffset = 0;

    D3D11_MAPPED_SUBRESOURCE mapped = {};
    const D3D11_MAP mapType = vertexOffset == 0 ? D3D11_MAP_WRITE_DISCARD : D3D11_MAP_WRITE_NO_OVERWRITE;
    const HRESULT hr = d3d11->Map(c.dynamicVertexBuffer, 0, mapType, 0, &mapped);
    if (FAILED(hr))
        printf("ERROR: 0x%x\n", static_cast<unsigned int>(hr));

    memcpy(reinterpret_cast<std::uint8_t *>(mapped.pData) + vertexOffset, vertices, vertexBytes);
    d3d11->Unmap(c.dynamicVertexBuffer, 0);

    StateUpdate();

    ID3D11Buffer *dynamicBuffer = c.dynamicVertexBuffer;
    d3d11->IASetVertexBuffers(0, 1, &dynamicBuffer, &stride, &vertexOffset);

    if (indexed)
        d3d11->DrawIndexed(drawCount, 0, 0);
    else
        d3d11->Draw(count, 0);

    c.dynamicVertexOffset = vertexOffset + vertexBytes;
}
