// gdraw_d3d.cpp - author: Sean Barrett - copyright 2009-2011 RAD Game Tools
//
// This implements the Iggy graphics driver layer for Direct3D 9.

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
//   as do D3D_NoMoreGDrawThisFrame and set_render_target
// - set_texture
// - set_renderstate and set_renderstate_full. These are the main places where render state changes occur;
//   you should probably start here.
// - DrawIndexedTriangles sets the active vertex/index buffers and vertex declaration
// - Most of the functions in the "filter effects" section modify D3D state, mostly
//   pixel shader constants and textures

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <d3d9.h>
#include <string.h>
#include <math.h>

// We temporarily disable this warning for the shared interface portions
#pragma warning (push)
#pragma warning (disable: 4201) // nonstandard extension used : nameless struct/union

#include "gdraw.h"
#include "iggy.h"
#include "gdraw_d3d.h"

// The native handle type holds resource handles and a coarse description.
typedef union {
   // handle that is a texture
   struct {
      IDirect3DTexture9 *d3d;
      IDirect3DTexture9 *d3d_msaa;
      U32 w, h;
   } tex;

   // handle that is a vertex buffer
   struct {
      IDirect3DVertexBuffer9 *base;
      IDirect3DIndexBuffer9 *indices;
   } vbuf;
} GDrawNativeHandle;

#define GDRAW_D3D // this controls the next include so the cache gets declared with the correct types, which makes it typesafe
#include "gdraw_shared.inl"

#pragma warning (pop)

// max rendertarget stack depth. this depends on the extent to which you
// use filters and non-standard blend modes, and how nested they are.
#define MAX_RENDER_STACK_DEPTH             8         // Iggy is hardcoded to a limit of 16... probably 1-3 is realistic
#define AATEX_SAMPLER                      7         // sampler that aa_tex gets set in
#define QUAD_IB_COUNT                      2048      // quad index buffer has indices for this many quads

#define ASSERT_COUNT(a,b)                  ((a) == (b) ? (b) : -1)

static GDrawFunctions gdraw_funcs;

// render target state
typedef struct
{
   GDrawHandle *color_buffer;
   S32 base_x, base_y, width, height;
   U32 flags;
   rrbool cached;
} GDrawFramebufferState;

struct ProgramWithCachedVariableLocations
{
   DWORD *bytecode;
   union {
      IDirect3DPixelShader9  *pshader;
      IDirect3DVertexShader9 *vshader;
   };
   int vars[MAX_VARS]; // it's unsigned in d3d, but we want an 'undefined' value
};

///////////////////////////////////////////////////////////////////////////////
//
//     GDraw data structure
//
//
// This is the primary rendering abstraction, which hides all
// the platform-specific rendering behavior from Iggy. It is
// full of platform-specific graphics state, and also general
// graphics state so that it doesn't have to callback into Iggy
// to get at that graphics state.

typedef struct
{
   IDirect3DDevice9 *d3d_device;

   // fragment shaders
   ProgramWithCachedVariableLocations fprog[GDRAW_TEXTURE__count][3];
   ProgramWithCachedVariableLocations exceptional_blend[GDRAW_BLENDSPECIAL__count];
   ProgramWithCachedVariableLocations filter_prog[2][16];
   ProgramWithCachedVariableLocations blur_prog[MAX_TAPS+1];
   ProgramWithCachedVariableLocations colormatrix;
   ProgramWithCachedVariableLocations manual_clear;

   // vertex declarations
   IDirect3DVertexDeclaration9 *vdec[GDRAW_vformat__count];

   // vertex shaders
   ProgramWithCachedVariableLocations vert[GDRAW_vformat__count]; // [format]

   // render targets
   GDrawHandleCache rendertargets;
   GDrawHandle rendertarget_handles[MAX_RENDER_STACK_DEPTH]; // not -1, because we use +1 to initialize

   gswf_recti rt_valid[MAX_RENDER_STACK_DEPTH+1]; // valid rect for texture clamping

   // size of our render targets
   S32 frametex_width, frametex_height;

   // viewport setting (in pixels) for current frame
   S32 vx,vy;
   S32 fw,fh; // full width/height of virtual display
   S32 tw,th; // actual width/height of current tile
   S32 tpw,tph; // width/height of padded version of tile

   S32 tx0,ty0;
   S32 tx0p,ty0p;
   rrbool in_blur;

   F32 projection[4]; // scalex,scaley,transx,transy
   rrbool use_3d;
   F32 xform_3d[3][4];

   IDirect3DSurface9 *main_framebuffer;
   IDirect3DSurface9 *main_depthbuffer;
   IDirect3DSurface9 *rt_depthbuffer; // non-multisampled rendertarget depth buffer. only used when MSAA is on!
   rrbool main_msaa; // does main framebuffer have MSAA enabled?

   IDirect3DTexture9 *aa_tex;

   // scale factors converting worldspace to viewspace <0,0>..<w,h>
   F32 world_to_pixel[2];

   // cached state
   U32 scissor_state;      // ~0 if unknown, otherwise 0 or 1
   int blend_mode;         // -1 if unknown, otherwise GDRAW_BLEND_*
   U32 stencil_key;        // field built from stencil test flags. 0=no stencil, ~0 is used for "unknown state"
   U32 z_key;              // same for z-writes/z-test
   rrbool last_was_3d;

   // render-state stack described above for 'temporary' rendering
   GDrawFramebufferState frame[MAX_RENDER_STACK_DEPTH];
   GDrawFramebufferState *cur;

   // texture and vertex buffer pools
   GDrawHandleCache *texturecache;
   GDrawHandleCache *vbufcache;

   // mipmapping
   GDrawMipmapContext mipmap;
   rrbool conditional_nonpow2;

   // stat tracking
   rrbool frame_done;
   U64 frame_counter;

   // error reporting
   void (__cdecl *error_handler)(HRESULT hr);
} GDraw;

static GDraw *gdraw;

// not a real index buffer because we only get quads via user pointer
static U16 quad_ib[QUAD_IB_COUNT*6];


////////////////////////////////////////////////////////////////////////
//
//   Error handling
//

static void report_d3d_error(HRESULT hr, char *call, char *context)
{
   if (hr == E_OUTOFMEMORY)
      IggyGDrawSendWarning(NULL, "GDraw D3D out of memory in %s%s", call, context);
   else
      IggyGDrawSendWarning(NULL, "GDraw D3D error in %s%s: 0x%08x", call, context, hr);
}


////////////////////////////////////////////////////////////////////////
//
//   General resource management for both textures and vertex buffers
//

template<typename T>
static void safe_release(T *&p)
{
   if (p) {
      p->Release();
      p = NULL;
   }
}

static void unbind_resources(void)
{
   IDirect3DDevice9 *d3d = gdraw->d3d_device;
   S32 i;

   // unset active textures and vertex/index buffers,
   // to make sure there are no dangling refs
   for (i=0; i < 3; ++i)
      d3d->SetTexture(i, NULL);

   d3d->SetStreamSource(0, NULL, 0, 0);
   d3d->SetIndices(NULL);
}

static void api_free_resource(GDrawHandle *r)
{
   unbind_resources();
   if (r->state != GDRAW_HANDLE_STATE_user_owned) {
      if (!r->cache->is_vertex) {
         safe_release(r->handle.tex.d3d);
      } else {
         safe_release(r->handle.vbuf.base);
         safe_release(r->handle.vbuf.indices);
      }
   }
}

static void RADLINK gdraw_UnlockHandles(GDrawStats * /*stats*/)
{
   gdraw_HandleCacheUnlockAll(gdraw->texturecache);
   gdraw_HandleCacheUnlockAll(gdraw->vbufcache);
}

////////////////////////////////////////////////////////////////////////
//
//   Texture creation/updating/deletion
//

extern GDrawTexture *gdraw_D3D_WrappedTextureCreate(IDirect3DTexture9 *texhandle)
{
   GDrawStats stats={0};
   GDrawHandle *p = gdraw_res_alloc_begin(gdraw->texturecache, 0, &stats); // it may need to free one item to give us a handle
   p->handle.tex.d3d = texhandle;
   p->handle.tex.w = 1;
   p->handle.tex.h = 1;
   gdraw_HandleCacheAllocateEnd(p, 0, NULL, GDRAW_HANDLE_STATE_user_owned);
   return (GDrawTexture *) p;
}

extern void gdraw_D3D_WrappedTextureChange(GDrawTexture *tex, IDirect3DTexture9 *texhandle)
{
   GDrawHandle *p = (GDrawHandle *) tex;
   p->handle.tex.d3d = texhandle;
}

extern void gdraw_D3D_WrappedTextureDestroy(GDrawTexture *tex)
{
   GDrawStats stats={0};
   gdraw_res_free((GDrawHandle *) tex, &stats);
}

static void RADLINK gdraw_SetTextureUniqueID(GDrawTexture *tex, void *old_id, void *new_id)
{
   GDrawHandle *p = (GDrawHandle *) tex;
   // if this is still the handle it's thought to be, change the owner;
   // if the owner *doesn't* match, then they're changing a stale handle, so ignore
   if (p->owner == old_id)
      p->owner = new_id;
}


static rrbool RADLINK gdraw_MakeTextureBegin(void *owner, S32 width, S32 height, gdraw_texture_format format, U32 flags, GDraw_MakeTexture_ProcessingInfo *p, GDrawStats *stats)
{
   GDrawHandle *t = NULL;
   D3DFORMAT d3dfmt;
   S32 bpp;

   if (format == GDRAW_TEXTURE_FORMAT_rgba32) {
      d3dfmt = D3DFMT_A8R8G8B8;
      bpp = 4;
   } else {
      d3dfmt = D3DFMT_A8;
      bpp = 1;
   }

   // compute estimated size of texture in video memory
   S32 size = width*height*bpp;
   if (flags & GDRAW_MAKETEXTURE_FLAGS_mipmap)
      size = size*4/3; // not correct for non-square

   // allocate a handle and make room in the cache for this much data
   t = gdraw_res_alloc_begin(gdraw->texturecache, size, stats);
   if (!t) 
      return NULL;

   HRESULT hr = gdraw->d3d_device->CreateTexture(width, height, (flags & GDRAW_MAKETEXTURE_FLAGS_mipmap) ? 0 : 1,
                                         0, d3dfmt, D3DPOOL_MANAGED, &t->handle.tex.d3d, NULL);

   if (FAILED(hr)) {
      gdraw_HandleCacheAllocateFail(t);
      IggyGDrawSendWarning(NULL, "GDraw CreateTexture() call failed with error code 0x%08x", hr);
      return false;
   }

   t->handle.tex.w = width;
   t->handle.tex.h = height;

   gdraw_HandleCacheAllocateEnd(t, size, owner, (flags & GDRAW_MAKETEXTURE_FLAGS_never_flush) ? GDRAW_HANDLE_STATE_pinned : GDRAW_HANDLE_STATE_locked);
   stats->nonzero_flags |= GDRAW_STATS_alloc_tex;
   stats->alloc_tex += 1;
   stats->alloc_tex_bytes += size;

   p->texture_type = GDRAW_TEXTURE_TYPE_bgra;
   p->p0 = t;

   if (flags & GDRAW_MAKETEXTURE_FLAGS_mipmap) {
      rrbool ok;
      assert(p->temp_buffer != NULL);
      ok = gdraw_MipmapBegin(&gdraw->mipmap, width, height, t->handle.tex.d3d->GetLevelCount(), bpp, p->temp_buffer, p->temp_buffer_bytes);
      assert(ok); // this should never trigger unless the temp_buffer is way too small

      p->p1 = &gdraw->mipmap;
      p->texture_data = gdraw->mipmap.pixels[0];
      p->num_rows = gdraw->mipmap.bheight;
      p->stride_in_bytes = gdraw->mipmap.pitch[0];
      p->i0 = 0; // current output y
      p->i1 = bpp;
   } else {
      D3DLOCKED_RECT z;
      hr = t->handle.tex.d3d->LockRect(0, &z, NULL, 0);
      if (FAILED(hr)) {
         t->handle.tex.d3d->Release();
         gdraw_HandleCacheAllocateFail(t);

         if (hr == E_OUTOFMEMORY) {
            IggyGDrawSendWarning(NULL, "GDraw out of texture memory allocating %dx%d (%dbpp) texture", width, height, 8*bpp);
            return false;
         } else {
            IggyGDrawSendWarning(NULL, "GDraw LockRect for texture allocation failed, D3D error 0x%08x\n", hr);
            return false;
         }
      }
      
      p->p1 = NULL;
      p->texture_data = (U8 *) z.pBits;
      p->num_rows = height;
      p->stride_in_bytes = z.Pitch;
   }

   return true;
}

