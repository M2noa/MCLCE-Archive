#pragma once

#include <vector>
#include "../../OpenGLRenderer.h"
#include "../../OpenGLRenderData.h"
#include "../../Objects/OpenGLRenderObject.h"
#include "../../../../IRenderTechnique.h"
#include "../Shadowing/CascadedShadowMap.h"
#include "Utils/Time.h"

namespace Esteem
{
	class Light;

	namespace OpenGL
	{
		enum DirectionLightingTechnique
		{
			CASCADED_SHADOW_MAPS
		};

		class DirectionalLighting : public RenderTechnique<OpenGLRenderData, OpenGLRenderer>
		{
		private:
			DirectionLightingTechnique lightingTechnique;
			RenderTechnique<OpenGLRenderData, OpenGLRenderer>* shadowMapTechnique;
			cgc::strong_ptr<OpenGLShader> lightingShader;

			cgc::strong_ptr<Texture2D> colorTexture;
			cgc::strong_ptr<Texture2D> normalsTexture;
			cgc::strong_ptr<Texture2D> depthTexture;
			cgc::strong_ptr<Texture2D> shadowTexture;
			cgc::strong_ptr<Texture2D> testMaterial;
			cgc::strong_ptr<Texture2D> noiseTexture;

			uint quadVAO;

			std::vector<ShadowMapInfo> shadowInfo;
			const glm::mat4* cascadedMatrices;

			inline void RenderFrameCSM(const OpenGLRenderData& renderData);

		public:
			DirectionalLighting(OpenGLRenderer& renderer, const cgc::strong_ptr<Texture2D>& colorTexture, const cgc::strong_ptr<Texture2D>& normalsTexture,
				const cgc::strong_ptr<Texture2D>& depthTexture, const cgc::strong_ptr<Texture2D>& testMaterial, std::vector<ShadowMapInfo>& shadowInfo);

			virtual void Initialize();

			void SetTechnique(DirectionLightingTechnique lightingTechnique, RenderTechnique<OpenGLRenderData, OpenGLRenderer>& shadowMapTechnique);

			void RenderShadows(const OpenGLRenderData& renderData, const LightRenderData& light);
			virtual void RenderFrame(const OpenGLRenderData& renderData);
		};
	}
}