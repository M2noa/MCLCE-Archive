#include "OpenGLSubShader.h"

namespace Esteem
{
	namespace OpenGL
	{
		OpenGLSubShader::OpenGLSubShader(GLenum type, uint32_t id)
			: type(type)
			, cutoff(0.f)
			, id(id)
			, compiled(false)
		{
		}

		OpenGLSubShader::~OpenGLSubShader()
		{
			glDeleteShader(id);
		}
	}
}