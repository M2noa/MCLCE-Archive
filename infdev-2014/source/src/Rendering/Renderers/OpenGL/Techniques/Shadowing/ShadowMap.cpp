#include "ShadowMap.h"

#include "Math/Math.h"

#include "General/Settings.h"
#include "Utils/Diagnostics.h"

#include "../Lighting/DirectionalLighting.h"

#include "../../OpenGLDebug.h"
#include "../../OpenGLRenderCalls.h"
#include "../../OpenGLFactory.h"

namespace Esteem
{
	namespace OpenGL
	{
		ShadowMap::ShadowMap(OpenGLRenderer& renderer)
			: RenderTechnique(renderer)
		{

		}

		void ShadowMap::Initialize()
		{
			int x = 0;
			int y = 0;
			int rows = 4;
			int width = 256;
			int textureWidth = 1024;

			shadowInfo.reserve(64);
			for (int i = 0; i < 64; ++i)
			{
				ShadowMapInfo mapInfo;
				mapInfo.startUV = glm::vec2((float)x / (float)textureWidth, (float)y / (float)textureWidth);
				mapInfo.endUV = glm::vec2(mapInfo.startUV.x + (float)width / (float)textureWidth, mapInfo.startUV.y + (float)width / (float)textureWidth);
				shadowInfo.push_back(mapInfo);

				x += width;
				if (x >= textureWidth)
				{
					x = 0;
					y += width;
				}
			}

			ConstructSettings::Texture2D settings;
			settings.size = Settings::shadowAtlasSize;
			settings.mipmapped = false;
			settings.repeat = TEXTURE_REPEAT::CLAMP_TO_BORDER;
			settings.textureFilter = TEXTURE_FILTER::LINEAR;
			settings.mipMapFilter = TEXTURE_FILTER::LINEAR_MIPMAP_LINEAR;
			settings.format = TEXTURE_FORMAT::DEPTH_24U;
			settings.borderColor = Color(0, 0, 0, 1);

			//shadowTexture = factory->LoadTexture2D("@ShadowDepthTexture", Settings::shadowAtlasSize, GL_DEPTH_COMPONENT24, GL_DEPTH_COMPONENT, GL_LINEAR, GL_CLAMP_TO_BORDER, glm::vec4(0, 0, 0, 1), false);
			shadowTexture = renderer.GetOpenGLFactory().LoadTexture2D(settings, "@ShadowDepthTexture");
			shadowFBO = renderer.GetOpenGLFactory().LoadFBO("ShadowLightingFBO");
			shadowFBO->AttachDepthTexture(shadowTexture);
			shadowFBO->Validate();

			shadowUBO = cgc::static_pointer_cast<UBO>(renderer.GetOpenGLFactory().LoadUBO(&shadowInfo[0], 64 * sizeof(ShadowMapInfo), "shadowMapInfoUB"));
			shader = renderer.GetOpenGLFactory().LoadOpenGLShader("ShadowMap");
			shaderCutoff = renderer.GetOpenGLFactory().LoadOpenGLShader("ShadowMap_cutoff");

			projectionMatrix = glm::mat4(glm::perspective<float>(Math::ToRadians(91.f), 1.f, 1.f, 1024.f)); // Projection Matrix
		}

