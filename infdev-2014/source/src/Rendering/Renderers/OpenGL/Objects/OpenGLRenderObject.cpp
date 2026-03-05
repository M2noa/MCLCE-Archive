#include "OpenGLRenderObject.h"

#undef TRANSPARENT

namespace Esteem
{
	namespace OpenGL
	{
		OpenGLRenderObject::OpenGLRenderObject(uint id, const cgc::strong_ptr<AbstractMesh>& mesh, const cgc::strong_ptr<OpenGLMaterial>& material)
			: RenderObject(material, mesh)
			, id(id)
			, renderMode(GL_TRIANGLES)
		{
			isTranslucent = material->IsTranslucent();

			if(material->IsTranslucent())
				SetRenderOrder(RenderOrder::TRANSLUCENT);
			else if(material->IsDoubleSided())
				SetRenderOrder(RenderOrder::OPAQUE_DOUBLE_SIDED);
		}

		void OpenGLRenderObject::BindCustomBuffers(const cgc::raw_ptr<OpenGLShader>& shader) const
		{
			if (customBuffers)
			{
				int textureIndex = 0;
				uint shaderID = shader->GetShaderID();

				auto* it = customBuffers->data.data();
				auto* end = it + customBuffers->bufferSize;
				for (; it != end; ++it)
				{
					glActiveTexture(GL_TEXTURE0 + textureIndex);
					glBindTexture(GL_TEXTURE_BUFFER, it->buffer->GetID());
					shader->SetUniform(it->hash, textureIndex);
					++textureIndex;
					/*if(textureCount >= RenderingEngine::maxTextureUnits)
						break;*/
				}

				end = customBuffers->data.data() + customBuffers->texture1DSize;
				for (; it != end; ++it)
				{
					glActiveTexture(GL_TEXTURE0 + textureIndex);
					glBindTexture(GL_TEXTURE_1D, it->buffer->GetID());
					shader->SetUniform(it->hash, textureIndex);
					++textureIndex;
					/*if(textureCount >= RenderingEngine::maxTextureUnits)
						break;*/
				}

				end = customBuffers->data.data() + customBuffers->texture2DSize;
				for (; it != end; ++it)
				{
					glActiveTexture(GL_TEXTURE0 + textureIndex);
					glBindTexture(GL_TEXTURE_2D, it->buffer->GetID());
					shader->SetUniform(it->hash, textureIndex);
					++textureIndex;
					/*if(textureCount >= RenderingEngine::maxTextureUnits)
						break;*/
				}
			}
		}
	}
}