#include "OpenGLBoneMatrices.h"
#include "../../../RenderingFactory.h"

namespace Esteem
{
	namespace OpenGL
	{
		inline OpenGLBoneMatrices::OpenGLBoneMatrices(value_type* matrices, uint index, uint size, const cgc::strong_ptr<UBO>& ubo)
			: BoneMatrices(matrices, index, size)
			, ubo(ubo)
		{}

		inline void OpenGLBoneMatrices::UpdateMatrices()
		{
			UpdateUBO();
		}

		inline void OpenGLBoneMatrices::UpdateMatrices(const value_type* matrices, size_t size, size_t offset)
		{
			BoneMatrices::UpdateMatrices(matrices);
			OpenGLBoneMatrices::UpdateMatrices();
		}

		inline void OpenGLBoneMatrices::UpdateMatrices(const value_type* matrices)
		{
			OpenGLBoneMatrices::UpdateMatrices(matrices, size, 0);
		}

		inline void OpenGLBoneMatrices::UpdateUBO()
		{
			if (ubo)
				ubo->UpdateBuffer(matrices, size * sizeof(value_type));
		}

		inline const cgc::strong_ptr<UBO>& OpenGLBoneMatrices::GetUBO() const { return ubo; }
	}
}
