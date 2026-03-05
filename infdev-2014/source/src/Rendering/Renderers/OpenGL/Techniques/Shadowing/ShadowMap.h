#pragma once

#include <vector>
#include "../../Objects/VAO.h"
#include "../../OpenGLDebug.h"
#include "../../OpenGLRenderer.h"
#include "../../OpenGLRenderData.h"
#include "../../Objects/OpenGLRenderObject.h"
#include "../../Objects/OpenGLBoneMatrices.h"
#include "../../../../IRenderTechnique.h"

namespace Esteem
{
	namespace OpenGL
	{
		class DirectionalLighting;
		class FBO;

		struct alignas(16) ShadowMapInfo // 20 units
		{
			glm::mat4 shadowMatrix;
			glm::vec2 startUV;
			glm::vec2 endUV;
		};

		class ShadowMap : public RenderTechnique<OpenGLRenderData, OpenGLRenderer>
		{
		private:
			cgc::strong_ptr<OpenGLShader> shader;
			cgc::strong_ptr<OpenGLShader> shaderCutoff;
			glm::mat4 projectionMatrix;

			std::vector<ShadowMapInfo> shadowInfo;
			cgc::strong_ptr<UBO> shadowUBO;
			RenderTechnique<OpenGLRenderData, OpenGLRenderer>* dirLightingTechnique;

			glm::mat4 GetCubeMapDirectionMatrix(const int cubemapSide, glm::vec3 position);

			inline void RenderObject(const cgc::strong_ptr<OpenGLShader>& shader, OpenGLRenderObject* renderObject)
			{
				shader->SetUniform(CT_HASH("ModelMatrix"), *renderObject->GetModelMatrix());
				renderObject->GetVAO()->Bind();

				if (renderObject->boneMatrices)
				{
					shader->SetUniform(CT_HASH("animations"), 1);
					static_cast<OpenGLBoneMatrices*>(renderObject->boneMatrices.ptr())->GetUBO()->Bind();
				}
				else
					shader->SetUniform(CT_HASH("animations"), 0);
			}

		public:
			cgc::strong_ptr<Texture2D> shadowTexture;
			cgc::strong_ptr<FBO> shadowFBO;

			ShadowMap(OpenGLRenderer& renderer);
			~ShadowMap();

			virtual void Initialize();
			virtual void RenderFrame(const OpenGLRenderData& renderData);

			void SetDirectionalTechnique(RenderTechnique<OpenGLRenderData, OpenGLRenderer>* dirLightingTechnique)
			{
				if (dirLightingTechnique != nullptr)
					this->dirLightingTechnique = dirLightingTechnique;
			}

			void RenderFrame(const OpenGLRenderData& renderData, const glm::mat4& matrix, float distance, bool orthographic = false);
			void RenderFrame(const OpenGLRenderData& renderData, const glm::mat4& matrix, float distance, const cgc::strong_ptr<FBO>& fbo);

			inline std::vector<ShadowMapInfo>& GetShadowInfo() { return shadowInfo; }
			inline const std::vector<ShadowMapInfo>& GetShadowInfo() const { return shadowInfo; }

			inline const cgc::strong_ptr<OpenGLShader>& GetShader() const { return shader; }
		};
	}
}