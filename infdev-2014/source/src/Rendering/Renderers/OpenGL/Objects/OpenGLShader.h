#pragma once

#include "../../../Objects/IShader.h"

#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <unordered_map>
#include <GL/glew.h>
#include <SFML/OpenGL.hpp>
#include <cppu/cgc/pointers.h>
#include <cppu/cgc/constructor.h>

#include "./OpenGLSubShader.h"

#include "Utils/Hash.h"

namespace Esteem
{
	namespace OpenGL
	{
		/// \brief OpenGL Shader
		///
		/// for simplicity "Shader" means a program with different shaders (fragment, vertex, etc.)
		/// use of the name "program" makes sense only for videocard programming but can be confusing in a program like a game engine
		class OpenGLShader : public IShader
		{
		friend class OpenGLFactory;
		friend class cgc::constructor;

		private:
			struct Variable
			{
				uint index;
				uint size;
				GLenum type;
				std::string name;
			};

		public:
			struct UniformValue
			{
			friend class OpenGLFactory;
			private:
				Variable* variable;
				GLenum type;
				bool active;

			public:				
				std::vector<byte> value;

				void(*func)(uint, const void*);

				inline bool isActive() const { return active; }
				inline const Variable* GetVariable() const { return variable; }
			};

		private:
			uint id;
			bool cutoff;
			bool initialized;

			/// \brief OpenGL shader id
			std::string uniqueName;

			std::vector<cgc::strong_ptr<OpenGLSubShader>> shaders;
			std::unordered_map<std::string, Variable> attributes;
			std::unordered_map<hash_t, Variable> uniforms;

			static OpenGLShader* bindShader;

			OpenGLShader(uint id, const std::string& uniqueName, const std::vector<cgc::strong_ptr<OpenGLSubShader>>& subShaders);
			OpenGLShader(uint id, const std::string& uniqueName);

			void LoadAllAttributesAndUniforms();
			
		public:
			OpenGLShader() = default;
			~OpenGLShader();

			inline const std::unordered_map<std::string, Variable>& GetAllAttributeVariables() const { return attributes; }
			inline const std::unordered_map<hash_t, Variable>& GetAllUniformVariables() const { return uniforms; }
			
			Variable* GetAttributeVariable(const std::string& attribName);
			Variable* GetUniformVariable(std::size_t attribName);

			void Bind() const;
			void Bind(const glm::mat4& CameraMVP) const;

			static void UnBind();

			inline const std::string& GetUniqueName() const { return uniqueName; }
			inline uint GetShaderID() const { return id; }

			inline bool IsCutOff() const { return cutoff; }
			inline bool IsInitialized() const { return initialized; }

			static std::string UniqueName(const std::vector<std::string>& subShaders);

		#pragma region SetUniform() array values
			void SetUniform(std::size_t variable, float x, uint index) const;
			void SetUniform(std::size_t variable, float x, float y, uint index) const;
			void SetUniform(std::size_t variable, float x, float y, float z, uint index) const;
			void SetUniform(std::size_t variable, float x, float y, float z, float w, uint index) const;

			void SetUniform(std::size_t variable, int x, uint index) const;
			void SetUniform(std::size_t variable, int x, int y, uint index) const;
			void SetUniform(std::size_t variable, int x, int y, int z, uint index) const;
			void SetUniform(std::size_t variable, int x, int y, int z, int w, uint index) const;

			void SetUniform(std::size_t variable, const glm::mat4& matrix, uint index, bool transpose = false) const;
			void SetUniform(std::size_t variable, const glm::mat3& matrix, uint index, bool transpose = false) const;
			void SetUniform(std::size_t variable, const std::vector<glm::mat4>& matrices, uint index, bool transpose = false) const;
		#pragma endregion

		#pragma region SetUniform() single value
			#pragma region Direct
			inline static void SetUniform1f(uint index, const void* x)
			{
				glUniform1f(index, *reinterpret_cast<const float*>(x));
			}

			inline static void SetUniform2f(uint index, const void* x)
			{
				const float* vec = reinterpret_cast<const float*>(x);
				glUniform2f(index, vec[0], vec[1]);
			}

			inline static void SetUniform3f(uint index, const void* x)
			{
				const float* vec = reinterpret_cast<const float*>(x);
				glUniform3f(index, vec[0], vec[1], vec[2]);
			}

			inline static void SetUniform4f(uint index, const void* x)
			{
				const float* vec = reinterpret_cast<const float*>(x);
				glUniform4f(index, vec[0], vec[1], vec[2], vec[3]);
			}
			
			inline static void SetUniform1i(uint index, const void* x)
			{
				glUniform1i(index, *reinterpret_cast<const int*>(x));
			}

			inline static void SetUniform2i(uint index, const void* x)
			{
				const int* vec = reinterpret_cast<const int*>(x);
				glUniform2i(index, vec[0], vec[1]);
			}

			inline static void SetUniform3i(uint index, const void* x)
			{
				const int* vec = reinterpret_cast<const int*>(x);
				glUniform3i(index, vec[0], vec[1], vec[2]);
			}

			inline static void SetUniform4i(uint index, const void* x)
			{
				const int* vec = reinterpret_cast<const int*>(x);
				glUniform4i(index, vec[0], vec[1], vec[2], vec[3]);
			}
			
			inline static void SetUniform1ui(uint index, const void* x)
			{
				glUniform1ui(index, *reinterpret_cast<const uint*>(x));
			}

			inline static void SetUniform2ui(uint index, const void* x)
			{
				const uint* vec = reinterpret_cast<const uint*>(x);
				glUniform2ui(index, vec[0], vec[1]);
			}

			inline static void SetUniform3ui(uint index, const void* x)
			{
				const uint* vec = reinterpret_cast<const uint*>(x);
				glUniform3ui(index, vec[0], vec[1], vec[2]);
			}

			inline static void SetUniform4ui(uint index, const void* x)
			{
				const uint* vec = reinterpret_cast<const uint*>(x);
				glUniform4ui(index, vec[0], vec[1], vec[2], vec[3]);
			}
			#pragma endregion
			#pragma region Indirect
			void SetUniform(std::size_t, float x) const;
			void SetUniform(std::size_t, float x, float y) const;
			void SetUniform(std::size_t, float x, float y, float z) const;
			void SetUniform(std::size_t, float x, float y, float z, float w) const;

			void SetUniform(std::size_t, int x) const;
			void SetUniform(std::size_t, int x, int y) const;
			void SetUniform(std::size_t, int x, int y, int z) const;
			void SetUniform(std::size_t, int x, int y, int z, int w) const;

			void SetUniform(std::size_t, const glm::mat4& matrix, bool transpose = false) const;
			void SetUniform(std::size_t, const glm::mat3& matrix, bool transpose = false) const;
			void SetUniform(std::size_t, const std::vector<glm::mat4>& matrices, bool transpose = false) const;
			#pragma endregion
		#pragma endregion
		};
	}
}