#pragma once

#include "./CullingObject.h"

#include "Memory/Partitioning/Spatial/ISpatialObjectContainer.h"

namespace Esteem
{
	inline bool CullingObject::IsDynamic() const
	{
		return this->type == Type::DYNAMIC;
	}

	inline void CullingObject::TransformUpdate()
	{
		if (container)
			container->Translate(this);
	}

	inline const std::vector<cgc::strong_ptr<RenderObject>>& CullingObject::GetRenderObjects() const
	{
		return renderObjects;
	}

	inline void CullingObject::SetRenderObjects(const std::vector<cgc::strong_ptr<RenderObject>>& copy)
	{
		renderObjects = copy;
	}

	inline void CullingObject::SetRenderObjects(std::vector<cgc::strong_ptr<RenderObject>>&& swap)
	{
		this->renderObjects.swap(swap);
	}

	inline void CullingObject::ClearRenderObjects()
	{
		this->renderObjects.clear();
	}
}