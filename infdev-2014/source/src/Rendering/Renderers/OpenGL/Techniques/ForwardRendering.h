#pragma once

#include <vector>

#include "../OpenGLRenderer.h"
#include "../Objects/OpenGLRenderObject.h"
#include "../../../IRenderTechnique.h"
#include "./SkyBox.h"
#include "./Shadowing/ShadowMap.h"
#include "./Shadowing/CascadedShadowMap.h"

namespace Esteem
{
	namespace OpenGL
	{
		class ForwardRendering : public RenderTechnique<OpenGLRenderData, OpenGLRenderer>
		{
		friend class OpenGLRenderer;
		friend class DeferredRendering;
		friend class LightIndexedRendering;

		private:
			cgc::strong_ptr<FBO> depthFrameBuffer;
			ShadowMap shadowMapTechnique;
			CascadedShadowMap cascadedShadowMapTechnique;

			SkyBox skyBoxTechnique;

			cgc::strong_ptr<Texture2D> colorTexture;
			uint quadVAO;
			cgc::strong_ptr<OpenGLShader> renderToTargetShader;

			ForwardRendering(OpenGLRenderer& renderer);

		public:
			virtual ~ForwardRendering() = default;

			virtual void RenderFrame(const OpenGLRenderData& renderData);
			void RenderOpaque(const OpenGLRenderData& renderData);
			void RenderTransparent(const OpenGLRenderData& renderData);

			virtual void Initialize();
		};
	}
}