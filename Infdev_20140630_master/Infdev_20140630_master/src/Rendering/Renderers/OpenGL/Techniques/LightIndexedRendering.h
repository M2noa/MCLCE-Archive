#pragma once

#include <vector>
#include "../OpenGLRenderer.h"
#include "../Objects/OpenGLRenderObject.h"
#include "../../../IRenderTechnique.h"
#include "./Shadowing/ShadowMap.h"
#include "./Shadowing/CascadedShadowMap.h"
#include "./Lighting/Lighting.h"

namespace Esteem
{
	namespace OpenGL
	{
		class LightIndexedRendering : public RenderTechnique<OpenGLRenderData, OpenGLRenderer>
		{
			friend class OpenGLRenderer;
			friend class DeferredRendering;
			friend class LightIndexedRendering;

		private:
			ShadowMap shadowMapTechnique;
			CascadedShadowMap cascadedShadowMapTechnique;
			Lighting* lightingTechnique;

			cgc::strong_ptr<FBO> indexFrameBuffer;
			cgc::strong_ptr<OpenGLShader> lightIndexShader;

			cgc::strong_ptr<OpenGLShader> depthShader;
			cgc::strong_ptr<OpenGLShader> depthShaderCutoff;

			cgc::strong_ptr<Texture2D> indexTexture;

			GLint clearColor[4];

			LightIndexedRendering(OpenGLRenderer& renderer);

		public:
			virtual ~LightIndexedRendering();

			virtual void RenderFrame(const OpenGLRenderData& renderData);
			void RenderOpaque(const OpenGLRenderData& renderData);
			void RenderTransparent(const OpenGLRenderData& renderData);

			virtual void Initialize();
		};
	}
}