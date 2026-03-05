// gdraw_wgl.c - copyright 2011-2012 RAD Game Tools
//
// This implements the Iggy graphics driver layer for GL on Windows.

#define GDRAW_ASSERTS

#include "iggy.h"
#include "gdraw.h"
#include "gdraw_wgl.h"
#include <windows.h>
#include <gl/gl.h>
#include "glext.h"
#include <string.h>
#include <math.h>

#define true 1
#define false 0

///////////////////////////////////////////////////////////////////////////////
//
//  Extensions (we map to GL 2.0 function names for a uniform interface
//  across platforms)
//

#define GDRAW_GL_EXTENSION_LIST \
   /*  identifier                      import                              procname */ \
   /* GL_ARB_multitexture */ \
   GLE(ActiveTexture,                  "ActiveTextureARB",                 ACTIVETEXTUREARB) \
   /* GL_ARB_texture_compression */ \
   GLE(CompressedTexImage2D,           "CompressedTexImage2DARB",          COMPRESSEDTEXIMAGE2DARB) \
   /* GL_ARB_vertex_buffer_object */ \
   GLE(GenBuffers,                     "GenBuffersARB",                    GENBUFFERSARB) \
   GLE(DeleteBuffers,                  "DeleteBuffersARB",                 DELETEBUFFERSARB) \
   GLE(BindBuffer,                     "BindBufferARB",                    BINDBUFFERARB) \
   GLE(BufferData,                     "BufferDataARB",                    BUFFERDATAARB) \
   GLE(MapBuffer,                      "MapBufferARB",                     MAPBUFFERARB) \
   GLE(UnmapBuffer,                    "UnmapBufferARB",                   UNMAPBUFFERARB) \
   GLE(VertexAttribPointer,            "VertexAttribPointerARB",           VERTEXATTRIBPOINTERARB) \
   GLE(EnableVertexAttribArray,        "EnableVertexAttribArrayARB",       ENABLEVERTEXATTRIBARRAYARB) \
   GLE(DisableVertexAttribArray,       "DisableVertexAttribArrayARB",      DISABLEVERTEXATTRIBARRAYARB) \
   /* GL_ARB_shader_objects */ \
   GLE(CreateShader,                   "CreateShaderObjectARB",            CREATESHADEROBJECTARB) \
   GLE(DeleteShader,                   "DeleteObjectARB",                  DELETEOBJECTARB) \
   GLE(ShaderSource,                   "ShaderSourceARB",                  SHADERSOURCEARB) \
   GLE(CompileShader,                  "CompileShaderARB",                 COMPILESHADERARB) \
   GLE(GetShaderiv,                    "GetObjectParameterivARB",          GETOBJECTPARAMETERIVARB) \
   GLE(GetShaderInfoLog,               "GetInfoLogARB",                    GETINFOLOGARB) \
   GLE(CreateProgram,                  "CreateProgramObjectARB",           CREATEPROGRAMOBJECTARB) \
   GLE(DeleteProgram,                  "DeleteObjectARB",                  DELETEOBJECTARB) \
   GLE(AttachShader,                   "AttachObjectARB",                  ATTACHOBJECTARB) \
   GLE(LinkProgram,                    "LinkProgramARB",                   LINKPROGRAMARB) \
   GLE(GetUniformLocation,             "GetUniformLocationARB",            GETUNIFORMLOCATIONARB) \
   GLE(UseProgram,                     "UseProgramObjectARB",              USEPROGRAMOBJECTARB) \
   GLE(GetProgramiv,                   "GetObjectParameterivARB",          GETOBJECTPARAMETERIVARB) \
   GLE(GetProgramInfoLog,              "GetInfoLogARB",                    GETINFOLOGARB) \
   GLE(Uniform1i,                      "Uniform1iARB",                     UNIFORM1IARB) \
   GLE(Uniform4f,                      "Uniform4fARB",                     UNIFORM4FARB) \
   GLE(Uniform4fv,                     "Uniform4fvARB",                    UNIFORM4FVARB) \
   /* GL_ARB_vertex_shader */ \
   GLE(BindAttribLocation,             "BindAttribLocationARB",            BINDATTRIBLOCATIONARB) \
   /* GL_EXT_framebuffer_object */ \
   GLE(GenRenderbuffers,               "GenRenderbuffersEXT",              GENRENDERBUFFERSEXT) \
   GLE(DeleteRenderbuffers,            "DeleteRenderbuffersEXT",           DELETERENDERBUFFERSEXT) \
   GLE(BindRenderbuffer,               "BindRenderbufferEXT",              BINDRENDERBUFFEREXT) \
   GLE(RenderbufferStorage,            "RenderbufferStorageEXT",           RENDERBUFFERSTORAGEEXT) \
   GLE(GenFramebuffers,                "GenFramebuffersEXT",               GENFRAMEBUFFERSEXT) \
   GLE(DeleteFramebuffers,             "DeleteFramebuffersEXT",            DELETEFRAMEBUFFERSEXT) \
   GLE(BindFramebuffer,                "BindFramebufferEXT",               BINDFRAMEBUFFEREXT) \
   GLE(CheckFramebufferStatus,         "CheckFramebufferStatusEXT",        CHECKFRAMEBUFFERSTATUSEXT) \
   GLE(FramebufferRenderbuffer,        "FramebufferRenderbufferEXT",       FRAMEBUFFERRENDERBUFFEREXT) \
   GLE(FramebufferTexture2D,           "FramebufferTexture2DEXT",          FRAMEBUFFERTEXTURE2DEXT) \
   GLE(GenerateMipmap,                 "GenerateMipmapEXT",                GENERATEMIPMAPEXT) \
   /* GL_EXT_framebuffer_blit */ \
   GLE(BlitFramebuffer,                "BlitFramebufferEXT",               BLITFRAMEBUFFEREXT) \
   /* GL_EXT_framebuffer_multisample */ \
   GLE(RenderbufferStorageMultisample, "RenderbufferStorageMultisampleEXT",RENDERBUFFERSTORAGEMULTISAMPLEEXT) \
   /* <end> */

