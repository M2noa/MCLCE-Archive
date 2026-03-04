#pragma once

#include "stdafx.h"
#include <string>
#include <GL/glew.h>
#include <cppu/cgc/constructor.h>

namespace Esteem
{
	namespace OpenGL
	{
		class OpenGLSubShader
		{
		friend class OpenGLFactory;
		friend class cgc::constructor;

		private:
			uint32_t type;
			uint32_t id;
			float cutoff;
			bool compiled;
			
			OpenGLSubShader(GLenum type, uint32_t id);

		public:
			~OpenGLSubShader();

			inline uint32_t GetID() const { return id; };
			inline GLenum GetType() const { return type; };
			inline float GetCutoff() const { return cutoff; }
		};
	}
}