#include "DeferredRendering.h"

#include "../OpenGLDebug.h"
#include "../OpenGLFactory.h"
#include "../OpenGLRenderCalls.h"

#include "General/Settings.h"
#include "Utils/Diagnostics.h"

#include "Rendering/Objects/Image.h"

namespace Esteem
{
	namespace OpenGL
	{
		DeferredRendering::DeferredRendering(OpenGLRenderer& renderer)
			: RenderTechnique(renderer)
		{

		}

		void DeferredRendering::Initialize()
		{
			debugShader = factory->LoadOpenGLShader("RenderToTarget");

			// The fullscreen quad's FBO
			glGenVertexArrays(1, &quadVAO);
			glBindVertexArray(quadVAO);

			static const GLfloat vertexData[] = {
				-1.0f, -1.0f, 0.0f, 0.f, 0.f,
				1.0f, -1.0f, 0.0f, 1.f, 0.f,
				-1.0f, 1.0f, 0.0f, 0.f, 1.f,
				1.0f, 1.0f, 0.0f, 1.f, 1.f
			};

			/*static const GLfloat vertexData[] = {
				0.7f, 0.7f, 0.0f, 0.f, 0.f,
				1.0f, 0.7f, 0.0f, 1.f, 0.f,
				0.7f, 1.0f, 0.0f, 0.f, 1.f,
				1.0f, 1.0f, 0.0f, 1.f, 1.f
				};*/

			uint vbo;
			glGenBuffers(1, &vbo);
			glBindBuffer(GL_ARRAY_BUFFER, vbo);
			glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);

			uint vertexIndex = debugShader->GetAttributeVariable("vertex")->index;
			glEnableVertexAttribArray(vertexIndex);
			glVertexAttribPointer(vertexIndex, 3, GL_FLOAT, false, sizeof(vertexData) / 4, 0);

			uint uvIndex = debugShader->GetAttributeVariable("uv")->index;
			glEnableVertexAttribArray(uvIndex);
			glVertexAttribPointer(uvIndex, 2, GL_FLOAT, true, sizeof(vertexData) / 4, (void*)12);

			static const uint indices[] = { 0, 1, 2, 1, 3, 2 };

			uint ebo;
			glGenBuffers(1, &ebo);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

			glBindVertexArray(0);

			deferredFrameBuffer = factory->LoadFBO("@DeferredFrameBuffer");

			cgc::strong_ptr<Texture2D> colorTexture;// = factory->LoadTexture2D("@ScreenColorBuffer", renderer->GetScreenSize(), GL_RGBA16, GL_RGBA, GL_LINEAR, GL_CLAMP_TO_EDGE, false);
			cgc::strong_ptr<Texture2D> normalsTexture;// = factory->LoadTexture2D("@ScreenNormalsBuffer", renderer->GetScreenSize(), GL_RGB16F, GL_RGB, GL_LINEAR, GL_CLAMP_TO_EDGE, false);
			cgc::strong_ptr<Texture2D> depthTexture;// = factory->LoadTexture2D("@ScreenDepthBuffer", renderer->GetScreenSize(), GL_DEPTH_COMPONENT24, GL_DEPTH_COMPONENT, GL_NEAREST, GL_CLAMP_TO_EDGE, false);

			deferredFrameBuffer->AttachRenderTexture(colorTexture, 0);
			deferredFrameBuffer->AttachRenderTexture(normalsTexture, 1);
			deferredFrameBuffer->AttachDepthTexture(depthTexture);

			deferredFrameBuffer->Validate();

			forwardRenderingTechnique = new ForwardRendering(renderer);

			// testing values
			cgc::strong_ptr<Image> image = cgc::construct_new<Image>();
			image->SaveToFile("MATERIALS/BlackToWhite.tga");
			ConstructSettings::Texture2D textureSettings;
			textureSettings.format = TEXTURE_FORMAT::RGB;
			textureSettings.repeat = TEXTURE_REPEAT::CLAMP_TO_EDGE;
			textureSettings.mipmapped = false;
			cgc::strong_ptr<Texture2D> testMaterial = factory->LoadTexture2D(textureSettings, image);
			lightDirection = glm::normalize(glm::vec3(0, -1, -1));

			// initialize Post Processing
			postProcessingTechnique = new PostProcessing(renderer, colorTexture, normalsTexture, depthTexture);
			postProcessingTechnique->Initialize();

			// default shadow map technique
			shadowMapTechnique = new ShadowMap(renderer);
			shadowMapTechnique->Initialize();

			// initialize point light techniques
			cgc::strong_ptr<OpenGLShader> lightingShader = renderer.GetOpenGLFactory().LoadOpenGLShader("DeferredShadowLight");

			std::unordered_map<std::string, cgc::strong_ptr<Texture2D>> textures
			{
				{ "diffuseTexture", colorTexture },
				{ "normalsTexture", normalsTexture },
				{ "depthTexture", depthTexture },
				{ "shadowAtlas", shadowMapTechnique->shadowTexture },
				{ "materialTexture", testMaterial }
			};
			lightingTechnique = new Lighting(renderer, lightingShader, textures);
			lightingTechnique->Initialize();

