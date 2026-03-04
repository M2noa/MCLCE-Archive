#pragma once

#include "./StaticBody.h"

namespace Esteem
{
	inline void StaticBody::SetModel(const cgc::strong_ptr<const Model>& model)
	{
		this->model = model;
	}

	inline const cgc::strong_ptr<const Model>& StaticBody::GetModel() const
	{
		return this->model;
	}

	inline void StaticBody::SetFilterGroup(uint32_t filterGroup)
	{
		this->filterGroup = filterGroup;
	}

	inline void StaticBody::SetFilterMask(uint32_t filterMask)
	{
		this->filterMask = filterMask;
	}
}