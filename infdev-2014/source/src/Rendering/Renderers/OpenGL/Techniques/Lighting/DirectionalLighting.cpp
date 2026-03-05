#include "DirectionalLighting.h"

#include "World/Constituents/Light.h"
#include "../../OpenGLFactory.h"

#include "Utils/Diagnostics.h"

namespace Esteem
{
	namespace OpenGL
	{
		DirectionalLighting::DirectionalLighting(OpenGLRenderer& renderer, const cgc::strong_ptr<Texture2D>& colorTexture, const cgc::strong_ptr<Texture2D>& normalsTexture,
			const cgc::strong_ptr<Texture2D>& depthTexture, const cgc::strong_ptr<Texture2D>& testMaterial, std::vector<ShadowMapInfo>& shadowInfo)
			: RenderTechnique(renderer)
			, colorTexture(colorTexture)
			, normalsTexture(normalsTexture)
			, depthTexture(depthTexture)
			, testMaterial(testMaterial)
			, shadowInfo(shadowInfo)
		{

		}

		void DirectionalLighting::Initialize()
		{
			lightingShader = renderer.GetOpenGLFactory().LoadOpenGLShader("DeferredCascadedShadows");

			//noiseTexture = renderer.GetOpenGLFactory().LoadTexture2D("POST_PROCESS/SSAO/noise");

			// The fullscreen quad's FBO
			glGenVertexArrays(1, &quadVAO);
			glBindVertexArray(quadVAO);

			static const GLfloat vertexData[] = {
				-1.0f, -1.0f, 0.0f, 0.f, 0.f,
				1.0f, -1.0f, 0.0f, 1.f, 0.f,
				-1.0f, 1.0f, 0.0f, 0.f, 1.f,
				1.0f, 1.0f, 0.0f, 1.f, 1.f
			};

			uint vbo;
			glGenBuffers(1, &vbo);
			glBindBuffer(GL_ARRAY_BUFFER, vbo);
			glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);

			uint vertexIndex = lightingShader->GetAttributeVariable("vertex")->index;
			glEnableVertexAttribArray(vertexIndex);
			glVertexAttribPointer(vertexIndex, 3, GL_FLOAT, false, sizeof(vertexData) / 4, 0);

			auto uvVariable = lightingShader->GetAttributeVariable("uv");
			if(uvVariable != nullptr)
			{
				glEnableVertexAttribArray(uvVariable->index);
				glVertexAttribPointer(uvVariable->index, 2, GL_FLOAT, true, sizeof(vertexData) / 4, (void*)12);
			}

			static const uint indices[] = { 0, 1, 2, 1, 3, 2 };

			uint ebo;
			glGenBuffers(1, &ebo);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

			glBindVertexArray(0);
		}

		void DirectionalLighting::SetTechnique(DirectionLightingTechnique lightingTechnique, RenderTechnique<OpenGLRenderData, OpenGLRenderer>& shadowMapTechnique)
		{
			this->lightingTechnique = lightingTechnique;
			switch(lightingTechnique)
			{
			case DirectionLightingTechnique::CASCADED_SHADOW_MAPS:
				CascadedShadowMap* csmTechnique = (CascadedShadowMap*)&shadowMapTechnique;
				this->shadowMapTechnique = csmTechnique;
				this->shadowTexture = csmTechnique->GetCascadedShadowTexture();
				this->cascadedMatrices = csmTechnique->GetCascadedMatrices().data();
				break;
			}
		}

		void DirectionalLighting::RenderFrame(const OpenGLRenderData& renderData)
		{
			switch(lightingTechnique)
			{
			case DirectionLightingTechnique::CASCADED_SHADOW_MAPS:
				RenderFrameCSM(renderData);
				break;
			}
		}

		void DirectionalLighting::RenderShadows(const OpenGLRenderData& renderData, const LightRenderData& light)
		{
			static_cast<CascadedShadowMap*>(shadowMapTechnique)->RenderFrame(renderData, *light.matrix, light.data->forward);
			size_t shadowIndex = light.data->shadowInfoIndex;

			for (size_t i = 0; i < 4; ++i)
				shadowInfo[shadowIndex + i].shadowMatrix = cascadedMatrices[i];
		}

		void DirectionalLighting::RenderFrameCSM(const OpenGLRenderData& renderData)
		{
			lightingShader->Bind();
			renderer.ActivateTexture(0, CT_HASH("diffuseTexture"), colorTexture.ptr(), lightingShader.ptr());
			renderer.ActivateTexture(1, CT_HASH("normalsTexture"), normalsTexture.ptr(), lightingShader.ptr());
			renderer.ActivateTexture(2, CT_HASH("depthTexture"), depthTexture.ptr(), lightingShader.ptr());
			renderer.ActivateTexture(3, CT_HASH("materialTexture"), testMaterial.ptr(), lightingShader.ptr());
			renderer.ActivateTexture(4, CT_HASH("csmShadowTexture"), shadowTexture.ptr(), lightingShader.ptr());
			//renderer->ActivateTexture(5, CT_HASH("noiseTexture"), noiseTexture.ptr(), lightingShader.ptr());

			glEnable(GL_BLEND);
			glDepthMask(GL_FALSE);
			glBlendFunc(GL_ONE, GL_ONE);
			glBindVertexArray(quadVAO);

			for(const auto& light : *renderData.GetLights())
			{
				if (light.data->type == LightData::LightType::DIRECTIONAL)
				{
					lightingShader->SetUniform(CT_HASH("lightID"), (int)light.index);
					glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

					Diagnostics::drawCalls++;
				}
			}

			glBindVertexArray(0);
			glDepthMask(GL_TRUE);
		}
	}
}