#define gdraw_GLx_(id)     gdraw_GL_##id
#define GDRAW_GLx_(id)     GDRAW_GL_##id
#define GDRAW_SHADERS      "gdraw_gl_shaders.inl"

typedef GLhandleARB GLhandle;
typedef gdraw_gl_resourcetype gdraw_resourcetype;

// Extensions
#define GLE(id, import, procname) static PFNGL##procname##PROC gl##id;
GDRAW_GL_EXTENSION_LIST
#undef GLE

static void load_extensions(void)
{
#define GLE(id, import, procname) gl##id = (PFNGL##procname##PROC) wglGetProcAddress("gl" import);
   GDRAW_GL_EXTENSION_LIST
#undef GLE
}

static void clear_renderstate_platform_specific(void)
{
   glDisable(GL_ALPHA_TEST);
}

static void error_msg_platform_specific(const char *msg)
{
   OutputDebugStringA(msg);
}

///////////////////////////////////////////////////////////////////////////////
//
//  Shared code
//

#define GDRAW_MULTISAMPLING
#include "gdraw_gl_shared.inl"

///////////////////////////////////////////////////////////////////////////////
//
//  Initialization and platform-specific functionality
//

GDrawFunctions *gdraw_GL_CreateContext(S32 w, S32 h, S32 msaa_samples)
{
   static const TextureFormatDesc tex_formats[] = {
      { IFT_FORMAT_rgba_8888,    1, 1,  4,   GL_RGBA,                            GL_RGBA,               GL_UNSIGNED_BYTE },
      { IFT_FORMAT_rgba_4444_LE, 1, 1,  2,   GL_RGBA4,                           GL_RGBA,               GL_UNSIGNED_SHORT_4_4_4_4 },
      { IFT_FORMAT_rgba_5551_LE, 1, 1,  2,   GL_RGB5_A1,                         GL_RGBA,               GL_UNSIGNED_SHORT_5_5_5_1 },
      { IFT_FORMAT_la_88,        1, 1,  2,   GL_LUMINANCE8_ALPHA8,               GL_LUMINANCE_ALPHA,    GL_UNSIGNED_BYTE },
      { IFT_FORMAT_la_44,        1, 1,  1,   GL_LUMINANCE4_ALPHA4,               GL_LUMINANCE_ALPHA,    GL_UNSIGNED_BYTE },
      { IFT_FORMAT_i_8,          1, 1,  1,   GL_INTENSITY8,                      GL_ALPHA,              GL_UNSIGNED_BYTE },
      { IFT_FORMAT_i_4,          1, 1,  1,   GL_INTENSITY4,                      GL_ALPHA,              GL_UNSIGNED_BYTE },
      { IFT_FORMAT_l_8,          1, 1,  1,   GL_LUMINANCE8,                      GL_LUMINANCE,          GL_UNSIGNED_BYTE },
      { IFT_FORMAT_l_4,          1, 1,  1,   GL_LUMINANCE4,                      GL_LUMINANCE,          GL_UNSIGNED_BYTE },
      { IFT_FORMAT_DXT1,         4, 4,  8,   GL_COMPRESSED_RGBA_S3TC_DXT1_EXT,   0,                     GL_UNSIGNED_BYTE },
      { IFT_FORMAT_DXT3,         4, 4, 16,   GL_COMPRESSED_RGBA_S3TC_DXT3_EXT,   0,                     GL_UNSIGNED_BYTE },
      { IFT_FORMAT_DXT5,         4, 4, 16,   GL_COMPRESSED_RGBA_S3TC_DXT5_EXT,   0,                     GL_UNSIGNED_BYTE },
      { 0,                       0, 0,  0,   0,                                  0,                     0 },
   };

   GDrawFunctions *funcs;
   const char *s;
   GLint n;

   // check for the extensions we need
   s = (const char *) glGetString(GL_EXTENSIONS);
   if (s == NULL) {
      assert(s != NULL); // if this is NULL, you're probably trying to create the device too early
      return NULL;
   }

   // check for the extensions we won't work without
   if (!hasext(s, "GL_ARB_multitexture") ||
       !hasext(s, "GL_ARB_texture_compression") ||
       !hasext(s, "GL_ARB_texture_mirrored_repeat") ||
       !hasext(s, "GL_ARB_texture_non_power_of_two") || // with caveats - see below!
       !hasext(s, "GL_ARB_vertex_buffer_object") ||
       !hasext(s, "GL_EXT_framebuffer_object") ||
       !hasext(s, "GL_ARB_shader_objects") ||
       !hasext(s, "GL_ARB_vertex_shader") ||
       !hasext(s, "GL_ARB_fragment_shader"))
      return NULL;
   
   // if user requests multisampling and HW doesn't support it, bail
   if (!hasext(s, "GL_EXT_framebuffer_multisample") && msaa_samples > 1)
      return NULL;

   load_extensions();
   funcs = create_context(w, h);
   if (!funcs)
      return NULL;

   gdraw->tex_formats = tex_formats;

   // check for optional extensions
   gdraw->has_mapbuffer = true; // part of core VBO extension on regular GL
   gdraw->has_depth24 = true;   // we just assume.
   gdraw->has_texture_max_level = true; // core on regular GL

   if (hasext(s, "GL_EXT_packed_depth_stencil"))      gdraw->has_packed_depth_stencil = true;

   // we require ARB_texture_non_power_of_two - on actual HW, this may either give us
   // "full" non-power-of-two support, or "conditional" non-power-of-two (wrap mode must
   // be CLAMP_TO_EDGE, no mipmaps). figure out which it is using this heuristic by
   // Unity's Aras Pranckevicius (thanks!):
   //   http://www.aras-p.info/blog/2012/10/17/non-power-of-two-textures/
   //
   // we use the second heuristic (texture size <8192 for cards without full NPOT support)
   // since we don't otherwise use ARB_fragment_program and don't want to create a program
   // just to be able to query MAX_PROGRAM_NATIVE_INSTRUCTIONS_ARB!
   glGetIntegerv(GL_MAX_TEXTURE_SIZE, &n);
   gdraw->has_conditional_non_power_of_two = n < 8192;

   // clamp number of multisampling levels to max supported
   if (msaa_samples > 1) {
      glGetIntegerv(GL_MAX_SAMPLES, &n);
      gdraw->multisampling = RR_MIN(msaa_samples, n);
   }

   opengl_check();

   return funcs;
}

