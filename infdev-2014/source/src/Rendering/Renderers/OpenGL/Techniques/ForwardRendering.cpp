#include "ForwardRendering.h"

#include "General/Settings.h"
#include "Utils/Diagnostics.h"
#include "../OpenGLFactory.h"

#include "../OpenGLRenderCalls.h"

namespace Esteem
{
	namespace OpenGL
	{
		ForwardRendering::ForwardRendering(OpenGLRenderer& renderer)
			: RenderTechnique(renderer)
			, skyBoxTechnique(renderer)
			, shadowMapTechnique(renderer)
			, cascadedShadowMapTechnique(renderer, shadowMapTechnique)
		{
			Initialize();
		}

		void ForwardRendering::Initialize()
		{
			//renderToTargetShader = factory->LoadOpenGLShader("RenderToTarget");
			renderer.GetFactory().HandleQueue();

			//renderToTargetShader->Bind();
			//renderToTargetShader->SetUniform(CT_HASH("tex"), 0);
			//OpenGLShader::UnBind();


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

			uint vertexIndex = 0;
			glEnableVertexAttribArray(vertexIndex);
			glVertexAttribPointer(vertexIndex, 3, GL_FLOAT, false, sizeof(vertexData) / 4, 0);

			uint uvIndex = 4;
			glEnableVertexAttribArray(uvIndex);
			glVertexAttribPointer(uvIndex, 2, GL_FLOAT, true, sizeof(vertexData) / 4, (void*)12);

			static const uint indices[] = { 0, 1, 2, 1, 3, 2 };

			uint ebo;
			glGenBuffers(1, &ebo);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

			glBindVertexArray(0);

			//depthFrameBuffer = factory->LoadFBO("@ScreenFrameBuffer");
			//colorTexture = factory->LoadTexture2D("@ScreenColorBuffer", renderer->GetScreenSize(), GL_RGBA16, GL_RGBA, GL_LINEAR, GL_CLAMP_TO_EDGE, false);
			//cgc::strong_ptr<Texture2D> depthTexture = factory->LoadTexture2D("@ScreenDepthBuffer", renderer->GetScreenSize(), GL_DEPTH_COMPONENT24, GL_DEPTH_COMPONENT, GL_NEAREST, GL_CLAMP_TO_EDGE, false);
			//depthFrameBuffer->AttachRenderTexture(colorTexture, 0);
			//depthFrameBuffer->AttachDepthTexture(depthTexture);
			//depthFrameBuffer->Validate();

			// default shadow map technique
			shadowMapTechnique.Initialize();
			cascadedShadowMapTechnique.Initialize();

			shadowMapTechnique.SetDirectionalTechnique(&cascadedShadowMapTechnique);

			// Sky box
			skyBoxTechnique.Initialize();
		}

		void ForwardRendering::RenderFrame(const OpenGLRenderData& renderData)
		{
			//glEnable(GL_DEPTH_TEST);

			// fill shadow atlas
			shadowMapTechnique.RenderFrame(renderData);
			if(!renderer.GetOpenGLFactory().GetLightsData().empty())
				renderer.GetLightsUBO()->UpdateBuffer(renderer.GetOpenGLFactory().GetLightsData().data(), renderer.GetOpenGLFactory().GetLightsData().size() * sizeof(LightData));
			
			//depthFrameBuffer->Bind();
			glClearColor(0, 0, 0, 1);
			glClear(GL_COLOR_BUFFER_BIT |  GL_DEPTH_BUFFER_BIT);
			
			if (Settings::drawLines)
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			
			// opaque world object rendering
			RenderOpaque(renderData);

			if (Settings::drawLines)
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

			// skybox
			skyBoxTechnique.RenderFrame(renderData);

			if (Settings::drawLines)
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

			// transparent world object rendering
			RenderTransparent(renderData);


			if (Settings::drawLines)
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			
			//FBO::UnBind(renderer);

			//VAO::UnBind();

			/*renderToTargetShader->Bind();
			glDepthMask(GL_FALSE);
			glDisable(GL_DEPTH_TEST);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, colorTexture->GetID());

			glBindVertexArray(quadVAO);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

			Diagnostics::drawCalls++;*/

			// debug image
			/*glm::ivec2 screenSize = renderer->GetScreenSize();
			glViewport(screenSize.x - 310, screenSize.y - 310, 300, 300);
			glBindTexture(GL_TEXTURE_2D, shadowMapTechnique->shadowTexture->GetID());
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);*/
			
			OpenGLShader::UnBind();
		}