static rrbool RADLINK gdraw_MakeTextureMore(GDraw_MakeTexture_ProcessingInfo *p)
{
   GDrawHandle *t = (GDrawHandle *) p->p0;
   
   if (p->p1) {
      GDrawMipmapContext *c = (GDrawMipmapContext *) p->p1;
      U32 outy = p->i0;
      U32 bpp = p->i1;
      U32 width = c->width;
      U32 height = c->height;
      U32 bheight = c->bheight;
      U32 level = 0;

      if (outy >= c->height)
         return false;

      do {
         // upload data for this miplevel
         D3DLOCKED_RECT z;
         HRESULT hr = t->handle.tex.d3d->LockRect(level, &z, NULL, 0);
         if (FAILED(hr))
            return false;

         for (U32 y=0; y < bheight; ++y)
            memcpy((U8 *) z.pBits + ((outy >> level) + y) * z.Pitch, c->pixels[level] + y * c->pitch[level], width * bpp);
         t->handle.tex.d3d->UnlockRect(level);

         // prepare next miplevel
         width = RR_MAX(width >> 1, 1);
         height = RR_MAX(height >> 1, 1);
         bheight = RR_MAX(bheight >> 1, 1);
      } while (gdraw_MipmapAddLines(c, ++level));

      // prepare next chunk
      p->i0 += p->num_rows;
      p->texture_data = c->pixels[0];
      p->num_rows = c->bheight = RR_MIN(c->bheight, c->height - p->i0);
      return true;
   } else
      return false;
}

static GDrawTexture * RADLINK gdraw_MakeTextureEnd(GDraw_MakeTexture_ProcessingInfo *p, GDrawStats * /*stats*/)
{
   GDrawHandle *t = (GDrawHandle *) p->p0;
   if (p->p1)
      gdraw_MakeTextureMore(p); // use more to upload the last batch of data
   else
      t->handle.tex.d3d->UnlockRect(0);

   return (GDrawTexture *) t;
}


static rrbool RADLINK gdraw_UpdateTextureBegin(GDrawTexture *t, void *unique_id, GDrawStats * /*stats*/)
{
   return gdraw_HandleCacheLock((GDrawHandle *) t, unique_id);
}

static void RADLINK gdraw_UpdateTextureRect(GDrawTexture *t, void * /*unique_id*/, S32 x, S32 y, S32 stride, S32 w, S32 h, U8 *samples, gdraw_texture_format format)
{
   GDrawHandle *s = (GDrawHandle *) t;
   RECT rdest = { x, y, x+w, y+h };
   S32 i, bpl = (format == GDRAW_TEXTURE_FORMAT_font ? 1 : 4) * w;
   D3DLOCKED_RECT lr;

   HRESULT hr = s->handle.tex.d3d->LockRect(0, &lr, &rdest, 0);
   if (FAILED(hr)) {
      IggyGDrawSendWarning(0, "GDraw LockRect() for texture update failed; D3D error 0x%08x", hr);
      return;
   }

   for (i=0; i < h; i++)
      memcpy((U8 *) lr.pBits + i * lr.Pitch, samples + i*stride, bpl);

   s->handle.tex.d3d->UnlockRect(0);
}

static void RADLINK gdraw_UpdateTextureEnd(GDrawTexture *t, void * /*unique_id*/, GDrawStats * /*stats*/)
{
   gdraw_HandleCacheUnlock((GDrawHandle *) t);
}

static void RADLINK gdraw_FreeTexture(GDrawTexture *tt, void *unique_id, GDrawStats *stats)
{
   GDrawHandle *t = (GDrawHandle *) tt;
   assert(t != NULL); // @GDRAW_ASSERT
   if (t->owner == unique_id || unique_id == NULL) {
      if (t->cache == &gdraw->rendertargets) {
         gdraw_HandleCacheUnlock(t);
         // cache it by simply not freeing it
         return;
      }

      gdraw_res_free(t, stats);
   }
}

static rrbool RADLINK gdraw_TryToLockTexture(GDrawTexture *t, void *unique_id, GDrawStats * /*stats*/)
{
   return gdraw_HandleCacheLock((GDrawHandle *) t, unique_id);
}

static void RADLINK gdraw_DescribeTexture(GDrawTexture *tex, GDraw_Texture_Description *desc)
{
   GDrawHandle *p = (GDrawHandle *) tex;
   desc->width = p->handle.tex.w;
   desc->height = p->handle.tex.h;
   desc->size_in_bytes = p->bytes;
}

static void RADLINK gdraw_SetAntialiasTexture(S32 width, U8 *rgba)
{
   HRESULT hr;
   D3DLOCKED_RECT lr;
   S32 i;
   U8 *d;

   safe_release(gdraw->aa_tex); // release the old texture, if any.

   hr = gdraw->d3d_device->CreateTexture(width, 1, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &gdraw->aa_tex, NULL);
   if (FAILED(hr)) {
      IggyGDrawSendWarning(0, "GDraw D3D error in CreateTexture 0x%08x", hr);
      return;
   }

   hr = gdraw->aa_tex->LockRect(0, &lr, NULL, 0);
   if (!FAILED(hr)) {
      d = (U8 *) lr.pBits;
      for (i=0; i < width; i++) {
         d[i*4+0] = rgba[i*4+2];
         d[i*4+1] = rgba[i*4+1];
         d[i*4+2] = rgba[i*4+0];
         d[i*4+3] = rgba[i*4+3];
      }

      gdraw->aa_tex->UnlockRect(0);
   } else
      IggyGDrawSendWarning(0, "GDraw D3D error in LockRect for texture creation: 0x%08x", hr);
}

////////////////////////////////////////////////////////////////////////
//
//   Vertex buffer creation/deletion
//

static rrbool RADLINK gdraw_MakeVertexBufferBegin(void *unique_id, gdraw_vformat /*vformat*/, S32 vbuf_size, S32 ibuf_size, GDraw_MakeVertexBuffer_ProcessingInfo *p, GDrawStats *stats)
{
   char *failed_call;
   GDrawHandle *vb = gdraw_res_alloc_begin(gdraw->vbufcache, vbuf_size + ibuf_size, stats);
   if (!vb)
      return false;

   vb->handle.vbuf.base = NULL;
   vb->handle.vbuf.indices = NULL;

   HRESULT hr;
   hr = gdraw->d3d_device->CreateVertexBuffer(vbuf_size, D3DUSAGE_WRITEONLY, 0, D3DPOOL_MANAGED, &vb->handle.vbuf.base, NULL);
   failed_call = "CreateVertexBuffer";
   if (!FAILED(hr)) {
      hr = gdraw->d3d_device->CreateIndexBuffer(ibuf_size, D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_MANAGED, &vb->handle.vbuf.indices, NULL);
      failed_call = "CreateIndexBuffer";
   }
   if (!FAILED(hr)) {
      hr = vb->handle.vbuf.base->Lock(0, vbuf_size, (void **) &p->vertex_data, 0);
      failed_call = "Lock";
   }
   if (!FAILED(hr))
      hr = vb->handle.vbuf.indices->Lock(0, ibuf_size, (void **) &p->index_data, 0);

   if (FAILED(hr)) {
      if (vb->handle.vbuf.base)
         vb->handle.vbuf.base->Unlock(); // does nothing if we didn't actually lock

      safe_release(vb->handle.vbuf.base);
      safe_release(vb->handle.vbuf.indices);

      gdraw_HandleCacheAllocateFail(vb);

      report_d3d_error(hr, failed_call, " creating vertex buffer");
      return false;
   }

   p->vertex_data_length = vbuf_size;
   p->index_data_length = ibuf_size;
   p->p0 = vb;

   gdraw_HandleCacheAllocateEnd(vb, vbuf_size + ibuf_size, unique_id, GDRAW_HANDLE_STATE_locked);
   return true;
}

static rrbool RADLINK gdraw_MakeVertexBufferMore(GDraw_MakeVertexBuffer_ProcessingInfo * /*p*/)
{
   assert(0);
   return false;
}

static GDrawVertexBuffer * RADLINK gdraw_MakeVertexBufferEnd(GDraw_MakeVertexBuffer_ProcessingInfo *p, GDrawStats * /*stats*/)
{
   GDrawHandle *vb = (GDrawHandle *) p->p0;
   vb->handle.vbuf.base->Unlock();
   vb->handle.vbuf.indices->Unlock();
   return (GDrawVertexBuffer *) vb;
}

static rrbool RADLINK gdraw_TryLockVertexBuffer(GDrawVertexBuffer *vb, void *unique_id, GDrawStats * /*stats*/)
{
   return gdraw_HandleCacheLock((GDrawHandle *) vb, unique_id);
}

static void RADLINK gdraw_FreeVertexBuffer(GDrawVertexBuffer *vb, void *unique_id, GDrawStats *stats)
{
   GDrawHandle *h = (GDrawHandle *) vb;
   assert(h != NULL); // @GDRAW_ASSERT
   if (h->owner == unique_id)
      gdraw_res_free(h, stats);
}

static void RADLINK gdraw_DescribeVertexBuffer(GDrawVertexBuffer *vbuf, GDraw_VertexBuffer_Description *desc)
{
   GDrawHandle *p = (GDrawHandle *) vbuf;
   desc->size_in_bytes = p->bytes;
}

////////////////////////////////////////////////////////////////////////
//
//   Create/free (or cache) render targets
//

static GDrawHandle *get_color_rendertarget(GDrawStats *stats)
{
   // try to recycle LRU rendertarget
   GDrawHandle *t = gdraw_HandleCacheGetLRU(&gdraw->rendertargets);
   if (t) {
      gdraw_HandleCacheLock(t, (void *) 1);
      return t;
   }

   // ran out of RTs, allocate a new one
   S32 size = gdraw->frametex_width * gdraw->frametex_height * 4;
   if (gdraw->rendertargets.bytes_free < size) {
      IggyGDrawSendWarning(NULL, "GDraw rendertarget allocation failed: hit size limit of %d bytes", gdraw->rendertargets.total_bytes);
      return NULL;
   }

   t = gdraw_HandleCacheAllocateBegin(&gdraw->rendertargets);
   if (!t) {
      IggyGDrawSendWarning(NULL, "GDraw rendertarget allocation failed: hit handle limit");
      return t;
   }

   HRESULT hr = gdraw->d3d_device->CreateTexture(gdraw->frametex_width, gdraw->frametex_height, 1, D3DUSAGE_RENDERTARGET,
      D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &t->handle.tex.d3d, NULL);
   if (FAILED(hr)) {
      report_d3d_error(hr, "CreateTexture", " creating rendertarget");
      gdraw_HandleCacheAllocateFail(t);
      return NULL;
   }

   gdraw_HandleCacheAllocateEnd(t, size, (void *) 1, GDRAW_HANDLE_STATE_locked);
   stats->nonzero_flags |= GDRAW_STATS_alloc_tex;
   stats->alloc_tex += 1;
   stats->alloc_tex_bytes += size;

   return t;
}

