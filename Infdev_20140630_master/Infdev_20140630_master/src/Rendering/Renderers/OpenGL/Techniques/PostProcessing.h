#pragma once

#include <vector>
#include "../OpenGLRenderer.h"
#include "../OpenGLRenderData.h"
#include "../Objects/OpenGLRenderObject.h"
#include "../../../IRenderTechnique.h"

namespace Esteem
{
	namespace OpenGL
	{
		class FBO;

		class PostProcessing : public RenderTechnique <OpenGLRenderData, OpenGLRenderer>
		{
		private:
			cgc::strong_ptr<FBO> renderBufferA;
			cgc::strong_ptr<FBO> renderBufferB;
			cgc::strong_ptr<FBO> renderBufferCustom;
			cgc::strong_ptr<FBO> currentBuffer;

			cgc::strong_ptr<Texture2D> renderTextureA;
			cgc::strong_ptr<Texture2D> renderTextureB;

			OpenGLShader* postProcessShader;
			OpenGLMaterial* postProcessMaterial;
			uint indicesCount;

			cgc::strong_ptr<Texture2D> colorTexture;
			cgc::strong_ptr<Texture2D> normalsTexture;
			cgc::strong_ptr<Texture2D> depthTexture;

			uint quadVAO;

		public:
			PostProcessing(OpenGLRenderer& renderer, const cgc::strong_ptr<Texture2D>& colorTexture, const cgc::strong_ptr<Texture2D>& normalsTexture, const cgc::strong_ptr<Texture2D>& depthTexture);
			virtual ~PostProcessing();

			virtual void RenderFrame(const OpenGLRenderData& renderData);
			void PostProcess(cgc::raw_ptr<FBO> fbo, OpenGLMaterial* material);
			void PostProcess(const cgc::strong_ptr<Texture3D>& renderTarget, OpenGLMaterial* material);

			virtual void Initialize();

			void BindNextFBO();
			const cgc::strong_ptr<Texture2D>& GetLastRenderTextureAndUnbindFBO();
		};
	}
}