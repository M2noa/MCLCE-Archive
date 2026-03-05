#pragma once

#include <vector>
#include "../OpenGLRenderer.h"
#include "../OpenGLRenderData.h"
#include "./ForwardRendering.h"
#include "../Objects/OpenGLRenderObject.h"
#include "../../../IRenderTechnique.h"

#include "./Lighting/Lighting.h"
#include "./Lighting/DirectionalLighting.h"
#include "./Shadowing/ShadowMap.h"
#include "./Shadowing/CascadedShadowMap.h"
#include "./PostProcessing.h"

namespace Esteem
{
	namespace OpenGL
	{
		class DeferredRendering : public RenderTechnique<OpenGLRenderData, OpenGLRenderer>
		{
		friend class OpenGLRenderer;
		private:
			OpenGLFactory* factory;
			cgc::strong_ptr<FBO> deferredFrameBuffer;
			cgc::strong_ptr<Texture2D> colorBuffer;
			cgc::strong_ptr<OpenGLShader> debugShader;
			uint quadVAO;

			PostProcessing* postProcessingTechnique;
			Lighting* lightingTechnique;
			ShadowMap* shadowMapTechnique;
			CascadedShadowMap* cascadedShadowMapTechnique;
			DirectionalLighting* directionalLightingTechnique;
			
			ForwardRendering* forwardRenderingTechnique;

			glm::vec3 lightDirection;

			DeferredRendering(OpenGLRenderer& renderer);

		public:
			virtual ~DeferredRendering();

			virtual void RenderFrame(const OpenGLRenderData& renderData);
			void DrawTextureOnScreen(const cgc::raw_ptr<Texture2D>& texture, glm::vec4& rect);

			virtual void Initialize();

		};
	}
}