static IDirect3DSurface9 *get_rendertarget_depthbuffer(GDrawStats *stats)
{
   if (!gdraw->rt_depthbuffer) {
      HRESULT hr = gdraw->d3d_device->CreateDepthStencilSurface(gdraw->frametex_width, gdraw->frametex_height, D3DFMT_D24S8, D3DMULTISAMPLE_NONE, 0, TRUE, &gdraw->rt_depthbuffer, NULL);
      if (FAILED(hr))
         IggyGDrawSendWarning(NULL, "GDraw D3D error in CreateDepthStencilSurface: 0x%08x", hr);
      else {
         stats->nonzero_flags |= GDRAW_STATS_alloc_tex;
         stats->alloc_tex += 1;
         stats->alloc_tex_bytes += gdraw->frametex_width * gdraw->frametex_height * 4;
      }
   }

   return gdraw->rt_depthbuffer;
}

static void flush_rendertargets(GDrawStats *stats)
{
   gdraw_res_flush(&gdraw->rendertargets, stats);
   safe_release(gdraw->rt_depthbuffer);
}

////////////////////////////////////////////////////////////////////////
//
//   Vertex shader constants
//

#define VVAR_world0           0
#define VVAR_world1           1
#define VVAR_count_worldonly  2 // number of constants to send if you only changed world matrix

#define VVAR_x_off            2
#define VVAR_count_world_xoff 3 // number of constants to send if you changed world+x_off

#define VVAR_texgen_s         3
#define VVAR_texgen_t         4
#define VVAR_count            5 // number of constants to send if you changed all per-batch state

#define VVAR_x3d              5
#define VVAR_y3d              6
#define VVAR_z3d              7
#define VVAR_count3d          8

// Fixed-location pixel shader constants
#define PVAR_cmul             0
#define PVAR_cadd             1
#define PVAR_focal            2
#define PVAR_rescale1         3

struct VertexVars
{
   F32 world[2][4];
   F32 x_off[4];
   F32 texgen_s[4];
   F32 texgen_t[4];
   F32 viewproj[3][4];
};

////////////////////////////////////////////////////////////////////////
//
//   Rendering helpers
//

static void set_d3d_texture(U32 sampler, IDirect3DTexture9 *tex, U32 wrap, U32 nearest)
{
   static const int addrmodes[ASSERT_COUNT(GDRAW_WRAP__count, 4)] = {
      D3DTADDRESS_CLAMP,   // GDRAW_WRAP_clamp
      D3DTADDRESS_WRAP,    // GDRAW_WRAP_repeat
      D3DTADDRESS_MIRROR,  // GDRAW_WRAP_mirror
      D3DTADDRESS_CLAMP,   // GDRAW_WRAP_clamp_to_border (never used by client code!)
   };

   static const int filtermodes[2] = {
      D3DTEXF_LINEAR,      // !nearest
      D3DTEXF_POINT,       // nearest
   };

   assert(wrap < sizeof(addrmodes) / sizeof(addrmodes[0]));
   assert(nearest < sizeof(filtermodes) / sizeof(filtermodes[0]));
   IDirect3DDevice9 *d3d = gdraw->d3d_device;

   d3d->SetTexture(sampler, tex);
   d3d->SetSamplerState(sampler, D3DSAMP_MAGFILTER, filtermodes[nearest]);
   d3d->SetSamplerState(sampler, D3DSAMP_ADDRESSU, addrmodes[wrap]);
   d3d->SetSamplerState(sampler, D3DSAMP_ADDRESSV, addrmodes[wrap]);
}

static void set_viewport_raw(S32 x, S32 y, S32 w, S32 h)
{
   D3DVIEWPORT9 vp = { x, y, w, h, 0.0f, 1.0f };
   gdraw->d3d_device->SetViewport(&vp);
}

static void set_projection_base(void)
{
   F32 m[3][4] = { 0 };

   //    x3d = < viewproj.x, 0, 0, 0 >
   //    y3d = < 0, viewproj.y, 0, 0 >
   //    w3d = < viewproj.z, viewproj.w, 1.0, 1.0 >

   m[0][0] = gdraw->projection[0];
   m[1][1] = gdraw->projection[1];
   m[2][0] = gdraw->projection[2];
   m[2][1] = gdraw->projection[3];

   m[2][2] = 1.0;
   m[2][3] = 1.0;

   gdraw->d3d_device->SetVertexShaderConstantF(VVAR_x3d, m[0], 3);
}

static void set_projection_raw(S32 x0, S32 x1, S32 y0, S32 y1)
{
   gdraw->projection[0] = 2.0f / (x1-x0);
   gdraw->projection[1] = 2.0f / (y1-y0);
   gdraw->projection[2] = (x1+x0)/(F32)(x0-x1) - 0.5f * gdraw->projection[0]; // -0.5f: convert from D3D9 to GL/D3D10 pixel coordinates
   gdraw->projection[3] = (y1+y0)/(F32)(y0-y1) - 0.5f * gdraw->projection[1];

   set_projection_base();
}

static void set_viewport(void)
{
   if (gdraw->in_blur) {
      set_viewport_raw(0, 0, gdraw->tpw, gdraw->tph);
      return;
   }

   if (gdraw->cur == gdraw->frame) // if the rendering stack is empty
      // render a tile-sized region to the user-request tile location
      set_viewport_raw(gdraw->vx, gdraw->vy, gdraw->tw, gdraw->th);
   else if (gdraw->cur->cached)
      set_viewport_raw(0, 0, gdraw->cur->width, gdraw->cur->height);
   else
      // if on the render stack, draw a padded-tile-sized region at the origin
      set_viewport_raw(0, 0, gdraw->tpw, gdraw->tph);
}

static void set_projection(void)
{
   if (gdraw->in_blur) return;
   if (gdraw->cur == gdraw->frame) // if the render stack is empty
      set_projection_raw(gdraw->tx0, gdraw->tx0+gdraw->tw, gdraw->ty0+gdraw->th, gdraw->ty0);
   else if (gdraw->cur->cached)
      set_projection_raw(gdraw->cur->base_x, gdraw->cur->base_x+gdraw->cur->width, gdraw->cur->base_y, gdraw->cur->base_y+gdraw->cur->height);
   else
      set_projection_raw(gdraw->tx0p, gdraw->tx0p+gdraw->tpw, gdraw->ty0p+gdraw->tph, gdraw->ty0p);
}

