#include "PostProcessing.h"

#include "../OpenGLDebug.h"
#include "../OpenGLFactory.h"

#include "Math/Math.h"

namespace Esteem
{
	namespace OpenGL
	{
		PostProcessing::PostProcessing(OpenGLRenderer& renderer, const cgc::strong_ptr<Texture2D>& colorTexture, const cgc::strong_ptr<Texture2D>& normalsTexture, const cgc::strong_ptr<Texture2D>& depthTexture)
			: RenderTechnique(renderer)
			, colorTexture(colorTexture)
			, normalsTexture(normalsTexture)
			, depthTexture(depthTexture)
		{

		}

		void PostProcessing::Initialize()
		{
			//postProcessShader = renderer.GetOpenGLFactory().LoadShader("POST_PROCESS/ssao");
			//postProcessMaterial = renderer.GetOpenGLFactory().LoadMaterial("POST_PROCESS/pp_ssao");

			renderBufferA = renderer.GetOpenGLFactory().LoadFBO("@FBOPostProcessA");
			renderBufferB = renderer.GetOpenGLFactory().LoadFBO("@FBOPostProcessB");
			renderBufferCustom = renderer.GetOpenGLFactory().LoadFBO("@FBOPostProcessCustom");

			//renderTextureA = renderer.GetOpenGLFactory().LoadTexture2D("@PostProcessTextureA", renderer->GetScreenSize(), GL_RGB, GL_RGB, GL_NEAREST, GL_CLAMP_TO_EDGE, false);
			//renderTextureB = renderer.GetOpenGLFactory().LoadTexture2D("@PostProcessTextureB", renderer->GetScreenSize(), GL_RGB, GL_RGB, GL_NEAREST, GL_CLAMP_TO_EDGE, false);

			//renderBufferA->AttachRenderTexture(renderTextureA, 0);
			//renderBufferA->AttachDepthTexture(depthTexture);
			//renderBufferA->Validate();

			//renderBufferB->AttachRenderTexture(renderTextureB, 0);
			//renderBufferB->AttachDepthTexture(depthTexture);
			//renderBufferB->Validate();

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

			uint vertexIndex = 0; // postProcessShader->GetAttributeVariable("vertex")->index;
			glEnableVertexAttribArray(vertexIndex);
			glVertexAttribPointer(vertexIndex, 3, GL_FLOAT, false, sizeof(vertexData) / 4, 0);

			/*auto uvVariable = postProcessShader->GetAttributeVariable("uv");
			if (uvVariable != nullptr)
			{
				glEnableVertexAttribArray(uvVariable->index);
				glVertexAttribPointer(uvVariable->index, 2, GL_FLOAT, true, sizeof(vertexData) / 4, (void*)12);
			}*/

			static const uint indices[] = { 0, 1, 2, 1, 3, 2 };

			uint ebo;
			glGenBuffers(1, &ebo);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

			glBindVertexArray(0);
		}

		PostProcessing::~PostProcessing()
		{
			glDeleteBuffers(1, &quadVAO);
		}


		void PostProcessing::RenderFrame(const OpenGLRenderData& renderData)
		{
			BindNextFBO();

			// Require all post processes of current frame

			glDepthMask(GL_FALSE);

			glBindVertexArray(quadVAO);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);

			glDepthMask(GL_TRUE);
		}

		void PostProcessing::PostProcess(const cgc::raw_ptr<FBO> fbo, OpenGLMaterial* material)
		{
			fbo->Bind();
			material->Bind();

			glDepthMask(GL_FALSE);

			glBindVertexArray(quadVAO);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);

			glDepthMask(GL_TRUE);
		}

		void PostProcessing::PostProcess(const cgc::strong_ptr<Texture3D>& renderTarget, OpenGLMaterial* material)
		{
			renderBufferCustom->AttachRenderTexture(renderTarget, 0);
			renderBufferCustom->Validate();
			renderBufferCustom->Bind();
			material->Bind();

			glDepthMask(GL_FALSE);

			glBindVertexArray(quadVAO);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);

			glDepthMask(GL_TRUE);
		}

		void PostProcessing::BindNextFBO()
		{
			currentBuffer = currentBuffer == renderBufferA ? renderBufferB : renderBufferA;
			currentBuffer->Bind();
			glClearColor(0, 0, 0, 1);
			glClear(GL_COLOR_BUFFER_BIT);
		}

		const cgc::strong_ptr<Texture2D>& PostProcessing::GetLastRenderTextureAndUnbindFBO()
		{
			const cgc::strong_ptr<Texture2D>& currentTexture = currentBuffer == renderBufferA ? renderTextureA : renderTextureB;

			// unbind and reset buffer state
			currentBuffer = cgc::strong_ptr<FBO>();
			FBO::UnBind(renderer);

			return currentTexture;
		}
	}
}