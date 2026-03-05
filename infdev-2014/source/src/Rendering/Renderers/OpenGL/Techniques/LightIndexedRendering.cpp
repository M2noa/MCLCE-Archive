#include "LightIndexedRendering.h"

#include "../OpenGLDebug.h"
#include "../OpenGLFactory.h"
#include "../OpenGLRenderCalls.h"

#include "General/Settings.h"
#include "Utils/Diagnostics.h"


namespace Esteem
{
	namespace OpenGL
	{
		LightIndexedRendering::LightIndexedRendering(OpenGLRenderer& renderer)
			: RenderTechnique(renderer)
			, shadowMapTechnique(renderer)
			, cascadedShadowMapTechnique(renderer, shadowMapTechnique)
		{
			Initialize();
		}

		void LightIndexedRendering::Initialize()
		{

			// default shadow map technique
			shadowMapTechnique.Initialize();
			shadowMapTechnique.SetDirectionalTechnique(&cascadedShadowMapTechnique);

			// initialize cascaded shadow maps techniques
			cascadedShadowMapTechnique.Initialize();

			// Shaders
			depthShader = renderer.GetOpenGLFactory().LoadOpenGLShader("DepthMap");
			depthShaderCutoff = renderer.GetOpenGLFactory().LoadOpenGLShader("DepthMap_cutoff");

			//lightIndexShader = renderer.GetOpenGLFactory().LoadShader("LightIndexed/LightIndices");

			// Light Indexed Frame buffer
			indexFrameBuffer = renderer.GetOpenGLFactory().LoadFBO("@DLightIndexeddFrameBuffer");

			//indexTexture = renderer.GetOpenGLFactory().LoadTexture2D("@LightIndexTexture", renderer->GetScreenSize(), GL_RGBA32I, GL_RGBA_INTEGER, GL_NEAREST, GL_CLAMP_TO_EDGE, false);
			//indexFrameBuffer->AttachRenderTexture(indexTexture, 0);
			indexFrameBuffer->Validate();

			// default lights
			clearColor[0] = 0x03;
			clearColor[1] = 0;
			clearColor[2] = 0;
			clearColor[3] = 0;
			
			cgc::strong_ptr<OpenGLShader> lightingShader = renderer.GetOpenGLFactory().LoadOpenGLShader("LightIndexed/LightIndices");
			//OpenGLShader* lightingShader = renderer.GetOpenGLFactory().LoadShader("DeferredShadowLight");
			std::unordered_map<std::string, cgc::strong_ptr<Texture2D>> textures { { "lightIndexTexture", indexTexture } };
			lightingTechnique = new Lighting(renderer, lightingShader, textures);
			lightingTechnique->Initialize();
		}

		void LightIndexedRendering::RenderFrame(const OpenGLRenderData& renderData)
		{
			cgc::raw_ptr<OpenGLShader> curShader = depthShader;
			bool cutoff = false;

			depthShader->Bind();
			glColorMask(false, false, false, false);

			const OpenGLRenderList& openGLRenderList = renderData.GetRenderList();
			const std::vector<OpenGLRenderObject*>* opaqueRenderList = &openGLRenderList[RenderObject::RenderOrder::OPAQUE_];

			for (uint i = 0; i < opaqueRenderList->size(); ++i)
			{
				OpenGLRenderObject* renderObject = (*opaqueRenderList)[i];
				cgc::raw_ptr<OpenGLMaterial> material = static_cast<OpenGLMaterial*>(renderObject->GetMaterial().ptr());
				cgc::raw_ptr<OpenGLShader> shader = static_cast<OpenGLShader*>(material->GetOpenGLShader().ptr());
				cgc::raw_ptr<VAO> vao = renderObject->GetVAO();

				if (cutoff != shader->IsCutOff())
				{
					cutoff = !cutoff;
					curShader = depthShaderCutoff;
					curShader->Bind();
				}

				if (cutoff)
				{
					static_assert("bind all buffer");
					/*glActiveTexture(GL_TEXTURE0);
					auto map = material->GetTextures();
					if(map[0].first == CT_HASH("tex"))
						glBindTexture(GL_TEXTURE_2D, map[0].second->GetID());
					depthShaderCutoff->SetUniform(CT_HASH("tex"), 0);*/
				}

				curShader->SetUniform(CT_HASH("ModelMatrix"), *renderObject->GetModelMatrix());
				vao->Bind();

				if (renderObject->boneMatrices != nullptr)
				{
					curShader->SetUniform(CT_HASH("animations"), 1);
					static_cast<OpenGLBoneMatrices*>(renderObject->boneMatrices.ptr())->GetUBO()->Bind(curShader);
				}
				else
					curShader->SetUniform(CT_HASH("animations"), 0);

				DrawRenderObject(*renderObject);
				//DrawElements(vao, renderObject->GetInstanceCount());

				Diagnostics::drawCalls++;
			}
			VAO::UnBind();
			glColorMask(true, true, true, true);
			OpenGLShader::UnBind();

			// fill shadow atlas
			shadowMapTechnique.RenderFrame(renderData);
			renderer.GetLightsUBO()->UpdateBuffer(renderer.GetOpenGLFactory().GetLightsData().data(), renderer.GetOpenGLFactory().GetLightsData().size() * sizeof(LightData));
			
			glDisable(GL_DEPTH_TEST);
			// light index texture
			indexFrameBuffer->Bind();
			glClearBufferiv(GL_COLOR, 0, clearColor);
			lightingTechnique->RenderFrame(renderData);
			FBO::UnBind(renderer);
			glEnable(GL_DEPTH_TEST);
			
			if (Settings::drawLines)
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

			RenderOpaque(renderData);
			RenderTransparent(renderData);

			if (Settings::drawLines)
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

			glDepthMask(GL_TRUE);
		}

		void LightIndexedRendering::RenderOpaque(const OpenGLRenderData& renderData)
		{

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

				renderer.ActivateTexture(7, CT_HASH("shadowAtlas"), shadowMapTechnique.shadowTexture.ptr(), shader);
				renderer.ActivateTexture(8, CT_HASH("csmShadowTexture"), cascadedShadowMapTechnique.GetCascadedShadowTexture().ptr(), shader);
				renderer.ActivateTexture(9, CT_HASH("lightIndexTexture"), indexTexture.ptr(), shader);

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
				//DrawElements(vao, renderObject->GetInstanceCount());

				Diagnostics::drawCalls++;
			}
			VAO::UnBind();
		}

		void LightIndexedRendering::RenderTransparent(const OpenGLRenderData& renderData)
		{
			//glDepthMask(GL_FALSE);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


			const OpenGLRenderList& openGLRenderList = renderData.GetRenderList();
			const std::vector<OpenGLRenderObject*>* transparentRenderList = &openGLRenderList[RenderObject::RenderOrder::TRANSLUCENT];

			for (uint i = 0; i < transparentRenderList->size(); ++i)
			{
				OpenGLRenderObject* renderObject = (*transparentRenderList)[i];
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
					reinterpret_cast<OpenGLBoneMatrices*>(renderObject->boneMatrices.ptr())->GetUBO()->Bind(shader);
				}
				else
					shader->SetUniform(CT_HASH("animations"), 0);

				DrawRenderObject(*renderObject);
				//DrawElements(vao, renderObject->GetInstanceCount());

				Diagnostics::drawCalls++;
			}
			VAO::UnBind();

			glDisable(GL_BLEND);
		}

		LightIndexedRendering::~LightIndexedRendering()
		{
			delete lightingTechnique;
		}
	}
}