static void set_common_renderstate()
{
   IDirect3DDevice9 *d3d = gdraw->d3d_device;
   S32 i;

   // all the render states we never change while drawing
   d3d->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
   d3d->SetRenderState(D3DRS_STENCILFAIL, D3DSTENCILOP_KEEP);
   d3d->SetRenderState(D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP);
   d3d->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_REPLACE);
   d3d->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_EQUAL);
   d3d->SetRenderState(D3DRS_STENCILREF,  255);
   d3d->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
   d3d->SetRenderState(D3DRS_SLOPESCALEDEPTHBIAS, 0 ); 
   d3d->SetRenderState(D3DRS_DEPTHBIAS, 0 );
   d3d->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
   d3d->SetRenderState(D3DRS_SEPARATEALPHABLENDENABLE, FALSE);

   for (i=0; i < 3; i++) {
      d3d->SetTextureStageState(i, D3DTSS_TEXCOORDINDEX, i);
      d3d->SetTextureStageState(i, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);
      d3d->SetSamplerState(i, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
      d3d->SetSamplerState(i, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
      d3d->SetSamplerState(i, D3DSAMP_MIPMAPLODBIAS, 0);
      d3d->SetSamplerState(i, D3DSAMP_MAXMIPLEVEL, 0);
   }

   d3d->SetTextureStageState(AATEX_SAMPLER, D3DTSS_TEXCOORDINDEX, AATEX_SAMPLER);

   d3d->SetTexture(AATEX_SAMPLER, gdraw->aa_tex);
   d3d->SetSamplerState(AATEX_SAMPLER, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
   d3d->SetSamplerState(AATEX_SAMPLER, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
   d3d->SetSamplerState(AATEX_SAMPLER, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
   d3d->SetSamplerState(AATEX_SAMPLER, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
   d3d->SetSamplerState(AATEX_SAMPLER, D3DSAMP_MIPMAPLODBIAS, 0);
   d3d->SetSamplerState(AATEX_SAMPLER, D3DSAMP_MAXMIPLEVEL, 0);

   // reset our state caching
   gdraw->scissor_state = ~0u;
   gdraw->blend_mode = -1;
   gdraw->stencil_key = ~0u;
   gdraw->z_key = ~0u;

   VertexVars vvars = { 0 };
   d3d->SetVertexShaderConstantF(0, vvars.world[0], VVAR_count);
}

static void clear_renderstate(void)
{
   IDirect3DDevice9 *d3d = gdraw->d3d_device;

   d3d->SetTexture(0, NULL);
   d3d->SetTexture(1, NULL);
   d3d->SetTexture(2, NULL);
   d3d->SetTexture(AATEX_SAMPLER, NULL);
   d3d->SetStreamSource(0, NULL, 0, 0);
   d3d->SetIndices(NULL);

   d3d->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
   d3d->SetRenderState(D3DRS_ZFUNC, D3DCMP_ALWAYS);
   d3d->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
   d3d->SetRenderState(D3DRS_COLORWRITEENABLE, 0xf);
   d3d->SetRenderState(D3DRS_STENCILENABLE, FALSE);
   d3d->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);

   d3d->SetVertexShader(0);
   d3d->SetPixelShader(0);
   d3d->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
   d3d->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
}

static void clear_zbuffer(IDirect3DSurface9 *surf, U32 flags)
{
   IDirect3DDevice9 *d3d = gdraw->d3d_device;
   IDirect3DSurface9 *target;
   D3DSURFACE_DESC desc;
   surf->GetDesc(&desc);

   // here's where it gets stupid: we need a rendertarget that's
   // big enough so we can actually clear the full z-buffer.
   // we don't actually render to it or anything, we just need to
   // set it...
   target = gdraw->main_framebuffer;
   if (surf != gdraw->main_depthbuffer) {
      // tile surface could, in theory, be larger than main framebuffer, so
      // check if we have at least one rendertarget matching the rt depth
      // buffer created and use that while clearing if possible
      S32 i;
      for (i=0; i < MAX_RENDER_STACK_DEPTH; ++i)
         if (gdraw->rendertargets.handle[i].handle.tex.d3d) {
            gdraw->rendertargets.handle[i].handle.tex.d3d->GetSurfaceLevel(0, &target);
            break;
         }
   }

   D3DVIEWPORT9 vp = { 0, 0, desc.Width, desc.Height, 0.0f, 1.0f };
   d3d->SetRenderTarget(0, target);
   d3d->SetDepthStencilSurface(surf);
   d3d->SetViewport(&vp);
   d3d->Clear(0, NULL, flags, 0, 1.0f, 0);

   if (target != gdraw->main_framebuffer)
      target->Release();
}

////////////////////////////////////////////////////////////////////////
//
//   Begin/end rendering of a tile and per-frame processing
//

void gdraw_D3D_SetTileOrigin(IDirect3DSurface9 *rt, IDirect3DSurface9 *depth, S32 x, S32 y)
{
   D3DSURFACE_DESC desc;

   if (gdraw->frame_done) {
      ++gdraw->frame_counter;
      gdraw->frame_done = false;
   }

   rt->GetDesc(&desc);

   gdraw->main_framebuffer = rt;
   gdraw->main_depthbuffer = depth;
   gdraw->main_msaa = (desc.MultiSampleType != D3DMULTISAMPLE_NONE);
   gdraw->vx = x;
   gdraw->vy = y;
}

static void RADLINK gdraw_SetViewSizeAndWorldScale(S32 w, S32 h, F32 scale_x, F32 scale_y)
{
   memset(gdraw->frame, 0, sizeof(gdraw->frame));
   gdraw->cur = gdraw->frame;
   gdraw->fw = w;
   gdraw->fh = h;
   gdraw->tw = w;
   gdraw->th = h;
   gdraw->world_to_pixel[0] = scale_x;
   gdraw->world_to_pixel[1] = scale_y;

   set_viewport();
}

static void RADLINK gdraw_Set3DTransform(F32 *mat)
{
   if (mat == NULL)
      gdraw->use_3d = 0;
   else {
      gdraw->use_3d = 1;
      memcpy(gdraw->xform_3d, mat, sizeof(gdraw->xform_3d));
   }
}


// must include anything necessary for texture creation/update
static void RADLINK gdraw_RenderingBegin(void)
{
}
static void RADLINK gdraw_RenderingEnd(void)
{
}

static void RADLINK gdraw_RenderTileBegin(S32 x0, S32 y0, S32 x1, S32 y1, S32 pad, GDrawStats *stats)
{
   if (x0 == 0 && y0 == 0 && x1 == gdraw->fw && y1 == gdraw->fh)
      pad = 0;

   gdraw->tx0 = x0;
   gdraw->ty0 = y0;
   gdraw->tw = x1-x0;
   gdraw->th = y1-y0;

   // padded region
   gdraw->tx0p = RR_MAX(x0 - pad, 0);
   gdraw->ty0p = RR_MAX(y0 - pad, 0);
   gdraw->tpw = RR_MIN(x1 + pad, gdraw->fw) - gdraw->tx0p;
   gdraw->tph = RR_MIN(y1 + pad, gdraw->fh) - gdraw->ty0p;

   // just record the max, but then when we texture from, we have to use
   // sub-regions -- alternatively, each gdraw gets its own rendertargets
   if (gdraw->tpw > gdraw->frametex_width || gdraw->tph > gdraw->frametex_height) {
      gdraw->frametex_width  = RR_MAX(gdraw->tpw, gdraw->frametex_width);
      gdraw->frametex_height = RR_MAX(gdraw->tph, gdraw->frametex_height);

      flush_rendertargets(stats);
   }

   // clear our depth buffers
   clear_zbuffer(gdraw->main_depthbuffer, D3DCLEAR_STENCIL | D3DCLEAR_ZBUFFER);
   if (gdraw->rt_depthbuffer)
      clear_zbuffer(gdraw->rt_depthbuffer, D3DCLEAR_STENCIL | D3DCLEAR_ZBUFFER);

   // set our rendertarget
   gdraw->d3d_device->SetRenderTarget(0, gdraw->main_framebuffer);
   gdraw->d3d_device->SetDepthStencilSurface(gdraw->main_depthbuffer);
   set_viewport();
   set_projection();
   set_common_renderstate();
}

static void RADLINK gdraw_RenderTileEnd(GDrawStats * /*stats*/)
{
}

void gdraw_D3D_SetRendertargetSize(S32 w, S32 h)
{
   if (gdraw && (w != gdraw->frametex_width || h != gdraw->frametex_height)) {
      GDrawStats stats = { 0 };
      gdraw->frametex_width = w;
      gdraw->frametex_height = h;
      flush_rendertargets(&stats);
   }
}

void gdraw_D3D_NoMoreGDrawThisFrame(void)
{
   clear_renderstate();
   if (gdraw->main_framebuffer)
      gdraw->d3d_device->SetRenderTarget(0, gdraw->main_framebuffer);
   if (gdraw->main_depthbuffer)
      gdraw->d3d_device->SetDepthStencilSurface(gdraw->main_depthbuffer);
   gdraw->frame_done = true;

   GDrawFence now = { gdraw->frame_counter };
   gdraw_HandleCacheTick(gdraw->texturecache, now);
   gdraw_HandleCacheTick(gdraw->vbufcache, now);
}

#define MAX_DEPTH_VALUE   (1 << 13)

static void RADLINK gdraw_GetInfo(GDrawInfo *d)
{
   D3DCAPS9 caps;
   gdraw->d3d_device->GetDeviceCaps(&caps);

   d->num_stencil_bits = 8;
   d->max_id = MAX_DEPTH_VALUE-2;
   // for floating point depth, just use mantissa, e.g. 16-20 bits
   d->max_texture_size = RR_MIN(caps.MaxTextureWidth, caps.MaxTextureHeight);
   d->buffer_format = GDRAW_BFORMAT_vbib;
   d->shared_depth_stencil = 1;
   d->always_mipmap = 1;
   d->conditional_nonpow2 = (caps.TextureCaps & D3DPTEXTURECAPS_NONPOW2CONDITIONAL) != 0;
}

////////////////////////////////////////////////////////////////////////
//
//   Enable/disable rendertargets in stack fashion
//

static void set_render_target(GDrawStats *stats)
{
   IDirect3DSurface9 *target = NULL, *depth = NULL;

   if (gdraw->cur->color_buffer) {
      S32 need_depth;
      need_depth = (gdraw->cur->flags & (GDRAW_TEXTUREDRAWBUFFER_FLAGS_needs_id | GDRAW_TEXTUREDRAWBUFFER_FLAGS_needs_stencil));

      unbind_resources(); // to make sure this RT isn't accidentally set as a texture (avoid D3D warnings)
      gdraw->cur->color_buffer->handle.tex.d3d->GetSurfaceLevel(0, &target);  // if this fails, there's nothing to be done
      if (need_depth) {
         if (gdraw->main_msaa)
            depth = get_rendertarget_depthbuffer(stats); // @TODO: is this right? get_rt_depthbuffer doesn't seem to do MSAA
         else {
            // if tile is smaller than frametex, then trying to use the tile's zbuffer may not work
            if (gdraw->tw < gdraw->frametex_width || gdraw->th < gdraw->frametex_height)
               depth = get_rendertarget_depthbuffer(stats);
            else
               depth = gdraw->main_depthbuffer;
         }
      }
   } else {
      target = gdraw->main_framebuffer;
      depth = gdraw->main_depthbuffer;
   }

   gdraw->d3d_device->SetRenderTarget(0, target);
   gdraw->d3d_device->SetDepthStencilSurface(depth);
   if (target != gdraw->main_framebuffer)
      target->Release();

   stats->nonzero_flags |= GDRAW_STATS_rendtarg;
   stats->rendertarget_changes += 1;
}

static rrbool RADLINK gdraw_TextureDrawBufferBegin(gswf_recti *region, gdraw_texture_format /*format*/, U32 flags, void *owner, GDrawStats *stats)
{
   GDrawFramebufferState *n = gdraw->cur+1;
   GDrawHandle *t;
   if (gdraw->tw == 0 || gdraw->th == 0)
      return false;

   if (n >= &gdraw->frame[MAX_RENDER_STACK_DEPTH]) {
      IggyGDrawSendWarning(NULL, "GDraw rendertarget nesting exceeds MAX_RENDER_STACK_DEPTH");
      return false;
   }

   if (owner) {
      S32 w = region->x1 - region->x0;
      S32 h = region->y1 - region->y0;

      // allocate a texture handle and free enough texture space
      t = gdraw_res_alloc_begin(gdraw->texturecache, w*h*4, stats);
      if (!t)
         return false;
      else {
         IDirect3DTexture9 * tex;
         HRESULT hr = gdraw->d3d_device->CreateTexture(w,h,1,
                                         D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8,
                                             D3DPOOL_DEFAULT, &tex, NULL);

         if (FAILED(hr)) {
            if (t)
               gdraw_HandleCacheAllocateFail(t);
            IggyGDrawSendWarning(NULL, "GDraw D3D error for CreateTexture for cacheAsBitmap rendertarget: 0x%08x", hr);
            return false;
         }

         stats->nonzero_flags |= GDRAW_STATS_alloc_tex;
         stats->alloc_tex += 1;
         stats->alloc_tex_bytes += w*h*4;

         gdraw_HandleCacheAllocateEnd(t, w*h*4, (void *) 1, GDRAW_HANDLE_STATE_locked);
      }
   } else {
      t = get_color_rendertarget(stats);
      if (!t)
         return false;
   }

   n->flags = flags;
   n->color_buffer = t;
   assert(n->color_buffer != NULL); // @GDRAW_ASSERT

   ++gdraw->cur;
   gdraw->cur->cached = owner != NULL;
   if (owner) {
      gdraw->cur->base_x = region->x0;
      gdraw->cur->base_y = region->y0;
      gdraw->cur->width  = region->x1 - region->x0;
      gdraw->cur->height = region->y1 - region->y0;
   }

   set_render_target(stats);
   assert(gdraw->frametex_width >= gdraw->tw && gdraw->frametex_height >= gdraw->th); // @GDRAW_ASSERT
   set_viewport();
   set_projection();

   int k = (int) (t - gdraw->rendertargets.handle);

   if (region) {
      D3DRECT r;
      S32 ox, oy, pad = 2; // 2 pixels of border on all sides
      // 1 pixel turns out to be not quite enough with the interpolator precision we get.

      if (gdraw->in_blur)
         ox = oy = 0;
      else
         ox = gdraw->tx0p, oy = gdraw->ty0p;

      // clamp region to tile
      S32 xt0 = RR_MAX(region->x0 - ox, 0);
      S32 yt0 = RR_MAX(region->y0 - oy, 0);
      S32 xt1 = RR_MIN(region->x1 - ox, gdraw->tpw);
      S32 yt1 = RR_MIN(region->y1 - oy, gdraw->tph);

      // but the padding needs to clamp to render target bounds
      r.x1 = RR_MAX(xt0 - pad, 0);
      r.y1 = RR_MAX(yt0 - pad, 0);
      r.x2 = RR_MIN(xt1 + pad, gdraw->frametex_width);
      r.y2 = RR_MIN(yt1 + pad, gdraw->frametex_height);

      if (r.x2 <= r.x1 || r.y2 <= r.y1) { // region doesn't intersect with current tile
         --gdraw->cur;
         gdraw_FreeTexture((GDrawTexture *) t, 0, stats);
         // note: don't send a warning since this will happen during regular tiled rendering
         return false;
      }

      gdraw->d3d_device->Clear(1, &r, D3DCLEAR_TARGET, 0, 1, 0);
      gdraw->rt_valid[k].x0 = xt0;
      gdraw->rt_valid[k].y0 = yt0;
      gdraw->rt_valid[k].x1 = xt1;
      gdraw->rt_valid[k].y1 = yt1;
   } else {
      gdraw->d3d_device->Clear(0, NULL, D3DCLEAR_TARGET, 0, 1, 0); 
      gdraw->rt_valid[k].x0 = 0;
      gdraw->rt_valid[k].y0 = 0;
      gdraw->rt_valid[k].x1 = gdraw->frametex_width;
      gdraw->rt_valid[k].y1 = gdraw->frametex_height;
   }

   return true;
}

static GDrawTexture *RADLINK gdraw_TextureDrawBufferEnd(GDrawStats *stats)
{
   GDrawFramebufferState *n =   gdraw->cur;
   GDrawFramebufferState *m = --gdraw->cur;

   if (gdraw->tw == 0 || gdraw->th == 0) return 0;

   if (n >= &gdraw->frame[MAX_RENDER_STACK_DEPTH])
      return 0; // already returned a warning in Begin

   assert(m >= gdraw->frame);  // bug in Iggy -- unbalanced

   if (m != gdraw->frame) {
      assert(m->color_buffer != NULL); // @GDRAW_ASSERT
   }
   assert(n->color_buffer != NULL); // @GDRAW_ASSERT

   // switch back to old render target
   set_render_target(stats);

   // if we're at the root, set the viewport back
   set_viewport();
   set_projection();

   return (GDrawTexture *) n->color_buffer;
}


////////////////////////////////////////////////////////////////////////
//
//   Clear stencil/depth buffers
//
// Open question whether we'd be better off finding bounding boxes
// and only clearing those; it depends exactly how fast clearing works.
//

static void clear_renderstate_acceleration_cache(void)
{
   gdraw->last_was_3d = false;
   gdraw->scissor_state = ~0u;
   gdraw->stencil_key = 0;
   gdraw->blend_mode = ~0u;
}

static void do_screen_quad(gswf_recti *s, F32 *tc, GDrawStats *stats);

static void RADLINK gdraw_ClearStencilBits(U32 bits)
{
   IDirect3DDevice9 *d3d = gdraw->d3d_device;
   F32 texcoord[8] = { 0 };
   GDrawStats stats = { 0 };
   gswf_recti region;

   region.x0 = 0;
   region.y0 = 0;
   region.x1 = gdraw->frametex_width;
   region.y1 = gdraw->frametex_height;

   d3d->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
   d3d->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);
   d3d->SetRenderState(D3DRS_STENCILWRITEMASK, bits);
   d3d->SetRenderState(D3DRS_STENCILENABLE, TRUE);
   // fewest states to force it to always write: make the stencil test always fail
   d3d->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_NEVER);
   d3d->SetRenderState(D3DRS_STENCILFAIL, D3DSTENCILOP_REPLACE);
   d3d->SetRenderState(D3DRS_STENCILREF,  0);
   d3d->SetRenderState(D3DRS_COLORWRITEENABLE, FALSE);
   d3d->SetRenderState(D3DRS_ZENABLE, FALSE);
   d3d->SetRenderState(D3DRS_ZFUNC, FALSE);
   d3d->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

   d3d->SetPixelShader(gdraw->manual_clear.pshader);
   do_screen_quad(&region, texcoord, &stats);

   // restore state from set_common_renderstate
   d3d->SetRenderState(D3DRS_STENCILFAIL, D3DSTENCILOP_KEEP);
   d3d->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_EQUAL);
   d3d->SetRenderState(D3DRS_STENCILREF,  255);

   // make next renderstate reset other state
   clear_renderstate_acceleration_cache();

   // reset matrices et al
   set_render_target(&stats);
   set_viewport();
   set_projection();
}

// this only happens rarely (hopefully never) if we use the depth buffer,
// so we can just clear the whole thing
static void RADLINK gdraw_ClearID(void)
{
   GDrawStats stats = { 0 };

   clear_zbuffer(gdraw->main_depthbuffer, D3DCLEAR_ZBUFFER);
   set_render_target(&stats);
   set_viewport();
   set_projection();
}

////////////////////////////////////////////////////////////////////////
//
//   Set all the render state from GDrawRenderState
//
// This also is responsible for getting the framebuffer into a texture
// if the read-modify-write blend operation can't be expressed with
// the native blend operators. (E.g. "screen")
//

// convert an ID request to a value suitable for the depth buffer,
// assuming the depth buffer has been mappped to 0..1
static F32 depth_from_id(S32 id)
{
   return 1.0f - ((F32) id + 1.0f) / MAX_DEPTH_VALUE;
}

static void set_texture(S32 texunit, GDrawTexture *tex)
{
   if (texunit >= 0)
      set_d3d_texture(texunit, ((GDrawHandle *) tex)->handle.tex.d3d, GDRAW_WRAP_clamp, 0);
}

static int set_renderstate_full(S32 vertex_format, GDrawRenderState *r, GDrawStats * /*stats*/)
{
   IDirect3DDevice9 *d3d = gdraw->d3d_device;
   U32 stencil_key, z_key;
   F32 depth = depth_from_id(r->id);
   VertexVars vvars;
   int vvarcount = VVAR_count_world_xoff;

   // set vertex shader
   d3d->SetVertexShader(gdraw->vert[vertex_format].vshader);
   
   // set vertex shader constants
   if (!r->use_world_space)
      gdraw_ObjectSpace(vvars.world[0], r->o2w, depth, 0.0f);
   else
      gdraw_WorldSpace(vvars.world[0], gdraw->world_to_pixel, depth, 0.0f);

   memcpy(&vvars.x_off, r->edge_matrix, 4*sizeof(F32));

   if (r->texgen0_enabled) {
      memcpy(&vvars.texgen_s, r->s0_texgen, 4*sizeof(F32));
      memcpy(&vvars.texgen_t, r->t0_texgen, 4*sizeof(F32));
      vvarcount = VVAR_count;
   }

   if (gdraw->use_3d || gdraw->last_was_3d) {
      if (gdraw->use_3d) {
         vvarcount = VVAR_count3d;
         memcpy(&vvars.viewproj, gdraw->xform_3d, 12*sizeof(F32));
      } else
         set_projection_base();
      gdraw->last_was_3d = gdraw->use_3d;
   }  

   d3d->SetVertexShaderConstantF(0, vvars.world[0], vvarcount);

   // set the blend mode
   int blend_mode = r->blend_mode;
   int tex0mode = r->tex0_mode;

   static struct gdraw_d3d_blendspec {
      BOOL enable;
      D3DBLEND src;
      D3DBLEND dest;
   } blends[ASSERT_COUNT(GDRAW_BLEND__count, 6)] = {
      FALSE,   D3DBLEND_ONE,        D3DBLEND_ZERO,          // GDRAW_BLEND_none
      TRUE,    D3DBLEND_ONE,        D3DBLEND_INVSRCALPHA,   // GDRAW_BLEND_alpha
      TRUE,    D3DBLEND_DESTCOLOR,  D3DBLEND_INVSRCALPHA,   // GDRAW_BLEND_multiply
      TRUE,    D3DBLEND_ONE,        D3DBLEND_ONE,           // GDRAW_BLEND_add

      FALSE,   D3DBLEND_ONE,        D3DBLEND_ZERO,          // GDRAW_BLEND_filter
      FALSE,   D3DBLEND_ONE,        D3DBLEND_ZERO,          // GDRAW_BLEND_special
   };
   assert(blend_mode >= 0 && blend_mode < sizeof(blends)/sizeof(*blends));
   
   if (blend_mode != gdraw->blend_mode) {
      gdraw->blend_mode = blend_mode;
      if (blends[blend_mode].enable) {
         d3d->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
         d3d->SetRenderState(D3DRS_SRCBLEND, blends[blend_mode].src);
         d3d->SetRenderState(D3DRS_DESTBLEND, blends[blend_mode].dest);
      } else
         d3d->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
   }

   // set the pixel shader
   IDirect3DPixelShader9 *pshader;
   if (blend_mode != GDRAW_BLEND_special) {
      assert(tex0mode >= 0 && tex0mode < sizeof(gdraw->fprog) / sizeof(*gdraw->fprog));

      int additive = 0;
      if (r->cxf_add) {
         additive = 1;
         if (r->cxf_add[3]) additive = 2;
      }

      pshader = gdraw->fprog[tex0mode][additive].pshader;
   } else
      pshader = gdraw->exceptional_blend[r->special_blend].pshader;

   d3d->SetPixelShader(pshader);

   // set textures
   if (tex0mode != GDRAW_TEXTURE_none) {
      if (!r->tex[0]) return 0;
      set_d3d_texture(0, ((GDrawHandle *) r->tex[0])->handle.tex.d3d, r->wrap0, r->nearest0);
   }

   // set pixel shader constants
   d3d->SetPixelShaderConstantF(PVAR_cmul, r->color, 1);
   
   if (r->cxf_add) {
      F32 temp[4] = { r->cxf_add[0]/255.0f, r->cxf_add[1]/255.0f, r->cxf_add[2]/255.0f, r->cxf_add[3]/255.0f };
      d3d->SetPixelShaderConstantF(PVAR_cadd, temp, 1);
   }

   if (tex0mode == GDRAW_TEXTURE_focal_gradient)
      d3d->SetPixelShaderConstantF(PVAR_focal, r->focal_point, 1);

   // Set pixel operation states
   gdraw->scissor_state = ~0u;
   if (r->scissor) {
      RECT s;
      gdraw->scissor_state = r->scissor;
      if (gdraw->cur == gdraw->frame) {
         s.left = r->scissor_rect.x0 + gdraw->vx - gdraw->tx0;
         s.top  = r->scissor_rect.y0 + gdraw->vy - gdraw->ty0;
         s.right = r->scissor_rect.x1 + gdraw->vx - gdraw->tx0;
         s.bottom = r->scissor_rect.y1 + gdraw->vy - gdraw->ty0;
      } else {
         s.left = r->scissor_rect.x0 - gdraw->tx0p;
         s.top  = r->scissor_rect.y0 - gdraw->ty0p;
         s.right = r->scissor_rect.x1 - gdraw->tx0p;
         s.bottom = r->scissor_rect.y1 - gdraw->ty0p;
      }
      d3d->SetScissorRect(&s);
      d3d->SetRenderState(D3DRS_SCISSORTESTENABLE, TRUE);
   } else {
      if (r->scissor != gdraw->scissor_state) {
         gdraw->scissor_state = r->scissor;
         d3d->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);
      }
   }

   // stencil changed?
   stencil_key = r->stencil_test | (r->stencil_set << 8);
   if (stencil_key != gdraw->stencil_key) {
      gdraw->stencil_key = stencil_key;

      d3d->SetRenderState(D3DRS_STENCILMASK, r->stencil_test);
      d3d->SetRenderState(D3DRS_STENCILWRITEMASK, r->stencil_set);
      d3d->SetRenderState(D3DRS_STENCILENABLE, (r->stencil_set | r->stencil_test) != 0);
      d3d->SetRenderState(D3DRS_COLORWRITEENABLE, r->stencil_set ? 0x0 : 0xf);
   }

   // z key
   z_key = r->set_id | (r->test_id << 1);
   if (z_key != gdraw->z_key) {
      gdraw->z_key = z_key;
      d3d->SetRenderState(D3DRS_ZENABLE, (r->test_id | r->set_id) ? D3DZB_TRUE : D3DZB_FALSE);
      d3d->SetRenderState(D3DRS_ZFUNC, r->test_id ? D3DCMP_LESS : D3DCMP_ALWAYS);
      d3d->SetRenderState(D3DRS_ZWRITEENABLE, r->set_id);
   }
   
   return 1;
}