			// initialize cascaded shadow maps techniques
			cascadedShadowMapTechnique = new CascadedShadowMap(renderer, *shadowMapTechnique);
			cascadedShadowMapTechnique->Initialize();
			shadowMapTechnique->SetDirectionalTechnique(cascadedShadowMapTechnique);

			// initialize directional lighting technique
			directionalLightingTechnique = new DirectionalLighting(renderer, colorTexture, normalsTexture, depthTexture, testMaterial, shadowMapTechnique->GetShadowInfo());
			directionalLightingTechnique->Initialize();
			directionalLightingTechnique->SetTechnique(DirectionLightingTechnique::CASCADED_SHADOW_MAPS, *cascadedShadowMapTechnique);

			// use this to draw an example texture on screen
			//colorBuffer = depthTexture;
			colorBuffer = cascadedShadowMapTechnique->GetCascadedShadowTexture();
			//colorBuffer = shadowMapTechnique->shadowTexture;
		}

		void DeferredRendering::RenderFrame(const OpenGLRenderData& renderData)
		{
			//renderer->ClearActiveTextures();
			// fill shadow atlas
			shadowMapTechnique->RenderFrame(renderData);
			renderer.GetLightsUBO()->UpdateBuffer(factory->GetLightsData().data(), factory->GetLightsData().size() * sizeof(LightData));

			deferredFrameBuffer->Bind();

			glClearColor(0, 0, 0, 1);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			//glEnable(GL_BLEND);
			//glAlphaFunc(GL_GREATER, 0.5f);
			//glEnable(GL_ALPHA_TEST);

			if (Settings::drawLines)
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

			const OpenGLRenderList& openGLRenderList = renderData.GetRenderList();
			const std::vector<OpenGLRenderObject*>* opaqueRenderList = &openGLRenderList[RenderObject::RenderOrder::OPAQUE_];

			for (uint i = 0; i < opaqueRenderList->size(); ++i)
			{
				OpenGLRenderObject* renderObject = (*opaqueRenderList)[i];
				cgc::raw_ptr<OpenGLMaterial> material = static_cast<OpenGLMaterial*>(renderObject->GetMaterial().ptr());
				cgc::raw_ptr<OpenGLShader> shader = static_cast<OpenGLShader*>(material->GetOpenGLShader().ptr());
				cgc::raw_ptr<VAO> vao = renderObject->GetVAO();

				shader->Bind();
				shader->SetUniform(CT_HASH("ModelMatrix"), *renderObject->GetModelMatrix());
				material->Bind();
				vao->Bind();

				if (renderObject->boneMatrices != nullptr)
				{
					shader->SetUniform(CT_HASH("animations"), 1);
					static_cast<OpenGLBoneMatrices*>(renderObject->boneMatrices.ptr())->GetUBO()->Bind(shader);
				}
				else
					shader->SetUniform(CT_HASH("animations"), 0);

				DrawRenderObject(*renderObject);
				//DrawElements(vao->GetEBO(), renderObject->GetInstanceCount());

				Diagnostics::drawCalls++;
			}
			VAO::UnBind();

			FBO::UnBind(renderer);

			if (Settings::drawLines)
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

			postProcessingTechnique->BindNextFBO();
			directionalLightingTechnique->RenderFrame(renderData);
			
			glDisable(GL_DEPTH_TEST);
			glEnable(GL_BLEND);
			glBlendFunc(GL_ONE, GL_ONE);
			lightingTechnique->RenderFrame(renderData);
			glDisable(GL_BLEND);
			
			if (Settings::drawLines)
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

			glEnable(GL_DEPTH_TEST);
			forwardRenderingTechnique->RenderTransparent(renderData);

			if (Settings::drawLines)
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

			glCullFace(GL_BACK);
			//postProcessingTechnique->RenderFrame();

			FBO::UnBind(renderer);

			debugShader->Bind();
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, postProcessingTechnique->GetLastRenderTextureAndUnbindFBO()->GetID());
			debugShader->SetUniform(CT_HASH("tex"), 0);

			glBindVertexArray(quadVAO);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);

			Diagnostics::drawCalls++;

			if (Settings::drawDebug)
			{
				glm::ivec2 screenSize = renderer.GetScreenSize();
				glViewport(screenSize.x - 210, screenSize.y - 210, 200, 200);
				//glViewport(screenSize.x - screenSize.y, 0, screenSize.y, screenSize.y);
				glBindTexture(GL_TEXTURE_2D, shadowMapTechnique->shadowTexture->GetID());
				glBindVertexArray(quadVAO);
				glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
				glBindVertexArray(0);
				glViewport(0, 0, screenSize.x, screenSize.y);

				Diagnostics::drawCalls++;

				glViewport(screenSize.x - 110, screenSize.y - 320, 100, 100);
				glBindTexture(GL_TEXTURE_2D, colorBuffer->GetID());
				glBindVertexArray(quadVAO);
				glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
				glBindVertexArray(0);
				glViewport(0, 0, screenSize.x, screenSize.y);

				Diagnostics::drawCalls++;
			}
			OpenGLShader::UnBind();
		}

		DeferredRendering::~DeferredRendering()
		{
			delete postProcessingTechnique;
			delete lightingTechnique;
			delete shadowMapTechnique;
			delete cascadedShadowMapTechnique;
			delete directionalLightingTechnique;
		}
	}
}