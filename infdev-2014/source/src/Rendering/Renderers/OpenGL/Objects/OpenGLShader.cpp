#include "OpenGLShader.h"
#include "Utils/Debug.h"
#include "../OpenGLFactory.h"

namespace Esteem
{
	namespace OpenGL
	{

		OpenGLShader* OpenGLShader::bindShader = nullptr;

		OpenGLShader::OpenGLShader(uint id, const std::string& uniqueName, const std::vector<cgc::strong_ptr<OpenGLSubShader>>& subShaders)
			: id(id)
			, uniqueName(uniqueName)
			, shaders(subShaders)
			, cutoff(false)
			, initialized(false)
		{
			// check if we are using cutoff shaders
			for (uint i = 0; i < subShaders.size(); ++i)
			{
				if (subShaders[i]->GetCutoff() > 0.001f)
				{
					cutoff = true;
					break;
				}
			}
		}

		OpenGLShader::OpenGLShader(uint id, const std::string& uniqueName)
			: id(id)
			, uniqueName(uniqueName)
			, shaders()
			, cutoff(false)
			, initialized(false)
		{ }
		
		std::string OpenGLShader::UniqueName(const std::vector<std::string>& subShaders)
		{
			std::string shaderName = "";
			for (uint i = 0; i < subShaders.size(); i++)
				shaderName += std::to_string(std::hash<std::string>()(subShaders[i])) + (i < subShaders.size() - 1 ? ";" : "");

			return shaderName;
		}

		OpenGLShader::Variable* OpenGLShader::GetAttributeVariable(const std::string& attribName)
		{
			auto foundAttribute = attributes.find(attribName);
			if (foundAttribute != attributes.end())
				return &foundAttribute->second;

			return nullptr;
		}

		OpenGLShader::Variable* OpenGLShader::GetUniformVariable(std::size_t attribName)
		{
			auto foundAttribute = uniforms.find(attribName);
			if (foundAttribute != uniforms.end())
				return &foundAttribute->second;

			return nullptr;
		}

		OpenGLShader::~OpenGLShader()
		{
			for (uint i = 0; i < shaders.size(); ++i)
				glDetachShader(id, shaders[i]->GetID());

			glDeleteProgram(id);
		}

		void OpenGLShader::LoadAllAttributesAndUniforms()
		{
			GLint numActiveAttribs = 0;
			GLint numActiveUniforms = 0;
			GLint numActiveUniformBlocks = 0;
			glGetProgramiv(id, GL_ACTIVE_ATTRIBUTES, &numActiveAttribs);
			glGetProgramiv(id, GL_ACTIVE_UNIFORMS, &numActiveUniforms);
			glGetProgramiv(id, GL_ACTIVE_UNIFORM_BLOCKS, &numActiveUniformBlocks);

			// Get attributes
			std::array<char, 256> nameData;
			for (int attrib = 0; attrib < numActiveAttribs; ++attrib)
			{
				GLint arraySize = 0;
				GLenum type = 0;
				GLsizei nameLength = 0;
				glGetActiveAttrib(id, attrib, nameData.size(), &nameLength, &arraySize, &type, nameData.data());

				std::string_view name = std::string_view(nameData.data(), nameLength);
				name = name.substr(0, name.find("["));
				int index = glGetAttribLocation(id, name.data());
				if (index != -1)
				{
					Variable var;
					var.index = (uint)index;
					var.type = type;
					var.size = arraySize;
					var.name = name;
					attributes[var.name] = var;
				}
			}

			// Get uniforms
			for (int unif = 0; unif < numActiveUniforms; ++unif)
			{
				GLint arraySize = 0;
				GLenum type = 0;
				GLsizei nameLength = 0;
				glGetActiveUniform(id, unif, nameData.size(), &nameLength, &arraySize, &type, nameData.data());
				
				std::string_view name = std::string_view(nameData.data(), nameLength);
				name = name.substr(0, name.find("["));
				int index = glGetUniformLocation(id, name.data());
				if (index != -1)
				{
					Variable var;
					var.index = (uint)index;
					var.type = type;
					var.size = arraySize;
					var.name = name;
					uniforms[RT_HASH(var.name)] = var;
				}
			}

			// Get uniform blocks
			for (int unif = 0; unif < numActiveUniformBlocks; ++unif)
			{
				GLint nameLength = 0;
				glGetActiveUniformBlockiv(id, unif, GL_UNIFORM_BLOCK_NAME_LENGTH, &nameLength);

				glGetActiveUniformBlockName(id, unif, nameLength, NULL, nameData.data());
				int index = glGetUniformBlockIndex(id, nameData.data());
				if (index != -1)
				{
					GLint blockSize = 0;
					glGetActiveUniformBlockiv(id, unif, GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS, &blockSize);

					Variable var;
					var.index = (uint)index;
					var.type = GL_UNIFORM_BLOCK_BINDING;
					var.size = blockSize;
					var.name = std::string_view(nameData.data(), nameLength - 1);
					uniforms[RT_HASH(var.name)] = var;
				}
			}
		}