		void ShadowMap::RenderFrame(const OpenGLRenderData& renderData)
		{
			shadowFBO->Bind();
			shader->Bind();

			glDepthMask(GL_TRUE);
			glCullFace(GL_FRONT);
			glDisable(GL_BLEND);

			glClearColor(1.f, 1.f, 1.f, 1.f);
			glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

			int x = 0;
			int y = 0;
			int rows = 4;
			int width = 256;
			int textureWidth = 1024;

			int shadowIndex = 0;

			for (const auto& light : *renderData.GetLights())
			{
				if (light.data->shadowInfoIndex == -1)
					continue;

				light.data->SetShadowMapIndex(shadowIndex);

				switch (light.data->type)
				{
				case LightData::LightType::DIRECTIONAL:
				{
					CascadedShadowMap* csmShadowMap = (CascadedShadowMap*)dirLightingTechnique;
					csmShadowMap->RenderFrame(renderData, *light.matrix, light.data->forward);

					auto& cascadedMatrices = csmShadowMap->GetCascadedMatrices();
					shadowInfo[shadowIndex++].shadowMatrix = cascadedMatrices[0];
					shadowInfo[shadowIndex++].shadowMatrix = cascadedMatrices[1];
					shadowInfo[shadowIndex++].shadowMatrix = cascadedMatrices[2];
					shadowInfo[shadowIndex++].shadowMatrix = cascadedMatrices[3];

					// csm technique will set his own fbo so set it back for other lights
					shadowFBO->Bind();
				}
				break;
				case LightData::LightType::POINT:
					shader->SetUniform(CT_HASH("lightDistance"), light.data->distance);
					shader->SetUniform(CT_HASH("orthographic"), false);

					for (uint j = 0; j < 6; ++j)
					{
						glViewport(x, y, width, width);
						shadowInfo[shadowIndex].shadowMatrix = projectionMatrix * GetCubeMapDirectionMatrix(GL_TEXTURE_CUBE_MAP_POSITIVE_X + j, light.data->position);
						shader->SetUniform(CT_HASH("CameraMVP"), shadowInfo[shadowIndex].shadowMatrix);

						const OpenGLRenderList& openGLRenderList = renderData.GetRenderList();
						const std::vector<OpenGLRenderObject*>* opaqueRenderList = &openGLRenderList[(size_t)RenderObject::RenderOrder::OPAQUE_];

						for (uint i = 0; i < opaqueRenderList->size(); ++i)
						{
							OpenGLRenderObject* renderObject = opaqueRenderList->operator[](i);
							if (renderObject->IsCastingShadows())
							{
								RenderObject(shader, renderObject);

								DrawRenderObject(*renderObject);
								//DrawElementsOnce(renderObject);

								Diagnostics::drawCalls++;
							}
						}

						x += width;
						if (x >= textureWidth)
						{
							x = 0;
							y += width;
						}
						++shadowIndex;
					}

					break;
				case LightData::LightType::SPOT:
					shader->SetUniform(CT_HASH("lightDistance"), light.data->distance);
					shader->SetUniform(CT_HASH("orthographic"), false);

					glViewport(x, y, width, width);
					shadowInfo[shadowIndex].shadowMatrix = projectionMatrix * glm::lookAt(light.data->position, light.data->position - light.data->forward, glm::vec3(0, 1, 0));
					shader->SetUniform(CT_HASH("CameraMVP"), shadowInfo[shadowIndex].shadowMatrix);

					const OpenGLRenderList& openGLRenderList = renderData.GetRenderList();
					const std::vector<OpenGLRenderObject*>* opaqueRenderList = &openGLRenderList[(size_t)RenderObject::RenderOrder::OPAQUE_];

					for (uint i = 0; i < opaqueRenderList->size(); ++i)
					{
						OpenGLRenderObject* renderObject = opaqueRenderList->operator[](i);
						if (renderObject->IsCastingShadows())
						{
							RenderObject(shader, renderObject);

							DrawRenderObject(*renderObject);
							//DrawElementsOnce(renderObject);

							Diagnostics::drawCalls++;
						}
					}

					x += width;
					if (x >= textureWidth)
					{
						x = 0;
						y += width;
					}
					++shadowIndex;
					break;
				}
			}

			VAO::UnBind();
			FBO::UnBind(renderer);

			shadowUBO->UpdateBuffer(&shadowInfo[0], shadowInfo.size() * sizeof(ShadowMapInfo));
		}

