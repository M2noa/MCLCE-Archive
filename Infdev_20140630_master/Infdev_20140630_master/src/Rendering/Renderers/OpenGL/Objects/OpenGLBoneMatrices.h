#pragma once

#include "Rendering/Objects/BoneMatrices.h"
#include <cppu/cgc/pointers.h>
#include "./UBO.h"

namespace Esteem
{
	namespace OpenGL
	{
		class OpenGLBoneMatrices : public BoneMatrices
		{
		friend class OpenGLFactory;
		private:
			cgc::strong_ptr<UBO> ubo;

			// TODO: (needs thought) maybe add a updater that only updates a piece of all the matrices?
			void UpdateUBO();

		public:
			OpenGLBoneMatrices(value_type* matrices, uint index, uint size, const cgc::strong_ptr<UBO>& ubo);
			
			virtual void UpdateMatrices();
			virtual void UpdateMatrices(const value_type* matrices, size_t size, size_t offset);
			virtual void UpdateMatrices(const value_type* matrices);

			inline const cgc::strong_ptr<UBO>& GetUBO() const;
		};
	}
}

#include "OpenGLBoneMatrices.inl"