static RADINLINE int set_renderstate(S32 vertex_format, GDrawRenderState *r, GDrawStats *stats)
{
   if (r->identical_state) {
      // fast path: only need to change vertex shader, other state is the same
      gdraw->d3d_device->SetVertexShader(gdraw->vert[vertex_format].vshader);
      return 1;
   } else
      return set_renderstate_full(vertex_format, r, stats);
}

////////////////////////////////////////////////////////////////////////
//
//   Vertex formats
//

static D3DVERTEXELEMENT9 vformat_v2[] =
{
   { 0,0, D3DDECLTYPE_FLOAT2, 0, D3DDECLUSAGE_POSITION, 0 },
   D3DDECL_END()
};

static D3DVERTEXELEMENT9 vformat_v2aa[] =
{
   { 0,0, D3DDECLTYPE_FLOAT2, 0, D3DDECLUSAGE_POSITION, 0 },
   { 0,8, D3DDECLTYPE_SHORT4, 0, D3DDECLUSAGE_TEXCOORD, 0 },
   D3DDECL_END()
};

static D3DVERTEXELEMENT9 vformat_v2tc2[] =
{
   { 0,0, D3DDECLTYPE_FLOAT2, 0, D3DDECLUSAGE_POSITION, 0 },
   { 0,8, D3DDECLTYPE_FLOAT2, 0, D3DDECLUSAGE_TEXCOORD, 0 }, 
   D3DDECL_END()
};

