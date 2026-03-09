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
#include "4J_Render.h"
#include "Renderer.h"

C4JRender RenderManager;

void C4JRender::Tick()
{
	InternalRenderManager.CBuffTick();
}

void C4JRender::UpdateGamma(unsigned short usGamma)
{
	InternalRenderManager.UpdateGamma(usGamma);
}

void C4JRender::MatrixMode(int type)
{
	InternalRenderManager.MatrixMode(type);
}

void C4JRender::MatrixSetIdentity()
{
	InternalRenderManager.MatrixSetIdentity();
}

void C4JRender::MatrixTranslate(float x, float y, float z)
{
	InternalRenderManager.MatrixTranslate(x, y, z);
}

void C4JRender::MatrixRotate(float angle, float x, float y, float z)
{
	InternalRenderManager.MatrixRotate(angle, x, y, z);
}

void C4JRender::MatrixScale(float x, float y, float z)
{
	InternalRenderManager.MatrixScale(x, y, z);
}

void C4JRender::MatrixPerspective(float fovy, float aspect, float zNear, float zFar)
{
	InternalRenderManager.MatrixPerspective(fovy, aspect, zNear, zFar);
}

void C4JRender::MatrixOrthogonal(float left, float right, float bottom, float top, float zNear, float zFar)
{
	InternalRenderManager.MatrixOrthogonal(left, right, bottom, top, zNear, zFar);
}

void C4JRender::MatrixPop()
{
	InternalRenderManager.MatrixPop();
}

void C4JRender::MatrixPush()
{
	InternalRenderManager.MatrixPush();
}

void C4JRender::MatrixMult(float* mat)
{
	InternalRenderManager.MatrixMult(mat);
}

const float* C4JRender::MatrixGet(int type)
{
	return InternalRenderManager.MatrixGet(type);
}

void C4JRender::Set_matrixDirty()
{
	InternalRenderManager.Set_matrixDirty();
}

void C4JRender::Initialise(ID3D11Device* pDevice, IDXGISwapChain* pSwapChain)
{
	InternalRenderManager.Initialise(pDevice, pSwapChain);
}

void C4JRender::InitialiseContext()
{
	InternalRenderManager.InitialiseContext(false);
}

void C4JRender::StartFrame()
{
	InternalRenderManager.StartFrame();
}

void C4JRender::DoScreenGrabOnNextPresent()
{
	InternalRenderManager.DoScreenGrabOnNextPresent();
}

void C4JRender::Present()
{
	InternalRenderManager.Present();
}

void C4JRender::Clear(int flags, D3D11_RECT* pRect)
{
	InternalRenderManager.Clear(flags, pRect);
}

void C4JRender::SetClearColour(const float colourRGBA[4])
{
	InternalRenderManager.SetClearColour(colourRGBA);
}

bool C4JRender::IsWidescreen()
{
	return InternalRenderManager.IsWidescreen();
}

bool C4JRender::IsHiDef()
{
	return InternalRenderManager.IsHiDef();
}

void C4JRender::CaptureThumbnail(ImageFileBuffer* pngOut)
{
	InternalRenderManager.CaptureThumbnail(pngOut);
}

void C4JRender::CaptureScreen(ImageFileBuffer* jpgOut, XSOCIAL_PREVIEWIMAGE* previewOut)
{
	InternalRenderManager.CaptureScreen(jpgOut, previewOut);
}

void C4JRender::BeginConditionalSurvey(int identifier)
{
	InternalRenderManager.BeginConditionalSurvey(identifier);
}

void C4JRender::EndConditionalSurvey()
{
	InternalRenderManager.EndConditionalSurvey();
}

void C4JRender::BeginConditionalRendering(int identifier)
{
	InternalRenderManager.BeginConditionalRendering(identifier);
}

void C4JRender::EndConditionalRendering()
{
	InternalRenderManager.EndConditionalRendering();
}

void C4JRender::DrawVertices(ePrimitiveType PrimitiveType, int count, void* dataIn, eVertexType vType, ePixelShaderType psType)
{
	InternalRenderManager.DrawVertices(PrimitiveType, count, dataIn, vType, psType);
}

void C4JRender::DrawVertexBuffer(ePrimitiveType PrimitiveType, int count, ID3D11Buffer* buffer, eVertexType vType, ePixelShaderType psType)
{
	InternalRenderManager.DrawVertexBuffer(PrimitiveType, count, buffer, vType, psType);
}

void C4JRender::CBuffLockStaticCreations()
{
	InternalRenderManager.CBuffLockStaticCreations();
}

