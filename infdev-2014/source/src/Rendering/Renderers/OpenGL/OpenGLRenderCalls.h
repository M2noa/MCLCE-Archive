#pragma once

#include <GL/glew.h>
//#include <SFML/OpenGL.hpp>

#include "Objects/OpenGLRenderObject.h"

namespace Esteem
{
	namespace OpenGL
	{
		inline void DrawElementsOnce(cgc::raw_ptr<EBO> ebo)
		{
			glDrawElements(static_cast<uint>(ebo->GetRenderType()), ebo->GetIndicesCount(), ebo->GetIndicesType(), 0);
		}

		inline void DrawElementsInstanced(cgc::raw_ptr<EBO> ebo, std::size_t instanceCount)
		{
			glDrawElementsInstanced(static_cast<uint>(ebo->GetRenderType()), ebo->GetIndicesCount(), ebo->GetIndicesType(), 0, instanceCount);
		}

		inline void DrawElements(cgc::raw_ptr<EBO> ebo, std::size_t instanceCount)
		{
			if (instanceCount == 1)
				DrawElementsOnce(ebo);
			else
				DrawElementsInstanced(ebo, instanceCount);
		}

		inline void DrawArraysOnce(cgc::raw_ptr<VBO> vbo)
		{
			glDrawArrays(static_cast<uint>(vbo->GetRenderType()), 0, vbo->GetVertexCount());
		}

		inline void DrawArraysInstanced(cgc::raw_ptr<VBO> vbo, std::size_t instanceCount)
		{
			glDrawArraysInstanced(static_cast<uint>(vbo->GetRenderType()), 0, vbo->GetVertexCount(), instanceCount);
		}

		inline void DrawArrays(cgc::raw_ptr<VBO> vbo, std::size_t instanceCount)
		{
			if (instanceCount == 1)
				DrawArraysOnce(vbo);
			else
				DrawArraysInstanced(vbo, instanceCount);
		}

		inline void DrawRenderObject(const OpenGLRenderObject& renderObject)
		{
			cgc::raw_ptr<VAO> vao = renderObject.GetVAO();
			cgc::raw_ptr<EBO> ebo = vao->GetEBO();
			if (ebo != nullptr)
				DrawElements(ebo, renderObject.GetInstanceCount());
			else
				DrawArrays(vao->GetVBO(), renderObject.GetInstanceCount());
		}
	}
}