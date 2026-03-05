#include "VAO.h"
#include "../OpenGLFactory.h"

namespace Esteem
{
	namespace OpenGL
	{
		VAO::VAO(uint id)
			: id(id) // 0 is default (non bind)
			, vbo()
			, ebo()
		{ }

		VAO::VAO()
			: id(0) // 0 is default (non bind)
			, vbo()
			, ebo()
		{ }

		VAO::~VAO()
		{
			if (id > 0)
				glDeleteVertexArrays(1, &id);
		}

		void VAO::AttachVBO(const cgc::strong_ptr<VBO>& vbo)
		{
			this->vbo = vbo;
			glBindVertexArray(id);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			vbo->Bind();
			VAO::UnBind();
		}

		void VAO::AttachEBO(const cgc::strong_ptr<EBO>& ebo)
		{
			if(ebo != nullptr)
			{
				this->ebo = ebo;
				glBindVertexArray(id);
				EBO::UnBind();
				ebo->Bind();
				VAO::UnBind();
			}
		}

		void VAO::Bind()
		{
			glBindVertexArray(id);
			ebo->Bind();
			vbo->Bind();
		}
		
		void VAO::UnBind()
		{
			glBindVertexArray(0);
		}
	}
}