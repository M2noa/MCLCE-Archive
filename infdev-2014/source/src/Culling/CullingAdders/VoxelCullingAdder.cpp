#include "VoxelCullingAdder.h"
#include "../CullingCheckers/FrustumCullingChecker.h"
#include "../Culling.h"

namespace Esteem
{
	VoxelCullingAdder::VoxelCullingAdder(Culling* culling, FrustumCullingChecker* frustumChecker)
		: culling(culling)
		, frustumChecker(frustumChecker)
	{ }

	void VoxelCullingAdder::AddObjects()
	{
		std::lock_guard lk(lock);
		for (const auto* object : objects)
		{
			if (frustumChecker->CubeInFrustum(object->GetAABB()))
				culling->AppendRenderObjectsDirectly(object->GetRenderObjects());
		}
	}

	void VoxelCullingAdder::RegisterCullingObject(CullingObject* cullingObject)
	{
		std::lock_guard lk(lock);
		objects.push_back(cullingObject);
		cullingObject->SetContainer(this);
	}

	void VoxelCullingAdder::UnRegisterCullingObject(CullingObject* cullingObject)
	{
		std::lock_guard lk(lock);
		objects.erase(std::remove(objects.begin(), objects.end(), cullingObject), objects.end());
		cullingObject->SetContainer(nullptr);
	}

	void VoxelCullingAdder::DebugRender(std::vector<DevRenderObject>& renderObjects)
	{
		for (auto& child : objects)
		{
			glm::vec3 size = (child->GetAABB().end - child->GetAABB().begin); //* glm::vec3(0.5);
			renderObjects.emplace_back(DevRenderObject::GeometryType::BOX, DevRenderObject::RenderType::LINES, child->GetCenter(), glm::quat(), size, Color8u::red);
		}
	}

	void VoxelCullingAdder::Translate(Partitioning::SpatialObject* cullingObject)
	{

	}

	void VoxelCullingAdder::Scale(Partitioning::SpatialObject* cullingObject)
	{

	}

	void VoxelCullingAdder::Remove(Partitioning::SpatialObject* cullingObject)
	{
		std::lock_guard lk(lock);
		objects.erase(std::remove(objects.begin(), objects.end(), static_cast<CullingObject*>(cullingObject)), objects.end());
	}
}