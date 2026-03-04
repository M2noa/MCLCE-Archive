#include "stdafx.h"

void glReadPixels(int,int, int, int, int, int, ByteBuffer *)
{
}

void glClearDepth(double)
{
}

void glVertexPointer(int, int, int, int)
{
}

void glVertexPointer(int, int, FloatBuffer *)
{
}

void glTexCoordPointer(int, int, int, int)
{
}

void glTexCoordPointer(int, int, FloatBuffer *)
{
}

void glNormalPointer(int, int, int)
{
}

void glNormalPointer(int, ByteBuffer *)
{
}

void glEnableClientState(int)
{
}

void glDisableClientState(int)
{
}

void glColorPointer(int, int, int, int)
{
}

void glColorPointer(int, bool, int, ByteBuffer *)
{
}

void glDrawArrays(int,int,int)
{
}

void glNormal3f(float,float,float)
{
}

void glGenQueriesARB(IntBuffer *)
{
}

void glBeginQueryARB(int,int)
{
}

void glEndQueryARB(int)
{
}

void glGetQueryObjectuARB(int,int,IntBuffer *)
{
}

void glShadeModel(int)
{
}

void glColorMaterial(int,int)
{
}

//1.8.2
void glClientActiveTexture(int)
{
}

void glActiveTexture(int)
{
}

void glFlush()
{
}

void glTexGeni(int,int,int)
{
}

#ifdef _XBOX
// 4J Stu - Added these to stop us needing to pull in loads of media libraries just to use Qnet
#include <xcam.h>
DWORD XCamInitialize(){ return 0; }
VOID XCamShutdown() {}
 
DWORD XCamCreateStreamEngine(
         CONST XCAM_STREAM_ENGINE_INIT_PARAMS *pParams,
         PIXCAMSTREAMENGINE *ppEngine
		 ) { return 0; }
 
DWORD XCamSetView(
         XCAMZOOMFACTOR ZoomFactor,
         LONG XCenter,
         LONG YCenter,
         PXOVERLAPPED pOverlapped
) { return 0; }
 
XCAMDEVICESTATE XCamGetStatus() { return XCAMDEVICESTATE_DISCONNECTED; }
#endif