static D3DVERTEXELEMENT9 *vformats[ASSERT_COUNT(GDRAW_vformat__count, 3)] = {
   vformat_v2,    // GDRAW_vformat_v2
   vformat_v2aa,  // GDRAW_vformat_v2aa
   vformat_v2tc2, // GDRAW_vformat_v2tc2
};

static int vertsize[ASSERT_COUNT(GDRAW_vformat__count, 3)] = {
   8,    // GDRAW_vformat_v2
   16,   // GDRAW_vformat_v2aa
   16,   // GDRAW_vformat_v2tc2
};

////////////////////////////////////////////////////////////////////////
//
//   Draw triangles with a given renderstate
//

static void tag_resources(void *r1, void *r2=NULL, void *r3=NULL, void *r4=NULL)
{
   U64 now = gdraw->frame_counter;
   if (r1) ((GDrawHandle *) r1)->fence.value = now;
   if (r2) ((GDrawHandle *) r2)->fence.value = now;
   if (r3) ((GDrawHandle *) r3)->fence.value = now;
   if (r4) ((GDrawHandle *) r4)->fence.value = now;
}

static void RADLINK gdraw_DrawIndexedTriangles(GDrawRenderState *r, GDrawPrimitive *p, GDrawVertexBuffer *buf, GDrawStats *stats)
{
   GDrawHandle *vb = (GDrawHandle *) buf;
   int vfmt = p->vertex_format;
   assert(vfmt >= 0 && vfmt < GDRAW_vformat__count);

   if (!set_renderstate(vfmt, r, stats))
      return;


   gdraw->d3d_device->SetVertexDeclaration(gdraw->vdec[vfmt]);

   if (vb) {
      gdraw->d3d_device->SetIndices(vb->handle.vbuf.indices);
      gdraw->d3d_device->SetStreamSource(0, vb->handle.vbuf.base, (U32) (UINTa) p->vertices, vertsize[vfmt]);

      gdraw->d3d_device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, p->num_vertices, (U32) ((UINTa) p->indices) >> 1, p->num_indices/3);

      gdraw->d3d_device->SetStreamSource(0,0,0,0);
      gdraw->d3d_device->SetIndices(0);
   } else if (p->indices) {
      gdraw->d3d_device->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, p->num_vertices, p->num_indices/3, p->indices, D3DFMT_INDEX16, p->vertices, vertsize[vfmt]);
   } else { // dynamic quads
      assert(p->num_vertices % 4 == 0);
      UINT stride = vertsize[vfmt];
      S32 pos = 0;
      while (pos < p->num_vertices) {
         S32 vert_count = RR_MIN(p->num_vertices - pos, QUAD_IB_COUNT * 4);
         gdraw->d3d_device->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, vert_count, (vert_count/4)*2, quad_ib, D3DFMT_INDEX16, (U8 *)p->vertices + pos * stride, stride);
         pos += vert_count;
      }
   }

   tag_resources(vb, r->tex[0], r->tex[1]);

   stats->nonzero_flags |= GDRAW_STATS_batches;
   stats->num_batches += 1;
   stats->drawn_indices += p->num_indices;
   stats->drawn_vertices += p->num_vertices;
}

///////////////////////////////////////////////////////////////////////
//
//   Flash 8 filter effects
//

static void set_pixel_constant(S32 constant, F32 x, F32 y, F32 z, F32 w)
{
   if (constant >= 0) {
      F32 value[4] = { x,y,z,w };
      gdraw->d3d_device->SetPixelShaderConstantF(constant, value, 1);
   }
}

// caller sets up texture coordinates
static void do_screen_quad(gswf_recti *s, F32 *tc, GDrawStats *stats)
{
   F32 px0 = (F32) s->x0, py0 = (F32) s->y0, px1 = (F32) s->x1, py1 = (F32) s->y1;
   gswf_vertex_xyst vert[4];
   VertexVars vvars;

   // interleave the data so we can use UP
   vert[0].x = px0; vert[0].y = py0; vert[0].s = tc[0]; vert[0].t = tc[1];
   vert[1].x = px1; vert[1].y = py0; vert[1].s = tc[2]; vert[1].t = tc[1];
   vert[2].x = px1; vert[2].y = py1; vert[2].s = tc[2]; vert[2].t = tc[3];
   vert[3].x = px0; vert[3].y = py1; vert[3].s = tc[0]; vert[3].t = tc[3];

   gdraw_PixelSpace(vvars.world[0]);

   gdraw->d3d_device->SetVertexDeclaration(gdraw->vdec[GDRAW_vformat_v2tc2]);
   gdraw->d3d_device->SetVertexShader(gdraw->vert[GDRAW_vformat_v2tc2].vshader);
   gdraw->d3d_device->SetVertexShaderConstantF(0, vvars.world[0], VVAR_count_worldonly);
   gdraw->d3d_device->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, vert, 16);

   stats->nonzero_flags |= GDRAW_STATS_batches;
   stats->num_batches += 1;
   stats->drawn_indices += 6;
   stats->drawn_vertices += 4;
}

static void gdraw_DriverBlurPass(GDrawRenderState *r, int taps,  float *data, gswf_recti *s, float *tc, float /*height_max*/, float *clamp, GDrawStats *gstats)
{
   ProgramWithCachedVariableLocations *prg = &gdraw->blur_prog[taps];
   gdraw->d3d_device->SetPixelShader(prg->pshader);
   set_texture(prg->vars[VAR_blur_tex0], r->tex[0]);

   assert(prg->vars[VAR_blur_tap] >= 0);
   gdraw->d3d_device->SetPixelShaderConstantF(prg->vars[VAR_blur_tap], data, taps);
   gdraw->d3d_device->SetPixelShaderConstantF(prg->vars[VAR_blur_clampv], clamp, 1);

   do_screen_quad(s, tc, gstats);
   tag_resources(r->tex[0]);
}

static void gdraw_Colormatrix(GDrawRenderState *r, gswf_recti *s, float *tc, GDrawStats *stats)
{
   if (!gdraw_TextureDrawBufferBegin(s, GDRAW_TEXTURE_FORMAT_rgba32, GDRAW_TEXTUREDRAWBUFFER_FLAGS_needs_color | GDRAW_TEXTUREDRAWBUFFER_FLAGS_needs_alpha, 0, stats))
      return;

   gdraw->d3d_device->SetPixelShader(gdraw->colormatrix.pshader);
   set_texture(gdraw->colormatrix.vars[VAR_colormatrix_tex0], r->tex[0]);
   gdraw->d3d_device->SetPixelShaderConstantF(gdraw->colormatrix.vars[VAR_colormatrix_data], r->shader_data, 5);
   do_screen_quad(s, tc, stats);
   tag_resources(r->tex[0]);
   r->tex[0] = gdraw_TextureDrawBufferEnd(stats);
}

static gswf_recti *get_valid_rect(GDrawTexture *tex)
{
   GDrawHandle *h = (GDrawHandle *) tex;
   S32 n = (S32) (h - gdraw->rendertargets.handle);
   assert(n >= 0 && n <= MAX_RENDER_STACK_DEPTH+1);
   return &gdraw->rt_valid[n];
}

static void set_clamp_constant(int constant, GDrawTexture *tex)
{
   gswf_recti *s = get_valid_rect(tex);
   // when we make the valid data, we make sure there is an extra empty pixel at the border
   set_pixel_constant(constant,
      (s->x0-0.5f) / gdraw->frametex_width,
      (s->y0-0.5f) / gdraw->frametex_height,
      (s->x1+0.5f) / gdraw->frametex_width,
      (s->y1+0.5f) / gdraw->frametex_height);
}

static void gdraw_Filter(GDrawRenderState *r, gswf_recti *s, float *tc, int isbevel, GDrawStats *stats)
{
   ProgramWithCachedVariableLocations *prg = &gdraw->filter_prog[isbevel][r->filter_mode];
   
   if (!gdraw_TextureDrawBufferBegin(s, GDRAW_TEXTURE_FORMAT_rgba32, GDRAW_TEXTUREDRAWBUFFER_FLAGS_needs_color | GDRAW_TEXTUREDRAWBUFFER_FLAGS_needs_alpha, NULL, stats))
      return;

   gdraw->d3d_device->SetPixelShader(prg->pshader);
   set_texture(prg->vars[VAR_filter_tex0], r->tex[0]);
   set_texture(prg->vars[VAR_filter_tex1], r->tex[1]);
   set_texture(prg->vars[VAR_filter_tex2], r->tex[2]);

   set_pixel_constant(prg->vars[VAR_filter_color], r->shader_data[0], r->shader_data[1], r->shader_data[2], r->shader_data[3]);
   set_pixel_constant(prg->vars[VAR_filter_tc_off], -r->shader_data[4] / (F32)gdraw->frametex_width, -r->shader_data[5] / (F32)gdraw->frametex_height, r->shader_data[6], 0);
   set_pixel_constant(prg->vars[VAR_filter_color2], r->shader_data[8], r->shader_data[9], r->shader_data[10], r->shader_data[11]);
   set_clamp_constant(prg->vars[VAR_filter_clamp0], r->tex[0]);
   set_clamp_constant(prg->vars[VAR_filter_clamp1], r->tex[1]);

   do_screen_quad(s, tc, stats);
   tag_resources(r->tex[0], r->tex[1], r->tex[2]);
   r->tex[0] = gdraw_TextureDrawBufferEnd(stats);
}