		void ForwardRendering::RenderOpaque(const OpenGLRenderData& renderData)
		{
			const OpenGLRenderList& openGLRenderList = renderData.GetRenderList();

			for (const auto* renderObject : openGLRenderList[RenderObject::RenderOrder::OPAQUE_])
			{
				cgc::raw_ptr<OpenGLMaterial> material = static_cast<OpenGLMaterial*>(renderObject->GetMaterial().ptr());
				cgc::raw_ptr<OpenGLShader> shader = static_cast<OpenGLShader*>(material->GetOpenGLShader().ptr());

				OpenGLBoneMatrices* boneMatrices = reinterpret_cast<OpenGLBoneMatrices*>(renderObject->boneMatrices.ptr());
				cgc::raw_ptr<VAO> vao = renderObject->GetVAO();

				shader->Bind();
				shader->SetUniform(CT_HASH("ModelMatrix"), *renderObject->GetModelMatrix());

				renderer.ActivateTexture(7, CT_HASH("shadowAtlas"), shadowMapTechnique.shadowTexture.ptr(), shader);
				renderer.ActivateTexture(8, CT_HASH("csmShadowTexture"), cascadedShadowMapTechnique.GetCascadedShadowTexture().ptr(), shader);

				material->Bind();
				vao->Bind();

				if (boneMatrices != nullptr)
				{
					shader->SetUniform(CT_HASH("animations"), 1);
					boneMatrices->GetUBO()->Bind();
				}
				else
					shader->SetUniform(CT_HASH("animations"), 0);

				DrawRenderObject(*renderObject);
				//DrawElements(vao, renderObject->GetInstanceCount());

				Diagnostics::drawCalls++;
			}

			const auto& opaqueDSRenderList = openGLRenderList[RenderObject::RenderOrder::OPAQUE_DOUBLE_SIDED];
			if (!opaqueDSRenderList.empty())
			{
				glDisable(GL_CULL_FACE);
				for (const auto* renderObject : opaqueDSRenderList)
				{
					cgc::raw_ptr<OpenGLMaterial> material = static_cast<OpenGLMaterial*>(renderObject->GetMaterial().ptr());
					cgc::raw_ptr<OpenGLShader> shader = static_cast<OpenGLShader*>(material->GetOpenGLShader().ptr());

					OpenGLBoneMatrices* boneMatrices = static_cast<OpenGLBoneMatrices*>(renderObject->boneMatrices.ptr());
					cgc::raw_ptr<VAO> vao = renderObject->GetVAO();

					shader->Bind();
					shader->SetUniform(CT_HASH("ModelMatrix"), *renderObject->GetModelMatrix());

					//renderer->ActivateTexture(7, CT_HASH("shadowAtlas"), shadowMapTechnique->shadowTexture.ptr(), shader.ptr());
					//renderer->ActivateTexture(8, CT_HASH("csmShadowTexture"), cascadedShadowMapTechnique->GetCascadedShadowTexture().ptr(), shader.ptr());

					material->Bind();
					renderObject->BindCustomBuffers(shader);
					vao->Bind();

					if (boneMatrices != nullptr)
					{
						shader->SetUniform(CT_HASH("animations"), 1);
						boneMatrices->GetUBO()->Bind();
					}
					else
						shader->SetUniform(CT_HASH("animations"), 0);

					DrawRenderObject(*renderObject);
					//DrawElements(vao, renderObject->GetInstanceCount());

					Diagnostics::drawCalls++;
				}
				glEnable(GL_CULL_FACE);
			}
		}

		void ForwardRendering::RenderTransparent(const OpenGLRenderData& renderData)
		{
			glDepthMask(GL_FALSE);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			
			const OpenGLRenderList& openGLRenderList = renderData.GetRenderList();
			const std::vector<OpenGLRenderObject*>& transparentRenderList = openGLRenderList[RenderObject::RenderOrder::TRANSLUCENT];

			for (uint i = 0; i < transparentRenderList.size(); ++i)
			{
				OpenGLRenderObject* renderObject = transparentRenderList[i];
				cgc::raw_ptr<OpenGLMaterial> material = static_cast<OpenGLMaterial*>(renderObject->GetMaterial().ptr());
				cgc::raw_ptr<OpenGLShader> shader = static_cast<OpenGLShader*>(material->GetOpenGLShader().ptr());

				OpenGLBoneMatrices* boneMatrices = reinterpret_cast<OpenGLBoneMatrices*>(renderObject->boneMatrices.ptr());
				cgc::raw_ptr<VAO> vao = renderObject->GetVAO();

				shader->Bind();
				shader->SetUniform(CT_HASH("ModelMatrix"), *renderObject->GetModelMatrix());

				//renderer->ActivateTexture(7, CT_HASH("shadowAtlas"), shadowMapTechnique->shadowTexture.ptr(), shader.ptr());
				//renderer->ActivateTexture(8, CT_HASH("csmShadowTexture"), cascadedShadowMapTechnique->GetCascadedShadowTexture().ptr(), shader.ptr());

				material->Bind();
				vao->Bind();

				if (boneMatrices != nullptr)
				{
					shader->SetUniform(CT_HASH("animations"), 1);
					boneMatrices->GetUBO()->Bind();
				}
				else
					shader->SetUniform(CT_HASH("animations"), 0);

				DrawRenderObject(*renderObject);
				//DrawElements(vao, renderObject->GetInstanceCount());

				Diagnostics::drawCalls++;
			}
			VAO::UnBind();

			glDisable(GL_BLEND);
			glDepthMask(GL_TRUE);
		}
	}
}