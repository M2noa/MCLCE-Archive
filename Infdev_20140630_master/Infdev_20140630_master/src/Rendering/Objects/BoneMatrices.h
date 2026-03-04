#pragma once

#include "stdafx.h"
#include <vector>
#include <glm/mat4x4.hpp>

namespace Esteem
{
	class RenderingFactory;

	class BoneMatrices
	{
	public:
		typedef glm::mat3x4 value_type;
		typedef value_type::transpose_type transpose_type;

	protected:
		value_type* matrices;
		size_t size;
		size_t index;
		bool dirty;

	public:
		BoneMatrices(value_type* matrices, size_t index, size_t size);

		virtual void UpdateMatrices() = 0;
		virtual void UpdateMatrices(const value_type* matrices, size_t size, size_t offset);
		virtual void UpdateMatrices(const value_type* matrices);

		const value_type* GetMatrices() const;
		value_type* GetMatrices();

		const BoneMatrices::value_type& GetMatrix(size_t index) const;
		void SetMatrix(size_t index, const BoneMatrices::value_type& matrix);

		size_t GetIndex() const;
		size_t GetSize() const;
	};
}

#include "BoneMatrices.inl"