#pragma once

#include <vector>
#include "../../OpenGLRenderer.h"
#include "../../OpenGLRenderData.h"
#include "../../Objects/OpenGLRenderObject.h"
#include "../../../../IRenderTechnique.h"

namespace Esteem
{
	namespace OpenGL
	{
		class SixSidedShadowMap : public RenderTechnique<OpenGLRenderData*, OpenGLRenderer>
		{
		};
	}
}
