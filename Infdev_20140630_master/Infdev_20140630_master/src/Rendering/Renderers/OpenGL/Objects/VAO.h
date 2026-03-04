#pragma once

#include <vector>
#include "./EBO.h"
#include "./VBO.h"
#include <cppu/cgc/constructor.h>

namespace Esteem
{
	namespace OpenGL
	{
		class VAO
		{
		friend class OpenGLFactory;
		friend class cgc::constructor;

		private:
			uint id;
			cgc::strong_ptr<VBO> vbo;
			cgc::strong_ptr<EBO> ebo;

			VAO(uint id);

		public:
			VAO();
			
			~VAO();

			/// \brief attach a vbo with this VAO
			void AttachVBO(const cgc::strong_ptr<VBO>& vbo);
			/// \brief will set the ebo object, when ebo is already set it will override it
			void AttachEBO(const cgc::strong_ptr<EBO>& ebo);


			/// \brief get id of this VAO (OpenGL ID)
			inline uint GetID() const { return id; };

			/// 
			inline const cgc::strong_ptr<EBO>& GetEBO() const { return ebo; };
			inline const cgc::strong_ptr<VBO>& GetVBO() const { return vbo; };
			
			void Bind();
			static void UnBind();
		};
	}
}
