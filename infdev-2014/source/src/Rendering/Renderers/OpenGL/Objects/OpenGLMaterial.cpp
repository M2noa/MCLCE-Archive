#include "./OpenGLMaterial.h"
#include "../OpenGLFactory.h"

namespace Esteem
{
	namespace OpenGL
	{
		OpenGLMaterial::OpenGLMaterial(uint id, std::string_view path, std::string_view name)
			: Material(path, name)
			, id(id)
		{ }
		
		OpenGLMaterial::~OpenGLMaterial()
		{
		}

		void OpenGLMaterial::Bind() const
		{
			int textureIndex = 0;
			uint shaderID = shader->GetShaderID();

			auto* it = buffers.data.data();
			auto* end = it + buffers.bufferSize;
			for (; it != end; ++it)
			{
				glActiveTexture(GL_TEXTURE0 + textureIndex);
				glBindTexture(GL_TEXTURE_BUFFER, it->buffer->GetID());
				shader->SetUniform(it->hash, textureIndex);
				++textureIndex;
				/*if(textureCount >= RenderingEngine::maxTextureUnits)
					break;*/
			}

			end = buffers.data.data() + buffers.texture1DSize;
			for (; it != end; ++it)
			{
				glActiveTexture(GL_TEXTURE0 + textureIndex);
				glBindTexture(GL_TEXTURE_1D, it->buffer->GetID());
				shader->SetUniform(it->hash, textureIndex);
				++textureIndex;
				/*if(textureCount >= RenderingEngine::maxTextureUnits)
					break;*/
			}

			end = buffers.data.data() + buffers.texture2DSize;
			for (; it != end; ++it)
			{
				glActiveTexture(GL_TEXTURE0 + textureIndex);
				glBindTexture(GL_TEXTURE_2D, it->buffer->GetID());
				shader->SetUniform(it->hash, textureIndex);
				++textureIndex;
				/*if(textureCount >= RenderingEngine::maxTextureUnits)
					break;*/
			}

			//shader->SetUniform(CT_HASH("materialID"), (int)id);

			for (auto it = activeUniformValues.begin(); it != activeUniformValues.end(); ++it)
				it->second.func(it->second.GetVariable()->index, &it->second.value[0]);
		}
	}
}