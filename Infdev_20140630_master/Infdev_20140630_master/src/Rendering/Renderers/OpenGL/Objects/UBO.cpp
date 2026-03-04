#include "UBO.h"
#include "Utils/Data.h"
#include "../OpenGLDebug.h"
#include "./OpenGLShader.h"

namespace Esteem
{
	namespace OpenGL
	{
		UBO::UBO(uint id, uint bindingIndex, std::string_view bindingName, uint size, uint usageType)
			: id(id)
			, bindingHash(RT_HASH(bindingName))
			, bindingName(bindingName)
			, bindingIndex(bindingIndex)
			, size(size)
		{
		}
		
		UBO::~UBO()
		{
			glDeleteBuffers(1, &id);
		}

		void UBO::UpdateBuffer(const void* data, uint size)
		{
			//glBindBuffer(GL_UNIFORM_BUFFER, bindingID);
			glBindBuffer(GL_UNIFORM_BUFFER, id);
			if(size <= this->size)
				glBufferSubData(GL_UNIFORM_BUFFER, 0, size, data);
			else
			{
				glBufferData(GL_UNIFORM_BUFFER, size, data, GL_DYNAMIC_DRAW);
				this->size = size;
			}

			glBindBuffer(GL_UNIFORM_BUFFER, 0);
		}

		void UBO::Bind(cgc::raw_ptr<OpenGLShader> shader, std::string_view uniformBlockName, uint index)
		{
			UBO::Bind(shader, RT_HASH(uniformBlockName), index);
		}

		void UBO::Bind(cgc::raw_ptr<OpenGLShader> shader, hash_t bindingHash, uint index)
		{
			const auto& uniforms = shader->GetAllUniformVariables();
			auto found = uniforms.find(bindingHash);
			if (found != uniforms.end())
				glUniformBlockBinding(shader->GetShaderID(), found->second.index, index);
		}

		/*void UBO::Bind(cgc::raw_ptr<OpenGLShader> shader)
		{
			const auto& uniforms = shader->GetAllUniformVariables();
			auto found = uniforms->find(bindingHash);
			if (found != uniforms->end())
				glBindBufferBase(GL_UNIFORM_BUFFER, found->second.index, id);
		}*/

		void UBO::Bind(uint index)
		{
			glBindBufferBase(GL_UNIFORM_BUFFER, index, id);
		}

		void UBO::Bind()
		{
			glBindBufferBase(GL_UNIFORM_BUFFER, bindingIndex, id);
		}
	}
}