#pragma once

#include <vector>

#include "../OpenGLRenderer.h"
#include "../Objects/OpenGLRenderObject.h"
#include "../../../IRenderTechnique.h"

namespace Esteem
{
	namespace OpenGL
	{
		class OverlayRendering : public RenderTechnique<OpenGLRenderData, OpenGLRenderer>
		{
		friend class OpenGLRenderer;
		friend class DeferredRendering;
		friend class LightIndexedRendering;

		private:
			OverlayRendering(OpenGLRenderer& renderer);

		public:
			virtual ~OverlayRendering();

			virtual void RenderFrame(const OpenGLRenderData& renderData);

			virtual void Initialize();
		};
	}
}