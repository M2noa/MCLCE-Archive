#pragma once

#include "stdafx.h"
#include <glm/vec2.hpp>
#include <GL/glew.h>
#include <SFML/OpenGL.hpp>

#include <cppu/cgc/constructor.h>

namespace Esteem
{
	namespace OpenGL
	{
		class RBO
		{
		friend class OpenGLFactory;
		friend class cgc::constructor;

		private:
			uint id;
			glm::uvec2 size;

			RBO(glm::ivec2 size, GLenum bufferType);
		public:
			
			~RBO();

			inline uint GetID() const
			{
				return id;
			}

			void Bind() const;
			static void UnBind();
			inline const glm::uvec2& GetSize() const
			{
				return size;
			}
		};
	}
}