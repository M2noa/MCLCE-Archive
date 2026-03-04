#pragma once

#include "../../../Objects/IVBO.h"
#include "../../../rtypes.h"
#include "./OpenGLShader.h"
#include <vector>
#include <string>
#include <GL/glew.h>

#include <cppu/cgc/pointers.h>

namespace Esteem
{
	namespace OpenGL
	{
		class VBO : public IVBO
		{
		friend class OpenGLFactory;
		friend class cgc::constructor;

		private:
			class VertexAttribute
			{
			public:
				std::string attribName;
				uint index;
				uint size;
				GLenum type;
				GLboolean normalized;
				uint stride;
				void* offset;
				bool enabled;

				void(*SetAttribute)(const VertexAttribute&);
			};

			uint id;
			std::size_t vertexCount;
			uint bufferSize;
			TRIANGLE_TYPE triangleType;
			cgc::strong_ptr<OpenGLShader> shader;
			std::vector<VertexAttribute> attributes;

			/// \brief Create a Vertex Buffer Object, used to store data that will take it's place on the gpu memory
			/// \param indices	array of indices
			/// \param bufferType	buffer type, GL_STATIC_DRAW, GL_DYNAMIC_DRAW or GL_STREAM_DRAW,
			///						gives a hint to the gpu how to store this buffer. Default is GL_STATIC_DRAW
			///						which is the fastest to render with. Use others to allow/optimize for changing data
			VBO(uint id, uint bufferSize, std::size_t vertexCount);

		public:
			virtual ~VBO();

			void SetShader(const cgc::strong_ptr<OpenGLShader>& shader);
			inline const cgc::strong_ptr<OpenGLShader>& GetShader() const { return shader; }

			virtual void AddVertexAttribute(std::string attribName, uint size, uint type, uint stride, void* offset, Mapping mapping = IVBO::Mapping::FLOAT);
			virtual void RemoveVertexAttribute(std::string attribName);
			
			inline uint GetID() const {	return id; }
			inline TRIANGLE_TYPE GetRenderType() { return triangleType; }
			inline std::size_t GetVertexCount() { return vertexCount; }


			static void SetAttributeWrong(const VertexAttribute& attribute);
			static void SetAttributeFloatTypes(const VertexAttribute& attribute) { glVertexAttribPointer(attribute.index, attribute.size, attribute.type, attribute.normalized, attribute.stride, attribute.offset); }
			static void SetAttributeIntegerTypes(const VertexAttribute& attribute) { glVertexAttribIPointer(attribute.index, attribute.size, attribute.type, attribute.stride, attribute.offset); }
			static void SetAttributeDoubleTypes(const VertexAttribute& attribute) { glVertexAttribLPointer(attribute.index, attribute.size, attribute.type, attribute.stride, attribute.offset); }
			
			void Bind();
			void UnBind();
		};
	}
}