		void ShadowMap::RenderFrame(const OpenGLRenderData& renderData, const glm::mat4& matrix, float distance, bool orthographic)
		{
			glDisable(GL_CULL_FACE);

			cgc::strong_ptr<OpenGLShader> curShader = shader;
			bool cutoff = false;

			shaderCutoff->Bind(matrix);
			shaderCutoff->SetUniform(CT_HASH("lightDistance"), distance);
			shaderCutoff->SetUniform(CT_HASH("orthographic"), orthographic);

			shader->Bind(matrix);
			shader->SetUniform(CT_HASH("lightDistance"), distance);
			shader->SetUniform(CT_HASH("orthographic"), orthographic);

			const OpenGLRenderList& openGLRenderList = renderData.GetRenderList();
			const std::vector<OpenGLRenderObject*>* opaqueRenderList = &openGLRenderList[(size_t)RenderObject::RenderOrder::OPAQUE_];

			for (uint i = 0; i < opaqueRenderList->size(); ++i)
			{
				OpenGLRenderObject* renderObject = opaqueRenderList->operator[](i);
				if (renderObject->IsCastingShadows())
				{
					if (cutoff != shader->IsCutOff())
					{
						cutoff = !cutoff;
						(cutoff ? shaderCutoff : shader)->Bind();
					}

					if (cutoff)
					{
						static_assert("bind all buffer");
						/*glActiveTexture(GL_TEXTURE0 );
						auto map = renderObject->GetMaterial()->GetTextures();
						if(map[0].first == CT_HASH("tex"))
							glBindTexture(GL_TEXTURE_2D, map[0].second->GetID());
						shader->SetUniform(CT_HASH("tex"), 0);*/
					}

					RenderObject(shader, renderObject);

					DrawRenderObject(*renderObject);
					//DrawElementsOnce(renderObject);

					Diagnostics::drawCalls++;
				}
			}
			VAO::UnBind();
			glEnable(GL_CULL_FACE);

		}

		void ShadowMap::RenderFrame(const OpenGLRenderData& renderData, const glm::mat4& matrix, float distance, const cgc::strong_ptr<FBO>& fbo)
		{
			fbo->Bind();

			shader->Bind(matrix);
			shader->SetUniform(CT_HASH("lightDistance"), distance);
			shader->SetUniform(CT_HASH("orthographic"), 0);

			//glViewport(0, 0, 1024, 512);
			glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
			glColorMask(false, false, false, false);

			const OpenGLRenderList& openGLRenderList = renderData.GetRenderList();
			const std::vector<OpenGLRenderObject*>* transparentRenderList = &openGLRenderList[(size_t)RenderObject::RenderOrder::TRANSLUCENT];

			for (uint i = 0; i < transparentRenderList->size(); ++i)
			{
				OpenGLRenderObject* renderObject = transparentRenderList->operator[](i);
				if (renderObject->IsCastingShadows())
				{
					RenderObject(shader, renderObject);

					DrawRenderObject(*renderObject);
					//DrawElementsOnce(renderObject);

					Diagnostics::drawCalls++;
				}

			}
			VAO::UnBind();
			glColorMask(true, true, true, true);

			FBO::UnBind(renderer);
		}

		glm::mat4 ShadowMap::GetCubeMapDirectionMatrix(const int cubemapSide, glm::vec3 position)
		{
			// View Matrices
			switch (cubemapSide)
			{
			case GL_TEXTURE_CUBE_MAP_POSITIVE_X:
				return glm::lookAt(position, position + glm::vec3(1, 0, 0), glm::vec3(0, 1, 0));
			case GL_TEXTURE_CUBE_MAP_NEGATIVE_X:
				return glm::lookAt(position, position + glm::vec3(-1, 0, 0), glm::vec3(0, 1, 0));
			case GL_TEXTURE_CUBE_MAP_POSITIVE_Y:
				return glm::lookAt(position, position + glm::vec3(0, 1, 0), glm::vec3(0, 0, 1));
			case GL_TEXTURE_CUBE_MAP_NEGATIVE_Y:
				return glm::lookAt(position, position + glm::vec3(0, -1, 0), glm::vec3(0, 0, -1));
			case GL_TEXTURE_CUBE_MAP_POSITIVE_Z:
				return glm::lookAt(position, position + glm::vec3(0, 0, 1), glm::vec3(0, 1, 0));
			case GL_TEXTURE_CUBE_MAP_NEGATIVE_Z:
				return glm::lookAt(position, position + glm::vec3(0, 0, -1), glm::vec3(0, 1, 0));
			default:
				return glm::mat4();
			}
		}

		ShadowMap::~ShadowMap()
		{

		}
	}
}