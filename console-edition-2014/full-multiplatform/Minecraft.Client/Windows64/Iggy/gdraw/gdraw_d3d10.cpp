// gdraw_d3d10.cpp - author: Fabian Giesen - copyright 2011 RAD Game Tools
//
// This implements the Iggy graphics driver layer for D3D 10.

// GDraw consists of several components that interact fairly loosely with each other;
// e.g. the resource management, drawing and filtering parts are all fairly independent
// of each other. If you want to modify some aspect of GDraw - say the texture allocation
// logic - your best bet is usually to just look for one of the related entry points,
// e.g. MakeTextureBegin, and take it from there. There's a bunch of code in this file,
// but none of it is really complicated.
//
// The one bit you might want to change that's not that localized is to integrate
// GDraw with an existing state caching system. The following bits all modify D3D state
// in some way:
// - The rendering helpers (set_viewport_raw, set_projection_raw, set_*_renderstate)
// - RenderTile*/TextureDrawBuffer* may change the active rendertarget and depth/stencil surface,
//   as do D3D1X_(NoMoreGDrawThisFrame) and set_render_target
// - set_texture
// - set_renderstate and set_renderstate_full. These are the main places where render state changes occur;
//   you should probably start here.
// - DrawIndexedTriangles sets the active vertex/index buffers and vertex declaration
// - Most of the functions in the "filter effects" section modify D3D state, mostly
//   pixel shader constants and textures

#define GDRAW_ASSERTS

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

// We temporarily disable this warning for the shared interface portions
#pragma warning (push)
#pragma warning (disable: 4201) // nonstandard extension used : nameless struct/union

#include <windows.h>
#include <d3d10.h>
#include "gdraw.h"
#include "iggy.h"
#include <string.h>
#include <math.h>

#include "gdraw_d3d10.h"

#pragma warning (pop)

// Some macros to allow as much sharing between D3D10 and D3D11 code as possible.
#define D3D1X_(id)         D3D10_##id
#define ID3D1X(id)         ID3D10##id
#define gdraw_D3D1X_(id)   gdraw_D3D10_##id
#define GDRAW_D3D1X_(id)   GDRAW_D3D10_##id

typedef ID3D10Device ID3D1XDevice;
typedef ID3D10Device ID3D1XContext;
typedef S32 ViewCoord;
typedef gdraw_d3d10_resourcetype gdraw_resourcetype;

static void report_d3d_error(HRESULT hr, char *call, char *context);

static void *map_buffer(ID3D1XContext *, ID3D10Buffer *buf, bool discard)
{
   void *ptr;
   if (FAILED(buf->Map(discard ? D3D10_MAP_WRITE_DISCARD : D3D10_MAP_WRITE_NO_OVERWRITE, 0, &ptr)))
      return NULL;
   else
      return ptr;
}

static void unmap_buffer(ID3D1XContext *, ID3D10Buffer *buf)
{
   buf->Unmap();
}

static RADINLINE void set_pixel_shader(ID3D10Device *ctx, ID3D10PixelShader *shader)
{
   ctx->PSSetShader(shader);
}

static RADINLINE void set_vertex_shader(ID3D10Device *ctx, ID3D10VertexShader *shader)
{
   ctx->VSSetShader(shader);
}

static ID3D10BlendState *create_blend_state(ID3D10Device *dev, BOOL blend, D3D10_BLEND src, D3D10_BLEND dst)
{
   D3D10_BLEND_DESC desc = {};
   desc.BlendEnable[0] = blend;
   desc.SrcBlend = src;
   desc.DestBlend = dst;
   desc.BlendOp = D3D10_BLEND_OP_ADD;
   desc.SrcBlendAlpha = (src == D3D10_BLEND_DEST_COLOR ) ? D3D10_BLEND_DEST_ALPHA : src;
   desc.DestBlendAlpha = dst;
   desc.BlendOpAlpha = D3D10_BLEND_OP_ADD;
   desc.RenderTargetWriteMask[0] = D3D10_COLOR_WRITE_ENABLE_ALL;

   ID3D10BlendState *res;
   HRESULT hr = dev->CreateBlendState(&desc, &res);
   if (FAILED(hr)) {
      report_d3d_error(hr, "CreateBlendState", "");
      res = NULL;
   }

   return res;
}

#define GDRAW_SHADER_FILE "gdraw_d3d10_shaders.inl"
#include "gdraw_d3d1x_shared.inl"

static void create_pixel_shader(ProgramWithCachedVariableLocations *p, ProgramWithCachedVariableLocations *src)
{
   *p = *src;
   if(p->bytecode) {
      HRESULT hr = gdraw->d3d_device->CreatePixelShader(p->bytecode, p->size, &p->pshader);
      if (FAILED(hr)) {
         report_d3d_error(hr, "CreatePixelShader", "");
         p->pshader = NULL;
         return;
      }
   }
}

static void create_vertex_shader(ProgramWithCachedVariableLocations *p, ProgramWithCachedVariableLocations *src)
{
   *p = *src;
   if(p->bytecode) {
      HRESULT hr = gdraw->d3d_device->CreateVertexShader(p->bytecode, p->size, &p->vshader);
      if (FAILED(hr)) {
         report_d3d_error(hr, "CreateVertexShader", "");
         p->vshader = NULL;
         return;
      }
   }
}

GDrawFunctions *gdraw_D3D10_CreateContext(ID3D10Device *dev, S32 w, S32 h)
{
   return create_context(dev, dev, w, h);
}

