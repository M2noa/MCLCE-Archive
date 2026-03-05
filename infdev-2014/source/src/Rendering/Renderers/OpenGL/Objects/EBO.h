#pragma once

#include "../../../Objects/IEBO.h"
#include "../../../rtypes.h"
#include <cppu/cgc/constructor.h>

namespace Esteem
{
	namespace OpenGL
	{
		class OpenGLFactory;

		class EBO : public IEBO
		{
		friend class OpenGLFactory;
		friend class cgc::constructor;

		private:
			uint id;
			uint indices;
			uint bufferSize;
			uint type;
			TRIANGLE_TYPE triangleType;

			/// \brief Create an Element Buffer Object, used to define indices for a VBO with vertex data
			/// \param indices	array of indices
			/// \param bufferType	buffer type, GL_STATIC_DRAW, GL_DYNAMIC_DRAW or GL_STREAM_DRAW,
			///						gives a hint to the gpu how to store this buffer. Default is GL_STATIC_DRAW
			///						which is the fastest to render with. Use others to allow/optimize for changing data
			EBO(uint id, uint bufferSize, uint indices, uint type, TRIANGLE_TYPE triangleType = TRIANGLE_TYPE::TRIANGLES);

		public:
			~EBO();

			virtual uint GetID() const { return id; }
			inline uint GetIndicesCount() const { return indices; }
			inline uint GetIndicesType() const { return type; };
			inline TRIANGLE_TYPE GetRenderType() const { return triangleType; };

			void Bind() const;
			static void UnBind();
		};
	}
}