		void OpenGLShader::Bind() const
		{
			if (OpenGLShader::bindShader != this)
			{
				glUseProgram(id);
				OpenGLShader::bindShader = const_cast<OpenGLShader*>(this);
			}
		}

		void OpenGLShader::Bind(const glm::mat4& CameraMVP) const
		{
			if (OpenGLShader::bindShader != this)
			{
				glUseProgram(id);
				OpenGLShader::bindShader = const_cast<OpenGLShader*>(this);
			}

			SetUniform(CT_HASH("CameraMVP"), CameraMVP);
		}

		void OpenGLShader::UnBind()
		{
			glUseProgram(0);
			OpenGLShader::bindShader = nullptr;
		}

	#pragma region SetUniform() single value
		void OpenGLShader::SetUniform(std::size_t variable, float x) const
		{
			auto found = uniforms.find(variable);
			if (found != uniforms.end())
				glUniform1f(found->second.index, x);
		}

		void OpenGLShader::SetUniform(std::size_t variable, float x, float y) const
		{
			auto found = uniforms.find(variable);
			if (found != uniforms.end())
				glUniform2f(found->second.index, x, y);
		}

		void OpenGLShader::SetUniform(std::size_t variable, float x, float y, float z) const
		{
			auto found = uniforms.find(variable);
			if (found != uniforms.end())
				glUniform3f(found->second.index, x, y, z);
		}

		void OpenGLShader::SetUniform(std::size_t variable, float x, float y, float z, float w) const
		{
			auto found = uniforms.find(variable);
			if (found != uniforms.end())
				glUniform4f(found->second.index, x, y, z, w);
		}

		void OpenGLShader::SetUniform(std::size_t variable, int x) const
		{
			auto found = uniforms.find(variable);
			if (found != uniforms.end())
				glUniform1i(found->second.index, x);
		}

		void OpenGLShader::SetUniform(std::size_t variable, int x, int y) const
		{
			auto found = uniforms.find(variable);
			if (found != uniforms.end())
				glUniform2i(found->second.index, x, y);
		}

		void OpenGLShader::SetUniform(std::size_t variable, int x, int y, int z) const
		{
			auto found = uniforms.find(variable);
			if (found != uniforms.end())
				glUniform3i(found->second.index, x, y, z);
		}

		void OpenGLShader::SetUniform(std::size_t variable, int x, int y, int z, int w) const
		{
			auto found = uniforms.find(variable);
			if (found != uniforms.end())
				glUniform4i(found->second.index, x, y, z, w);
		}

		void OpenGLShader::SetUniform(std::size_t variable, const glm::mat4& matrix, bool transpose) const
		{
			auto found = uniforms.find(variable);
			if (found != uniforms.end())
				glUniformMatrix4fv(found->second.index, 1, transpose, &matrix[0][0]);
		}

