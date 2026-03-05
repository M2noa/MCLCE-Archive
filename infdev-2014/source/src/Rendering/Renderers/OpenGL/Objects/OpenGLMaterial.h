#pragma once

#include "stdafx.h"
#include <string>
#include <unordered_map>
#include <cppu/cgc/pointers.h>

#include "Rendering/Objects/Material.h"
#include "Rendering/Objects/Texture1D.h"
#include "Rendering/Objects/Texture2D.h"
#include "Rendering/Objects/Texture3D.h"
#include "./UBO.h"
#include "./OpenGLShader.h"

namespace Esteem
{
	namespace OpenGL
	{
		class OpenGLFactory;
		class UBO;

		class OpenGLMaterial : public Material
		{
		friend class OpenGLFactory;
		friend class cgc::constructor;

		private:
			uint id;
			cgc::strong_ptr<OpenGLShader> shader;

			std::unordered_map<std::string, OpenGLShader::UniformValue> activeUniformValues;
			std::unordered_map<std::string, OpenGLShader::UniformValue> unactiveUniformValues;

			/// \brief we store all our material information into a ubo to be able to switch fast
			cgc::strong_ptr<UBO> ubo;

			OpenGLMaterial(uint id, std::string_view path, std::string_view name);
			
		public:
			virtual ~OpenGLMaterial();

			inline uint GetID() const { return id; }
			inline const cgc::strong_ptr<OpenGLShader>& GetOpenGLShader() const { return shader; }
			virtual cgc::strong_ptr<Esteem::IShader> GetShader() const { return cgc::static_pointer_cast<IShader>(shader); }

			template<typename T>
			void SetUniformValue(const std::string& uniformName, T value)
			{
				auto found = activeUniformValues.find(uniformName);
				if (found != activeUniformValues.end())
				{
					// TODO: do some checking maybe?
					memcpy(&found->second.value[0], &value, sizeof(T));
				}
			}

			virtual void Bind() const;
		};

	}
}