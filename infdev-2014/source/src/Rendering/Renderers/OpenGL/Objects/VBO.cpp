#include "VBO.h"
#include <algorithm>
#include "../../../rtypes.h"

#include <Utils/Debug.h>

namespace Esteem
{
	namespace OpenGL
	{
		VBO::VBO(uint id, uint bufferSize, std::size_t vertexCount)
			: id(id) // 0 is default (non bind)
			, triangleType(TRIANGLE_TYPE::TRIANGLES)
			, vertexCount(vertexCount)
			, bufferSize(bufferSize)
		{ }

		VBO::~VBO()
		{
#ifndef OPENGL_NO_INVALIDATE_BEFORE_DELETE
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, 0, nullptr, GL_STATIC_DRAW);
			//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
#endif
			glDeleteBuffers(1, &id);
		}

		void VBO::SetShader(const cgc::strong_ptr<OpenGLShader>& shader)
		{
			if(shader != nullptr)
			{
				this->shader = shader;
				for(uint i = 0; i < attributes.size(); i++)
				{
					auto attribute = shader->GetAttributeVariable(attributes[i].attribName);
					if (attribute != nullptr)
					{
						attributes[i].enabled = attribute != nullptr;

						if (attributes[i].enabled)
							attributes[i].index = attribute->index;
					}
				}
			}
		}

		void VBO::AddVertexAttribute(std::string attribName, uint size, uint type, uint stride, void* offset, Mapping mapping)
		{
			RemoveVertexAttribute(attribName); // make sure it does not exist anymore

			VertexAttribute& attribute = attributes.emplace_back();
			static const decltype(attribute.SetAttribute) functions[4]{ VBO::SetAttributeIntegerTypes, VBO::SetAttributeFloatTypes, VBO::SetAttributeDoubleTypes, VBO::SetAttributeWrong };

			attribute.attribName = attribName;
			attribute.size = size;
			attribute.type = type;
			attribute.normalized = (int)mapping & (int)Mapping::NORMALIZED;
			attribute.stride = stride;
			attribute.offset = offset;
			assert(((int)mapping & 0b11) < 4);
			attribute.SetAttribute = functions[(int)mapping & 0b11];

			if(shader != nullptr)
			{
				auto attributeVariable = shader->GetAttributeVariable(attribName);
				attribute.enabled = attributeVariable != nullptr;
				
				if(attribute.enabled)
					attribute.index = attributeVariable->index;
			}
		}

		void VBO::RemoveVertexAttribute(std::string attribName)
		{
			auto foundAttribute = std::find_if(attributes.begin(), attributes.end(), [&attribName](VertexAttribute attr) -> bool { return attr.attribName == attribName; });
			if(foundAttribute != attributes.end())
				attributes.erase(foundAttribute);
		}
		
		void VBO::Bind()
		{
			glBindBuffer(GL_ARRAY_BUFFER, id);
			for(uint i = 0; i < attributes.size(); i++)
			{
				if(attributes[i].enabled)
				{
					glEnableVertexAttribArray(attributes[i].index);
					attributes[i].SetAttribute(attributes[i]);
				}
			}
		}

		void VBO::UnBind()
		{
			for(uint i = 0; i < attributes.size(); i++)
			{
				if(attributes[i].enabled)
					glDisableVertexAttribArray(attributes[i].index);
			}
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}

		void VBO::SetAttributeWrong(const VertexAttribute& attribute)
		{
			Debug::LogError("Wrong attribute mapping has been set, for some reason the programmer asked for the 4th non-existing map function, well here it is!");
		}
	}
}