static void RADLINK gdraw_FilterQuad(GDrawRenderState *r, S32 x0, S32 y0, S32 x1, S32 y1, GDrawStats *stats)
{
   IDirect3DDevice9 *d3d = gdraw->d3d_device;
   F32 tc[4];
   gswf_recti s;

   // clip to tile boundaries
   s.x0 = RR_MAX(x0, gdraw->tx0p);
   s.y0 = RR_MAX(y0, gdraw->ty0p);
   s.x1 = RR_MIN(x1, gdraw->tx0p + gdraw->tpw);
   s.y1 = RR_MIN(y1, gdraw->ty0p + gdraw->tph);
   if (s.x1 < s.x0 || s.y1 < s.y0)
      return;

   tc[0] = (s.x0 - gdraw->tx0p) / (F32) gdraw->frametex_width;    
   tc[1] = (s.y0 - gdraw->ty0p) / (F32) gdraw->frametex_height;   
   tc[2] = (s.x1 - gdraw->tx0p) / (F32) gdraw->frametex_width;    
   tc[3] = (s.y1 - gdraw->ty0p) / (F32) gdraw->frametex_height;   

   // clear to known render state
   d3d->SetRenderState(D3DRS_STENCILENABLE, FALSE);
   d3d->SetRenderState(D3DRS_COLORWRITEENABLE, 0xf);

   d3d->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
   d3d->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESS);
   d3d->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
   
   d3d->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);
   d3d->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

   gdraw->scissor_state = 0;
   gdraw->blend_mode = GDRAW_BLEND_none;
   gdraw->stencil_key = 0;
   gdraw->z_key = 0;

   if (r->blend_mode == GDRAW_BLEND_filter) {
      switch (r->filter) {
         case GDRAW_FILTER_blur: {
            GDrawBlurInfo b;
            gswf_recti bounds = *get_valid_rect(r->tex[0]);
            gdraw_ShiftRect(&s, &s, -gdraw->tx0p, -gdraw->ty0p); // blur uses physical rendertarget coordinates

            b.BlurPass = gdraw_DriverBlurPass;
            b.w = gdraw->tpw;
            b.h = gdraw->tph;
            b.frametex_width = gdraw->frametex_width;
            b.frametex_height = gdraw->frametex_height;

            // blur needs to draw with multiple passes, so set up special state
            gdraw->in_blur = true;
            set_viewport_raw(0, 0, gdraw->tpw, gdraw->tph);
            set_projection_raw(0, gdraw->tpw, gdraw->tph, 0);

            // do the blur
            gdraw_Blur(&gdraw_funcs, &b,r, &s, &bounds, stats);

            // restore the normal state
            gdraw->in_blur = false;
            set_viewport();
            set_projection();
            break;
         }

         case GDRAW_FILTER_colormatrix:
            gdraw_Colormatrix(r, &s, tc, stats);
            break;

         case GDRAW_FILTER_dropshadow:
            gdraw_Filter(r, &s, tc, 0, stats);
            break;

         case GDRAW_FILTER_bevel:
            gdraw_Filter(r, &s, tc, 1, stats);
            break;

         default:
            assert(0);
      }
   } else {
      GDrawHandle *blend_tex = NULL;
      static S16 zero[4] = { 0, 0, 0, 0 };

      // for crazy blend modes, we need to read back from the framebuffer
      // and do the blending in the pixel shader. we do this with StretchRect
      // rather than trying to render-to-texture-all-along, because we want
      // to be able to render over the user's existing framebuffer, which might
      // not be a texture. note that this causes MSAA issues!
      if (r->blend_mode == GDRAW_BLEND_special &&
          (blend_tex = get_color_rendertarget(stats)) != NULL) {

         IDirect3DSurface9 *src;
         HRESULT hr = d3d->GetRenderTarget(0, &src);
         if (!FAILED(hr)) {
            IDirect3DSurface9 *dest;
            hr = blend_tex->handle.tex.d3d->GetSurfaceLevel(0, &dest);
            if (!FAILED(hr)) {
               RECT drect = { 0, 0, gdraw->tpw, gdraw->tph };
               RECT srect;

               if (gdraw->cur != gdraw->frame)
                  srect = drect;
               else {
                  srect.left   = gdraw->vx;
                  srect.top    = gdraw->vy;
                  srect.right  = gdraw->vx + gdraw->tw;
                  srect.bottom = gdraw->vy + gdraw->th;
                  drect.left   = gdraw->tx0 - gdraw->tx0p;
                  drect.top    = gdraw->ty0 - gdraw->ty0p;
                  drect.right  = drect.left + gdraw->tw;
                  drect.bottom = drect.top + gdraw->th;
               }

               d3d->StretchRect(src, &srect, dest, &drect, D3DTEXF_POINT);
               dest->Release();

               stats->nonzero_flags |= GDRAW_STATS_blits;
               stats->num_blits += 1;
               stats->num_blit_pixels += (srect.right - srect.left) * (srect.bottom - srect.top);
            }

            src->Release();
         }

         set_texture(1, (GDrawTexture *) blend_tex);
         // make sure we set color_add, because the shader reads it
         if (!r->cxf_add)
            r->cxf_add = zero;
      }

      if (!set_renderstate(GDRAW_vformat_v2tc2, r, stats))
         return;

      do_screen_quad(&s, tc, stats);
      tag_resources(r->tex[0], r->tex[1]);
      if (blend_tex)
         gdraw_FreeTexture((GDrawTexture *) blend_tex, 0, stats);
   }
}

///////////////////////////////////////////////////////////////////////
//
//   Shaders
//

#include "gdraw_d3d9_shaders.inl"

static void create_pixel_shader(ProgramWithCachedVariableLocations *p, ProgramWithCachedVariableLocations *src)
{
   *p = *src;
   if(p->bytecode) {
      HRESULT hr = gdraw->d3d_device->CreatePixelShader(p->bytecode, &p->pshader);
      if (FAILED(hr)) {
         IggyGDrawSendWarning(NULL, "GDraw D3D error creating pixel shader: 0x%08x", hr);
         p->pshader = NULL;
      }
   }
}

static void create_vertex_shader(ProgramWithCachedVariableLocations *p, ProgramWithCachedVariableLocations *src)
{
   *p = *src;
   if(p->bytecode) {
      HRESULT hr = gdraw->d3d_device->CreateVertexShader(p->bytecode, &p->vshader);
      if (FAILED(hr)) {
         IggyGDrawSendWarning(NULL, "GDraw D3D error creating vertex shader: 0x%08x", hr);
         p->vshader = NULL;
      }
   }
}

static void destroy_shader(ProgramWithCachedVariableLocations *p)
{
   safe_release(p->pshader);
}

static void create_all_shaders(void)
{
   S32 i;

   for (i=0; i < GDRAW_TEXTURE__count*3; ++i)       create_pixel_shader(&gdraw->fprog[0][i], pshader_basic_arr + i);
   for (i=0; i < GDRAW_BLENDSPECIAL__count; ++i)    create_pixel_shader(&gdraw->exceptional_blend[i], pshader_exceptional_blend_arr + i);
   for (i=0; i < 32; ++i)                           create_pixel_shader(&gdraw->filter_prog[0][i], pshader_filter_arr + i);
   for (i=0; i < MAX_TAPS+1; ++i)                   create_pixel_shader(&gdraw->blur_prog[i], pshader_blur_arr + i);
   create_pixel_shader(&gdraw->colormatrix, pshader_color_matrix_arr);
   create_pixel_shader(&gdraw->manual_clear, pshader_manual_clear_arr);

   for (i=0; i < GDRAW_vformat__count; i++) {
      HRESULT hr = gdraw->d3d_device->CreateVertexDeclaration(vformats[i], &gdraw->vdec[i]);
      if (FAILED(hr))
         IggyGDrawSendWarning(NULL, "GDraw D3D error in CreateVertexDeclaration: 0x%08x", hr);

      create_vertex_shader(&gdraw->vert[i], vshader_vsd3d9_arr + i);
   }
}

static void destroy_all_shaders()
{
   S32 i;

   for (i=0; i < GDRAW_TEXTURE__count*3; ++i)       destroy_shader(&gdraw->fprog[0][i]);
   for (i=0; i < GDRAW_BLENDSPECIAL__count; ++i)    destroy_shader(&gdraw->exceptional_blend[i]);
   for (i=0; i < 32; ++i)                           destroy_shader(&gdraw->filter_prog[0][i]);
   for (i=0; i < MAX_TAPS+1; ++i)                   destroy_shader(&gdraw->blur_prog[i]);
   destroy_shader(&gdraw->colormatrix);

   for (i=0; i < GDRAW_vformat__count; i++) {
      if (gdraw->vdec[i]) { gdraw->vdec[i]->Release(); gdraw->vdec[i] = 0; }
      destroy_shader(&gdraw->vert[i]);
   }
}

////////////////////////////////////////////////////////////////////////
//
//   Create and tear-down the state
//

typedef struct
{
   S32 num_handles;
   S32 num_bytes;
} GDrawResourceLimit;

// These are the defaults limits used by GDraw unless the user specifies something else.
static GDrawResourceLimit gdraw_limits[ASSERT_COUNT(GDRAW_D3D_RESOURCE__count, 3)] = {
   MAX_RENDER_STACK_DEPTH + 1, 16*1024*1024,  // GDRAW_D3D_RESOURCE_rendertarget
    500,                       16*1024*1024,  // GDRAW_D3D_RESOURCE_texture
   1000,                        2*1024*1024,  // GDRAW_D3D_RESOURCE_vertexbuffer
};

static GDrawHandleCache *make_handle_cache(gdraw_d3d_resourcetype type)
{
   S32 num_handles = gdraw_limits[type].num_handles;
   S32 num_bytes = gdraw_limits[type].num_bytes;
   GDrawHandleCache *cache = (GDrawHandleCache *) IggyGDrawMalloc(sizeof(GDrawHandleCache) + (num_handles - 1) * sizeof(GDrawHandle));
   if (cache) {
      gdraw_HandleCacheInit(cache, num_handles, num_bytes);
      cache->is_vertex = (type == GDRAW_D3D_RESOURCE_vertexbuffer);
   }

   return cache;
}

static void free_gdraw()
{
   if (!gdraw) return;
   if (gdraw->texturecache) IggyGDrawFree(gdraw->texturecache);
   if (gdraw->vbufcache) IggyGDrawFree(gdraw->vbufcache);
   IggyGDrawFree(gdraw);
   gdraw = NULL;
}

int gdraw_D3D_SetResourceLimits(gdraw_d3d_resourcetype type, S32 num_handles, S32 num_bytes)
{
   GDrawStats stats={0};

   if (type == GDRAW_D3D_RESOURCE_rendertarget) // RT count is small and space is preallocated
      num_handles = MAX_RENDER_STACK_DEPTH + 1;

   assert(type >= GDRAW_D3D_RESOURCE_rendertarget && type < GDRAW_D3D_RESOURCE__count);
   assert(num_handles >= 0);
   assert(num_bytes >= 0);

   // nothing to do if the values are unchanged
   if (gdraw_limits[type].num_handles == num_handles &&
      gdraw_limits[type].num_bytes == num_bytes)
      return 1;

   gdraw_limits[type].num_handles = num_handles;
   gdraw_limits[type].num_bytes = num_bytes;

   // if no gdraw context created, there's nothing to worry about
   if (!gdraw)
      return 1;

   // resize the appropriate pool
   switch (type) {
      case GDRAW_D3D_RESOURCE_rendertarget:
         flush_rendertargets(&stats);
         gdraw_HandleCacheInit(&gdraw->rendertargets, num_handles, num_bytes);
         return 1;
         
      case GDRAW_D3D_RESOURCE_texture:
         if (gdraw->texturecache) {
            gdraw_res_flush(gdraw->texturecache, &stats);
            IggyGDrawFree(gdraw->texturecache);
         }
         gdraw->texturecache = make_handle_cache(GDRAW_D3D_RESOURCE_texture);
         return gdraw->texturecache != NULL;

      case GDRAW_D3D_RESOURCE_vertexbuffer:
         if (gdraw->vbufcache) {
            gdraw_res_flush(gdraw->vbufcache, &stats);
            IggyGDrawFree(gdraw->vbufcache);
         }
         gdraw->vbufcache = make_handle_cache(GDRAW_D3D_RESOURCE_vertexbuffer);
         return gdraw->vbufcache != NULL;

      default:
         return 0;
   }
}