int C4JRender::CBuffCreate(int count)
{
	return InternalRenderManager.CBuffCreate(count);
}

void C4JRender::CBuffDelete(int first, int count)
{
	InternalRenderManager.CBuffDelete(first, count);
}

void C4JRender::CBuffStart(int index, bool full)
{
	InternalRenderManager.CBuffStart(index, full);
}

void C4JRender::CBuffClear(int index)
{
	InternalRenderManager.CBuffClear(index);
}

int C4JRender::CBuffSize(int index)
{
	return InternalRenderManager.CBuffSize(index);
}

void C4JRender::CBuffEnd()
{
	InternalRenderManager.CBuffEnd();
}

bool C4JRender::CBuffCall(int index, bool full)
{
	return InternalRenderManager.CBuffCall(index, full);
}

void C4JRender::CBuffTick()
{
	InternalRenderManager.CBuffTick();
}

void C4JRender::CBuffDeferredModeStart()
{
	InternalRenderManager.CBuffDeferredModeStart();
}

void C4JRender::CBuffDeferredModeEnd()
{
	InternalRenderManager.CBuffDeferredModeEnd();
}

int C4JRender::TextureCreate()
{
	return InternalRenderManager.TextureCreate();
}

void C4JRender::TextureFree(int idx)
{
	InternalRenderManager.TextureFree(idx);
}

void C4JRender::TextureBind(int idx)
{
	InternalRenderManager.TextureBind(idx);
}

void C4JRender::TextureBindVertex(int idx)
{
	InternalRenderManager.TextureBindVertex(idx);
}

void C4JRender::TextureSetTextureLevels(int levels)
{
	InternalRenderManager.TextureSetTextureLevels(levels);
}

int C4JRender::TextureGetTextureLevels()
{
	return InternalRenderManager.TextureGetTextureLevels();
}

void C4JRender::TextureData(int width, int height, void* data, int level, eTextureFormat format)
{
	InternalRenderManager.TextureData(width, height, data, level, format);
}

void C4JRender::TextureDataUpdate(int xoffset, int yoffset, int width, int height, void* data, int level)
{
	InternalRenderManager.TextureDataUpdate(xoffset, yoffset, width, height, data, level);
}

void C4JRender::TextureSetParam(int param, int value)
{
	InternalRenderManager.TextureSetParam(param, value);
}

void C4JRender::TextureDynamicUpdateStart()
{
	InternalRenderManager.TextureDynamicUpdateStart();
}

void C4JRender::TextureDynamicUpdateEnd()
{
	InternalRenderManager.TextureDynamicUpdateEnd();
}

HRESULT C4JRender::LoadTextureData(const char* szFilename, D3DXIMAGE_INFO* pSrcInfo, int** ppDataOut)
{
	return InternalRenderManager.LoadTextureData(szFilename, pSrcInfo, ppDataOut);
}

HRESULT C4JRender::LoadTextureData(BYTE* pbData, DWORD dwBytes, D3DXIMAGE_INFO* pSrcInfo, int** ppDataOut)
{
	return InternalRenderManager.LoadTextureData(pbData, dwBytes, pSrcInfo, ppDataOut);
}

HRESULT C4JRender::SaveTextureData(const char* szFilename, D3DXIMAGE_INFO* pSrcInfo, int* ppDataOut)
{
	return InternalRenderManager.SaveTextureData(szFilename, pSrcInfo, ppDataOut);
}

HRESULT C4JRender::SaveTextureDataToMemory(void* pOutput, int outputCapacity, int* outputLength, int width, int height, int* ppDataIn)
{
	return InternalRenderManager.SaveTextureDataToMemory(pOutput, outputCapacity, outputLength, width, height, ppDataIn);
}

void C4JRender::TextureGetStats()
{
}

ID3D11ShaderResourceView* C4JRender::TextureGetTexture(int idx)
{
	return InternalRenderManager.TextureGetTexture(idx);
}

void C4JRender::StateSetColour(float r, float g, float b, float a)
{
	InternalRenderManager.StateSetColour(r, g, b, a);
}

void C4JRender::StateSetDepthMask(bool enable)
{
	InternalRenderManager.StateSetDepthMask(enable);
}

void C4JRender::StateSetBlendEnable(bool enable)
{
	InternalRenderManager.StateSetBlendEnable(enable);
}

void C4JRender::StateSetBlendFunc(int src, int dst)
{
	InternalRenderManager.StateSetBlendFunc(src, dst);
}

