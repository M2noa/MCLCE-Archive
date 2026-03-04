#pragma once

#include "../../../Objects/RenderObject.h"
#include "OpenGLMaterial.h"

namespace Esteem
{
	class AbstractMesh;

	namespace OpenGL
	{
		class VAO;
		class OpenGLShader;

		class OpenGLRenderObject : public RenderObject
		{
		friend class OpenGLFactory;
		friend class cgc::constructor;

		private:
			uint id;
			cgc::strong_ptr<VAO> vao;
			uint renderMode;

			OpenGLRenderObject(uint id, const cgc::strong_ptr<AbstractMesh>& mesh, const cgc::strong_ptr<OpenGLMaterial>& material);
			
		public:
			inline uint GetOGLRenderMode() const { return renderMode; }

			inline uint GetID() const { return id; }

			inline void SetMaterial(const cgc::strong_ptr<OpenGLMaterial>& material) { this->material = material; }

			inline const cgc::strong_ptr<VAO>& GetVAO() const { return vao; }

			void BindCustomBuffers(const cgc::raw_ptr<OpenGLShader>& shader) const;
		};
	}
}