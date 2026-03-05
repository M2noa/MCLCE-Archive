// gdraw_d3d.h - author: Sean Barrett - copyright 2009-2011 RAD Game Tools
//
// Interface for creating a D3D GDraw driver.

#include "gdraw.h"

#define IDOC
//idoc(parent,GDraw_d3d9)

typedef enum gdraw_d3d_resourcetype
{
   GDRAW_D3D_RESOURCE_rendertarget,
   GDRAW_D3D_RESOURCE_texture,
   GDRAW_D3D_RESOURCE_vertexbuffer,

   GDRAW_D3D_RESOURCE__count,
} gdraw_d3d_resourcetype;

IDOC extern int gdraw_D3D_SetResourceLimits(gdraw_d3d_resourcetype type, S32 num_handles, S32 num_bytes);
/* This sets how large the memory pool for a given resource types is, and how many handles
   GDraw should allocate for it. GDraw keeps track of allocations in each pool, and will free
   old resources in a LRU manner to make space if one of the limits is about to be exceeded.

   Returns 1 if value successfully changed, 0 on error.
   You need to call IggyPlayerFlushAll on all active Iggys before you do this to make
   them flush their resources since changing the resource limits invalidates all handles.
   You also need to call IggyFlushInstalledFonts if you have any installed fonts.
*/

IDOC extern GDrawFunctions * gdraw_D3D_CreateContext(IDirect3DDevice9 *dev, S32 w, S32 h);
/* Creates a GDraw context for rendering using D3D. You need to pass in the D3D device
   and the width/height of the content you're displaying.
   
   The width/height is used solely for sizing internal rendertargets. They will be
   allocated to the larger of this size and the size of any rendered tiles (with padding).
   In other words, you can pass in (0,0) and the rendertargets will be allocated to the
   right size. However, if you draw multiple Iggy files or tiles of different sizes,
   they might first be allocated too small; it's best to pass in the correct size initially
   to avoid unnecessary allocation/deallocation of too-small rendertargets.

   There can only be one D3D GDraw context active at any one time.

   If initialization fails for some reason (the main reason would be an out of memory condition),
   NULL is returned. Otherwise, you can pass the return value to IggySetGDraw. */

IDOC extern void gdraw_D3D_SetRendertargetSize(S32 w, S32 h);
/* Reset the size used for internal rendertargets defined by CreateContext. Flushes
   all existing rendertargets if the size changes. */

IDOC extern void gdraw_D3D_DestroyContext(void);
/* Destroys the current GDraw context, if any. */

IDOC extern void gdraw_D3D_SetTileOrigin(IDirect3DSurface9 *rt, IDirect3DSurface9 *depth, S32 x, S32 y);
/* This sets the main rendertarget that GDraw should render to, the corresponding
   depth/stencil surface, and the x/y position where to draw the top-left of the current tile (to be used for tiled
   rendering). You need to call this before Iggy calls any rendering functions. */

IDOC extern void gdraw_D3D_NoMoreGDrawThisFrame(void);
/* Tells GDraw that no more rendering operations will occur this frame. This triggers
   some end-of-frame processing; most importantly, GDraw uses this call as a marker to
   detect thrashing (and react accordingly), so please do not forget to call this
   every frame! (As long as Iggy does any rendering, that is) */

IDOC extern void gdraw_D3D_PreReset(void);
/* Call this before D3D device Reset(); it will free all default pool resources allocated
   by GDraw. */

IDOC extern void gdraw_D3D_PostReset(void);
/* Call after D3D device Reset(). */

IDOC extern void RADLINK gdraw_D3D_BeginCustomDraw(IggyCustomDrawCallbackRegion *Region, D3DMATRIX *mat);
/* Call at the beginning of Iggy custom draw callback to clear any odd render states GDraw has
   set on the D3D device, and to get the current 2D object-to-world transformation. */

IDOC extern void RADLINK gdraw_D3D_EndCustomDraw(IggyCustomDrawCallbackRegion *Region);
/* Call at the end of Iggy custom draw callback so GDraw can restore its render states. */

IDOC extern void RADLINK gdraw_D3D_GetResourceUsageStats(gdraw_d3d_resourcetype type, S32 *handles_used, S32 *bytes_used);
/* D3D only: Get resource usage stats for last frame.
   This can be used to get an estimate of how much graphics memory got used by GDraw
   during the last frame.
   Caveat: This counts the number of bytes that GDraw knows about. 3D hardware usually
   has its own management overhead, alignment requirements, allocation granularity
   and so on. In short, this is not an accurate estimate of how much memory is actually
   used by the GPU - it is a lower bound, though, and makes for a useful ballpark estimate. */

IDOC extern GDrawTexture *gdraw_D3D_WrappedTextureCreate(IDirect3DTexture9 *tex_handle);
/* Create a wrapped texture from a D3D texture.
   A wrapped texture can be used to let Iggy draw using the contents of a texture
   you create and manage on your own. For example, you might render to this texture,
   or stream video into it. Wrapped textures take up a handle. They will never be
   freed or otherwise modified by GDraw; nor will GDraw change any reference counts.
   All this is up to the application. */

IDOC extern void gdraw_D3D_WrappedTextureChange(GDrawTexture *tex, IDirect3DTexture9 *tex_handle);
/* Switch an existing GDrawTexture * that represents a wrapped texture to use
   a new underlying D3D texture. For example, you might internally double-buffer
   a dynamically updated texture. As above, GDraw will leave this texture alone
   and not touch any reference counts. */

IDOC extern void gdraw_D3D_WrappedTextureDestroy(GDrawTexture *tex);
/* Destroys the GDraw wrapper for a wrapped texture object. This will free up
   a GDraw texture handle but not release the associated D3D texture; that is
   up to you. */

extern GDrawTexture * RADLINK gdraw_D3D_MakeTextureFromResource(U8 *resource_file, S32 length, IggyFileTextureRaw *texture);
extern void RADLINK gdraw_D3D_DestroyTextureFromResource(GDrawTexture *tex);