GDrawFunctions *gdraw_D3D_CreateContext(IDirect3DDevice9 *dev, S32 w, S32 h)
{
   gdraw = (GDraw *) IggyGDrawMalloc(sizeof(*gdraw));
   if (!gdraw) return NULL;

   memset(gdraw, 0, sizeof(*gdraw));

   gdraw->frametex_width = w;
   gdraw->frametex_height = h;

   gdraw->texturecache = make_handle_cache(GDRAW_D3D_RESOURCE_texture);
   gdraw->vbufcache = make_handle_cache(GDRAW_D3D_RESOURCE_vertexbuffer);
   gdraw_HandleCacheInit(&gdraw->rendertargets, gdraw_limits[GDRAW_D3D_RESOURCE_rendertarget].num_handles, gdraw_limits[GDRAW_D3D_RESOURCE_rendertarget].num_bytes);

   if (!gdraw->texturecache || !gdraw->vbufcache) {
      free_gdraw();
      return NULL;
   }

   if (!quad_ib[1]) {
      // initialize quad index buffer
      for (U16 i=0; i < QUAD_IB_COUNT; i++) {
         quad_ib[i*6 + 0] = i*4 + 0;
         quad_ib[i*6 + 1] = i*4 + 1;
         quad_ib[i*6 + 2] = i*4 + 2;
         quad_ib[i*6 + 3] = i*4 + 0;
         quad_ib[i*6 + 4] = i*4 + 2;
         quad_ib[i*6 + 5] = i*4 + 3;
      }
   }

   #if 1
   D3DCAPS9 caps;
   dev->GetDeviceCaps(&caps);
   gdraw->conditional_nonpow2 = ((caps.TextureCaps & D3DPTEXTURECAPS_NONPOW2CONDITIONAL) != 0);
   #endif

   gdraw->d3d_device = dev;
   create_all_shaders();

   gdraw_funcs.SetViewSizeAndWorldScale = gdraw_SetViewSizeAndWorldScale;
   gdraw_funcs.GetInfo       = gdraw_GetInfo;

   gdraw_funcs.DescribeTexture = gdraw_DescribeTexture;
   gdraw_funcs.DescribeVertexBuffer = gdraw_DescribeVertexBuffer;

   gdraw_funcs.RenderingBegin  = gdraw_RenderingBegin;
   gdraw_funcs.RenderingEnd    = gdraw_RenderingEnd;
   gdraw_funcs.RenderTileBegin = gdraw_RenderTileBegin;
   gdraw_funcs.RenderTileEnd = gdraw_RenderTileEnd;

   gdraw_funcs.TextureDrawBufferBegin = gdraw_TextureDrawBufferBegin;
   gdraw_funcs.TextureDrawBufferEnd = gdraw_TextureDrawBufferEnd;

   gdraw_funcs.DrawIndexedTriangles = gdraw_DrawIndexedTriangles;
   gdraw_funcs.FilterQuad = gdraw_FilterQuad;

   gdraw_funcs.SetAntialiasTexture = gdraw_SetAntialiasTexture;

   gdraw_funcs.ClearStencilBits = gdraw_ClearStencilBits;
   gdraw_funcs.ClearID = gdraw_ClearID;

   gdraw_funcs.MakeTextureBegin = gdraw_MakeTextureBegin;
   gdraw_funcs.MakeTextureMore  = gdraw_MakeTextureMore;
   gdraw_funcs.MakeTextureEnd   = gdraw_MakeTextureEnd;

   gdraw_funcs.UpdateTextureBegin = gdraw_UpdateTextureBegin;
   gdraw_funcs.UpdateTextureRect  = gdraw_UpdateTextureRect;
   gdraw_funcs.UpdateTextureEnd   = gdraw_UpdateTextureEnd;

   gdraw_funcs.FreeTexture = gdraw_FreeTexture;
   gdraw_funcs.TryToLockTexture = gdraw_TryToLockTexture;

   gdraw_funcs.MakeTextureFromResource = (gdraw_make_texture_from_resource *) gdraw_D3D_MakeTextureFromResource;
   gdraw_funcs.FreeTextureFromResource = gdraw_D3D_DestroyTextureFromResource;

   gdraw_funcs.MakeVertexBufferBegin = gdraw_MakeVertexBufferBegin;
   gdraw_funcs.MakeVertexBufferMore  = gdraw_MakeVertexBufferMore;
   gdraw_funcs.MakeVertexBufferEnd   = gdraw_MakeVertexBufferEnd;
   gdraw_funcs.TryToLockVertexBuffer = gdraw_TryLockVertexBuffer;
   gdraw_funcs.FreeVertexBuffer = gdraw_FreeVertexBuffer;

   gdraw_funcs.UnlockHandles = gdraw_UnlockHandles;
   gdraw_funcs.SetTextureUniqueID = gdraw_SetTextureUniqueID;
   gdraw_funcs.Set3DTransform = gdraw_Set3DTransform;

   return &gdraw_funcs;
}

void gdraw_D3D_DestroyContext(void)
{
   if (gdraw && gdraw->d3d_device) {
      GDrawStats stats={0};
      destroy_all_shaders();
      gdraw_res_flush(gdraw->texturecache, &stats);
      gdraw_res_flush(gdraw->vbufcache, &stats);
      flush_rendertargets(&stats);

      safe_release(gdraw->aa_tex);
      gdraw->d3d_device = NULL;
   }

   free_gdraw();
}

void gdraw_D3D_SetErrorHandler(void (__cdecl *error_handler)(HRESULT hr))
{
   if (gdraw)
      gdraw->error_handler = error_handler;
}

void gdraw_D3D_PreReset(void)
{
   if (!gdraw) return;

   GDrawStats stats={0};
   flush_rendertargets(&stats);

   // we may end up resizing the frame buffer
   gdraw->frametex_width = 0;
   gdraw->frametex_height = 0;
}

void gdraw_D3D_PostReset(void)
{
   // maybe re-create rendertargets right now?
}

void RADLINK gdraw_D3D_BeginCustomDraw(IggyCustomDrawCallbackRegion *region, D3DMATRIX *mat)
{
   clear_renderstate();
   gdraw_GetObjectSpaceMatrix(mat->m[0], region->o2w, gdraw->projection, 0, 0);
}

void RADLINK gdraw_D3D_EndCustomDraw(IggyCustomDrawCallbackRegion * /*region*/)
{
   set_common_renderstate();
   set_projection();
}

void RADLINK gdraw_D3D_GetResourceUsageStats(gdraw_d3d_resourcetype type, S32 *handles_used, S32 *bytes_used)
{
   GDrawHandleCache *cache;

   switch (type) {
      case GDRAW_D3D_RESOURCE_rendertarget:  cache = &gdraw->rendertargets; break;
      case GDRAW_D3D_RESOURCE_texture:       cache = gdraw->texturecache; break;
      case GDRAW_D3D_RESOURCE_vertexbuffer:  cache = gdraw->vbufcache; break;
      default:                               cache = NULL; break;
   }

   *handles_used = *bytes_used = 0;

   if (cache) {
      S32 i;
      U64 frame = gdraw->frame_counter;

      for (i=0; i < cache->max_handles; ++i)
         if (cache->handle[i].bytes && cache->handle[i].owner && cache->handle[i].fence.value == frame) {
            *handles_used += 1;
            *bytes_used += cache->handle[i].bytes;
         }
   }
}

GDrawTexture * RADLINK gdraw_D3D_MakeTextureFromResource(U8 *resource_file, S32 /*len*/, IggyFileTextureRaw *texture)
{
   GDrawTexture *h;
   S32 width, height, mipmaps, size;
   IDirect3DTexture9 *tex;
   D3DFORMAT d3dfmt;
   width = texture->w;
   height = texture->h;
   mipmaps = texture->mipmaps;

   // discard mipmaps on non-pow-of-2 if the hardware doesn't support
   if (gdraw->conditional_nonpow2) {
      if ((width & (width-1)) || (height & (height-1)))
         mipmaps = 1;
   }

   switch (texture->format) {
      case IFT_FORMAT_rgba_8888   : size= 4; d3dfmt = D3DFMT_A8R8G8B8; break;
      case IFT_FORMAT_rgba_4444_LE: size= 2; d3dfmt = D3DFMT_A4R4G4B4; break;
      case IFT_FORMAT_rgba_5551_LE: size= 2; d3dfmt = D3DFMT_A1R5G5B5; break;
      case IFT_FORMAT_la_88       : size= 2; d3dfmt = D3DFMT_A8L8; break;
      case IFT_FORMAT_la_44       : size= 1; d3dfmt = D3DFMT_A4L4; break;
      case IFT_FORMAT_i_8         :  
      case IFT_FORMAT_i_4         : size= 2; d3dfmt = D3DFMT_A8L8; break;
      case IFT_FORMAT_l_8         : size= 1; 
      case IFT_FORMAT_l_4         : size= 1; d3dfmt = D3DFMT_L8; break;
      case IFT_FORMAT_DXT1        : size= 8; d3dfmt = D3DFMT_DXT1; break;
      case IFT_FORMAT_DXT3        : size=16; d3dfmt = D3DFMT_DXT3; break;
      case IFT_FORMAT_DXT5        : size=16; d3dfmt = D3DFMT_DXT5; break;
      default: size=0; d3dfmt=D3DFMT_UNKNOWN; break;
   }

   HRESULT hr = gdraw->d3d_device->CreateTexture(width, height, mipmaps,
                                         0, d3dfmt, D3DPOOL_MANAGED, &tex, NULL);
   if (FAILED(hr)) {
      if (size >= 8 && ((width & 3) || (height & 3)))
         IggyGDrawSendWarning(NULL, "GDraw D3D error, dxtc texture dimensions must be multiples of 4");
      else 
         report_d3d_error(hr, "CreateTexture", "");
      return NULL;
   }

   U8 *data = resource_file + texture->file_offset;

   for (int level=0; level < mipmaps; ++level) {
      S32 dxt_width = (width+3)/4;
      S32 effective_height;
      if (size >= 8)
         effective_height = (height+3)/4;
      else
         effective_height = height;

      D3DLOCKED_RECT z;
      hr = tex->LockRect(level, &z, NULL, 0);
      if (FAILED(hr)) {
         report_d3d_error(hr, "LockRect", " while creating texture");
         tex->Release();
         return NULL;
      }
      U8 *pixels = (U8*) z.pBits;
      for (S32 j=0; j < effective_height; ++j) {
         switch (texture->format) {
            default:
               memcpy(pixels + z.Pitch*j, data + j*width*size, width*size);
               break;
            case IFT_FORMAT_rgba_8888: {
               for (S32 i=0; i < width; ++i) {
                  pixels[z.Pitch*j + i*4 + 0] = data[j*width*4 + i*4 + 2];
                  pixels[z.Pitch*j + i*4 + 1] = data[j*width*4 + i*4 + 1];
                  pixels[z.Pitch*j + i*4 + 2] = data[j*width*4 + i*4 + 0];
                  pixels[z.Pitch*j + i*4 + 3] = data[j*width*4 + i*4 + 3];
               }
               break;
            }
            case IFT_FORMAT_DXT1:
            case IFT_FORMAT_DXT3:
            case IFT_FORMAT_DXT5:
               memcpy(pixels + z.Pitch*j, data + j*dxt_width*size, dxt_width*size);
               break;
            case IFT_FORMAT_i_8:  
            case IFT_FORMAT_i_4: {
               // convert from intensity to luminance-alpha by replicating all the pixels
               for (S32 i=0; i < width; ++i)
                  pixels[z.Pitch*j + i*2 + 0] = 
                  pixels[z.Pitch*j + i*2 + 1] = data[j*width + i];
               break;
            }
         }
      }
      tex->UnlockRect(level);

      data += (size<8) ? width*height*size : ((width+3)/4)*((height+3)/4)*size;
      width  = width >>1; width  += !width ;
      height = height>>1; height += !height;
   }
   
   h = gdraw_D3D_WrappedTextureCreate(tex);
   if (!h) tex->Release();
   return h;
}

void RADLINK gdraw_D3D_DestroyTextureFromResource(GDrawTexture *tex)
{
   GDrawHandle *h = (GDrawHandle *) tex;
   h->handle.tex.d3d->Release();
   gdraw_D3D_WrappedTextureDestroy(tex);
}

