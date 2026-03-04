#pragma once

#include <vector>
#include <cppu/cgc/pointers.h>

#include "../../../IRenderTechnique.h"

namespace Esteem
{
	class TextureCube;

	namespace OpenGL
	{
		class OpenGLRenderData;
		class OpenGLRenderer;
		class OpenGLShader;
		class OpenGLFactory;
		class OpenGLMaterial;
		class VAO;

		class SkyBox : public RenderTechnique <OpenGLRenderData, OpenGLRenderer>
		{
		private:
			cgc::strong_ptr<VAO> vao;

			cgc::strong_ptr<TextureCube> skyBoxTexture;
			cgc::strong_ptr<OpenGLShader> skyBoxShader;
			OpenGLMaterial* skyBoxMaterial;

		public:
			SkyBox(OpenGLRenderer& renderer);
			virtual ~SkyBox();

			virtual void RenderFrame(const OpenGLRenderData& renderData);

			virtual void Initialize();
		};
	}
}