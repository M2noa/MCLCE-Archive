#pragma once

#include "./Culling.h"

namespace Esteem
{
	inline RenderList& Culling::GetRenderObjectList()
	{
		return renderList;
	}

	inline std::vector<Light*>& Culling::GetLightList()
	{
		return lights;
	}

	// This requires speed so inline it
	inline void Culling::AddObjectToRenderList(RenderObject* renderObject)
	{
		for (uint i = 0; i < cullingCheckers.size(); ++i)
		{
			if (!cullingCheckers[i]->SingleObjectCheck(renderObject))
				return;
		}

		this->renderList[renderObject->GetRenderOrder()].push_back(renderObject);
	}

	// This requires speed so inline it
	inline void Culling::AppendRenderObjectsDirectly(const std::vector<cgc::strong_ptr<RenderObject>>& renderObjects)
	{
		for (auto it = renderObjects.begin(); it != renderObjects.end();)
		{
			const cgc::strong_ptr<RenderObject>& renderObject = *it++;
			if (renderObject->IsInitialized())
				this->renderList[renderObject->GetRenderOrder()].push_back(renderObject.ptr());
		}
	}

	// This requires speed so inline it
	inline void Culling::AppendRenderObjectsDirectly(const stor::lock::deque<cgc::strong_ptr<RenderObject>>& renderObjects)
	{
		std::shared_lock<std::shared_mutex> lk;
		const auto& rObjects = renderObjects.get_deque_and_lock(lk);
		for (auto& renderObject : renderObjects)
		{
			if (renderObject->IsInitialized())
				this->renderList[renderObject->GetRenderOrder()].push_back(renderObject.ptr());
		}
	}

	// This requires speed so inline it
	inline void Culling::AppendRenderObjectsDirectly(const std::vector<RenderObject*>& renderObjects)
	{
		for (RenderObject* renderObject : renderObjects)
		{
			if (renderObject->IsInitialized())
				this->renderList[renderObject->GetRenderOrder()].push_back(renderObject);
		}
	}

	// This requires speed so inline it
	inline void Culling::AppendRenderObjectsDirectly(const stor::lock::deque<RenderObject*>& renderObjects)
	{
		std::shared_lock<std::shared_mutex> lk;
		const auto& rObjects = renderObjects.get_deque_and_lock(lk);
		for (RenderObject* renderObject : rObjects)
		{
			if (renderObject->IsInitialized())
				this->renderList[renderObject->GetRenderOrder()].push_back(renderObject);
		}
	}

	inline void Culling::UnRegisterCullingObject(CullingObject* cullingObject)
	{
		cullingObject->RemoveFromCulling();
	}

	inline void Culling::DebugRender(std::vector<DevRenderObject>& renderObjects)
	{
		for (auto& adder : cullingAdders)
			adder->DebugRender(renderObjects);
	}
}