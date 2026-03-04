#ifndef __RAD_INCLUDE_GDRAW_GL_H__
#define __RAD_INCLUDE_GDRAW_GL_H__

#include "rrCore.h"
#include "gdraw.h"

RADDEFSTART

typedef enum gdraw_gl_resourcetype
{
   GDRAW_GL_RESOURCE_rendertarget,
   GDRAW_GL_RESOURCE_texture,
   GDRAW_GL_RESOURCE_vertexbuffer,

   GDRAW_GL_RESOURCE__count,
} gdraw_gl_resourcetype;

struct IggyCustomDrawCallbackRegion;

extern int gdraw_GL_SetResourceLimits(gdraw_gl_resourcetype type, S32 num_handles, S32 num_bytes);
extern GDrawFunctions * gdraw_GL_CreateContext(S32 min_w, S32 min_h, S32 msaa_samples);
extern void gdraw_GL_DestroyContext(void);
extern void gdraw_GL_SetTileOrigin(S32 vx, S32 vy, U32 framebuffer); // framebuffer=FBO handle, or 0 for main frame buffer
extern void gdraw_GL_NoMoreGDrawThisFrame(void);

extern GDrawTexture *gdraw_GL_WrappedTextureCreate(S32 gl_texture_handle, S32 width, S32 height, rrbool has_mipmaps);
extern void          gdraw_GL_WrappedTextureChange(GDrawTexture *tex, S32 new_gl_texture_handle, S32 new_width, S32 new_height, rrbool new_has_mipmaps);
extern void          gdraw_GL_WrappedTextureDestroy(GDrawTexture *tex);

extern void gdraw_GL_BeginCustomDraw(struct IggyCustomDrawCallbackRegion *region, F32 *matrix);
extern void gdraw_GL_EndCustomDraw(struct IggyCustomDrawCallbackRegion *region);

extern GDrawTexture * RADLINK gdraw_GL_MakeTextureFromResource(U8 *resource_file, S32 resource_len, IggyFileTextureRaw *texture);
extern void RADLINK gdraw_GL_DestroyTextureFromResource(GDrawTexture *tex);

RADDEFEND

#endif
