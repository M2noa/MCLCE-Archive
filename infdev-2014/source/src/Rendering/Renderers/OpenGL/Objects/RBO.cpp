#include "./RBO.h"

namespace Esteem
{
	namespace OpenGL
	{
		RBO::RBO(glm::ivec2 size, GLenum bufferType)
			: id(0) // 0 is default (non bind)
		{
			glGenRenderbuffers(1, &id);
			Bind();
			glRenderbufferStorage(GL_RENDERBUFFER, bufferType, size.x, size.y);
			RBO::UnBind();
		}

		RBO::~RBO()
		{
			glDeleteBuffers(1, &id);
		}

		void RBO::Bind() const
		{
			glBindRenderbuffer(GL_RENDERBUFFER, id);
		}

		void RBO::UnBind()
		{
			glBindRenderbuffer(GL_RENDERBUFFER, 0);
		}
	}
}