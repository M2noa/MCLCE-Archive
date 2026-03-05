#pragma once

#include <vector>
#include "../../OpenGLRenderer.h"
#include "../../OpenGLRenderData.h"
#include "../../Objects/OpenGLRenderObject.h"
#include "../../../../IRenderTechnique.h"
#include "./ShadowMap.h"

namespace Esteem
{
	namespace OpenGL
	{
		class CascadedShadowMap : public RenderTechnique<OpenGLRenderData, OpenGLRenderer>
		{
		private:
			ShadowMap& shadowMapTechnique;

			cgc::strong_ptr<FBO> cascadedShadowFBO;
			cgc::strong_ptr<Texture2D> cascadedShadowTexture;

			std::vector<glm::vec3> cascadedDistances;
			std::vector<glm::mat4> cascadedMatrices;

			uint renderNextIndex;

			void CalculateCascadedDistances(uint numberOfCascades, float firstRadius, float fov, float nearDistance, float farDistance, float increment);

		public:
			CascadedShadowMap(OpenGLRenderer& renderer, ShadowMap& shadowMapTechnique);

			virtual void Initialize();

			virtual void RenderFrame(const OpenGLRenderData& renderData);
			void RenderFrame(const OpenGLRenderData& renderData, const glm::mat4& lightMatrix, const glm::vec3& lightDirection);

			inline const cgc::strong_ptr<Texture2D>& GetCascadedShadowTexture() { return cascadedShadowTexture; };
			inline const std::vector<glm::mat4>& GetCascadedMatrices() { return cascadedMatrices; };
		};
	}
}