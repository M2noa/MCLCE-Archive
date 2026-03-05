#include "BoneMatrices.h"
#include "Rendering/RenderingFactory.h"

namespace Esteem
{
	inline BoneMatrices::BoneMatrices(value_type* matrices, size_t index, size_t size)
		: matrices(matrices)
		, index(index)
		, size(size)
		, dirty(true)
	{ }

	inline void BoneMatrices::UpdateMatrices(const value_type* matrices, size_t size, size_t offset)
	{
		memcpy(this->matrices + index + offset, matrices, size * sizeof(value_type));
	}

	inline void BoneMatrices::UpdateMatrices(const value_type* matrices)
	{
		BoneMatrices::UpdateMatrices(matrices, size, 0);
	}

	inline const BoneMatrices::value_type* BoneMatrices::GetMatrices() const { return matrices; }
	inline BoneMatrices::value_type* BoneMatrices::GetMatrices() { return matrices; }

	inline const BoneMatrices::value_type& BoneMatrices::GetMatrix(size_t index) const { return matrices[index]; }
	inline void BoneMatrices::SetMatrix(size_t index, const BoneMatrices::value_type& matrix) { matrices[index] = matrix; }

	inline size_t BoneMatrices::GetIndex() const { return index; }
	inline size_t BoneMatrices::GetSize() const { return size; }
}