void C4JRender::StateSetBlendFactor(unsigned int colour)
{
	InternalRenderManager.StateSetBlendFactor(colour);
}

void C4JRender::StateSetAlphaFunc(int func, float param)
{
	InternalRenderManager.StateSetAlphaFunc(func, param);
}

void C4JRender::StateSetDepthFunc(int func)
{
	InternalRenderManager.StateSetDepthFunc(func);
}

void C4JRender::StateSetFaceCull(bool enable)
{
	InternalRenderManager.StateSetFaceCull(enable);
}

void C4JRender::StateSetFaceCullCW(bool enable)
{
	InternalRenderManager.StateSetFaceCullCW(enable);
}

void C4JRender::StateSetLineWidth(float width)
{
	InternalRenderManager.StateSetLineWidth(width);
}

void C4JRender::StateSetWriteEnable(bool red, bool green, bool blue, bool alpha)
{
	InternalRenderManager.StateSetWriteEnable(red, green, blue, alpha);
}

void C4JRender::StateSetDepthTestEnable(bool enable)
{
	InternalRenderManager.StateSetDepthTestEnable(enable);
}

void C4JRender::StateSetAlphaTestEnable(bool enable)
{
	InternalRenderManager.StateSetAlphaTestEnable(enable);
}

void C4JRender::StateSetDepthSlopeAndBias(float slope, float bias)
{
	InternalRenderManager.StateSetDepthSlopeAndBias(slope, bias);
}

void C4JRender::StateSetFogEnable(bool enable)
{
	InternalRenderManager.StateSetFogEnable(enable);
}

void C4JRender::StateSetFogMode(int mode)
{
	InternalRenderManager.StateSetFogMode(mode);
}

void C4JRender::StateSetFogNearDistance(float dist)
{
	InternalRenderManager.StateSetFogNearDistance(dist);
}

void C4JRender::StateSetFogFarDistance(float dist)
{
	InternalRenderManager.StateSetFogFarDistance(dist);
}

void C4JRender::StateSetFogDensity(float density)
{
	InternalRenderManager.StateSetFogDensity(density);
}

void C4JRender::StateSetFogColour(float red, float green, float blue)
{
	InternalRenderManager.StateSetFogColour(red, green, blue);
}

void C4JRender::StateSetLightingEnable(bool enable)
{
	InternalRenderManager.StateSetLightingEnable(enable);
}

void C4JRender::StateSetVertexTextureUV(float u, float v)
{
	InternalRenderManager.StateSetVertexTextureUV(u, v);
}

void C4JRender::StateSetLightColour(int light, float red, float green, float blue)
{
	InternalRenderManager.StateSetLightColour(light, red, green, blue);
}

void C4JRender::StateSetLightAmbientColour(float red, float green, float blue)
{
	InternalRenderManager.StateSetLightAmbientColour(red, green, blue);
}

void C4JRender::StateSetLightDirection(int light, float x, float y, float z)
{
	InternalRenderManager.StateSetLightDirection(light, x, y, z);
}

void C4JRender::StateSetLightEnable(int light, bool enable)
{
	InternalRenderManager.StateSetLightEnable(light, enable);
}

void C4JRender::StateSetViewport(eViewportType viewportType)
{
	InternalRenderManager.StateSetViewport(viewportType);
}

void C4JRender::StateSetEnableViewportClipPlanes(bool enable)
{
	InternalRenderManager.StateSetEnableViewportClipPlanes(enable);
}

void C4JRender::StateSetTexGenCol(int col, float x, float y, float z, float w, bool eyeSpace)
{
	InternalRenderManager.StateSetTexGenCol(col, x, y, z, w, eyeSpace);
}

void C4JRender::StateSetStencil(int Function, uint8_t stencil_ref, uint8_t stencil_func_mask, uint8_t stencil_write_mask)
{
	InternalRenderManager.StateSetStencil((D3D11_COMPARISON_FUNC)Function, stencil_ref, stencil_func_mask, stencil_write_mask);
}

void C4JRender::StateSetForceLOD(int LOD)
{
	InternalRenderManager.StateSetForceLOD(LOD);
}

void C4JRender::BeginEvent(LPCWSTR eventName)
{
	InternalRenderManager.BeginEvent(eventName);
}

void C4JRender::EndEvent()
{
	InternalRenderManager.EndEvent();
}

void C4JRender::Suspend()
{
	InternalRenderManager.Suspend();
}

bool C4JRender::Suspended()
{
	return InternalRenderManager.Suspended();
}

void C4JRender::Resume()
{
	InternalRenderManager.Resume();
}
