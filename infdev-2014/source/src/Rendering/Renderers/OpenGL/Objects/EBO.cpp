#include "EBO.h"
#include <GL/glew.h>

namespace Esteem
{
	namespace OpenGL
	{
		EBO::EBO(uint id, uint bufferSize, uint indices, uint type, TRIANGLE_TYPE triangleType)
			: id(id) // 0 is default (non bind)
			, indices(indices)
			, type(type)
			, triangleType(triangleType)
			, bufferSize(bufferSize)
		{
		}

		EBO::~EBO()
		{
#ifndef OPENGL_NO_INVALIDATE_BEFORE_DELETE
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, 0, nullptr, GL_STATIC_DRAW);
			//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
#endif
			glDeleteBuffers(1, &id);
		}

		void EBO::Bind() const
		{
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
		}

		void EBO::UnBind()
		{
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		}
	}
}