		void OpenGLShader::SetUniform(std::size_t variable, const glm::mat3& matrix, bool transpose) const
		{
			auto found = uniforms.find(variable);
			if (found != uniforms.end())
				glUniformMatrix3fv(found->second.index, 1, transpose, &matrix[0][0]);
		}

		void OpenGLShader::SetUniform(std::size_t variable, const std::vector<glm::mat4>& matrices, bool transpose) const
		{
			auto found = uniforms.find(variable);
			if (found != uniforms.end())
				glUniformMatrix4fv(found->second.index, matrices.size(), transpose, &matrices[0][0][0]);
		}
	#pragma endregion

	#pragma region SetUniform() array values
		void OpenGLShader::SetUniform(std::size_t variable, float x, uint index) const
		{
			auto found = uniforms.find(variable);
			if (found != uniforms.end() && index < found->second.size)
				glUniform1f(found->second.index + index, x);
		}

		void OpenGLShader::SetUniform(std::size_t variable, float x, float y, uint index) const
		{
			auto found = uniforms.find(variable);
			if (found != uniforms.end() && index < found->second.size)
				glUniform2f(found->second.index + index, x, y);
		}

		void OpenGLShader::SetUniform(std::size_t variable, float x, float y, float z, uint index) const
		{
			auto found = uniforms.find(variable);
			if (found != uniforms.end() && index < found->second.size)
				glUniform3f(found->second.index + index, x, y, z);
		}

		void OpenGLShader::SetUniform(std::size_t variable, float x, float y, float z, float w, uint index) const
		{
			auto found = uniforms.find(variable);
			if (found != uniforms.end() && index < found->second.size)
				glUniform4f(found->second.index + index, x, y, z, w);
		}

		void OpenGLShader::SetUniform(std::size_t variable, int x, uint index) const
		{
			auto found = uniforms.find(variable);
			if (found != uniforms.end() && index < found->second.size)
				glUniform1i(found->second.index + index, x);
		}

		void OpenGLShader::SetUniform(std::size_t variable, int x, int y, uint index) const
		{
			auto found = uniforms.find(variable);
			if (found != uniforms.end() && index < found->second.size)
				glUniform2i(found->second.index + index, x, y);
		}

		void OpenGLShader::SetUniform(std::size_t variable, int x, int y, int z, uint index) const
		{
			auto found = uniforms.find(variable);
			if (found != uniforms.end() && index < found->second.size)
				glUniform3i(found->second.index + index, x, y, z);
		}

		void OpenGLShader::SetUniform(std::size_t variable, int x, int y, int z, int w, uint index) const
		{
			auto found = uniforms.find(variable);
			if (found != uniforms.end() && index < found->second.size)
				glUniform4i(found->second.index + index, x, y, z, w);
		}

		void OpenGLShader::SetUniform(std::size_t variable, const glm::mat4& matrix, uint index, bool transpose) const
		{
			auto found = uniforms.find(variable);
			if (found != uniforms.end() && index < found->second.size)
				glUniformMatrix4fv(found->second.index + index, 1, transpose, &matrix[0][0]);
		}

		void OpenGLShader::SetUniform(std::size_t variable, const glm::mat3& matrix, uint index, bool transpose) const
		{
			auto found = uniforms.find(variable);
			if (found != uniforms.end() && index < found->second.size)
				glUniformMatrix3fv(found->second.index + index, 1, transpose, &matrix[0][0]);
		}

		void OpenGLShader::SetUniform(std::size_t variable, const std::vector<glm::mat4>& matrices, uint index, bool transpose) const
		{
			auto found = uniforms.find(variable);
			if (found != uniforms.end() && index < found->second.size)
				glUniformMatrix4fv(found->second.index + index, matrices.size(), transpose, &matrices[0][0][0]);
		}
	#pragma endregion
	}
}