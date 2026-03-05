#pragma once

#include "Rendering/Objects/IUBO.h"

#include <cppu/hash.h>
#include <cppu/cgc/pointers.h>

namespace Esteem
{
	namespace OpenGL
	{
		class OpenGLFactory;
		class OpenGLShader;

		class UBO : public IUBO
		{
		friend class OpenGLFactory;
		friend class cgc::constructor;

		private:
			uint id;
			uint bindingIndex;
			uint size;
			hash_t bindingHash;
			std::string bindingName;

			UBO(uint id, uint bindingIndex, std::string_view bindingName, uint size, uint usageType);

		public:
			virtual ~UBO();

			virtual uint GetID() const { return id; }
			virtual uint GetBindingIndex() const { return bindingIndex; }
			virtual const std::string& GetBindingName() const { return bindingName; }
			
			virtual void UpdateBuffer(const void* data, uint size);

			static void Bind(cgc::raw_ptr<OpenGLShader> shader, std::string_view uniformBlockName, uint index);
			static void Bind(cgc::raw_ptr<OpenGLShader> shader, hash_t bindingHash, uint index);

			//void Bind(cgc::raw_ptr<OpenGLShader> shader);
			void Bind(uint index);
			void Bind();
		};
	}
}
