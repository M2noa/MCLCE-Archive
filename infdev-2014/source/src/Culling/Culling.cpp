#include "Culling.h"
#include "CullingAdders/VoxelCullingAdder.h"
#include "Rendering/Objects/RenderCamera.h"

#include "World/Constituents/Camera.h"
#include "World/World.h"

#include "Rendering/Objects/RenderCamera.h"

namespace Esteem
{
	Culling::Culling()
	{
		//cullingAdders[CullingObject::Type::STATIC] = new OctreeCullingAdder<Partitioning::OctreeNodeWrapping::DEFAULT>(this, &frustumChecker, CullingObject::Flags::NONE));
		cullingAdders[CullingObject::Type::STATIC] = new OctreeCullingAdder<Partitioning::OctreeNodeWrapping::TIGHT>(this, &frustumChecker);
		cullingAdders[CullingObject::Type::DYNAMIC] = new OctreeCullingAdder<Partitioning::OctreeNodeWrapping::LOOSE>(this, &frustumChecker);
		cullingAdders[CullingObject::Type::VOXEL] = new VoxelCullingAdder(this, &frustumChecker);
	}

	void Culling::PreRenderUpdate(RenderCamera* renderCamera)
	{
		//std::unique_lock<std::mutex> lk(lock);

		frustumChecker.CalculateFrustum(renderCamera->data.viewProjectMatrix);

		// clean up lists
		lights.clear();
		for(auto& list : renderList)
			list.clear();
		
		for (auto& adder : cullingAdders)
			adder->AddObjects();
	}
	
	bool Culling::RegisterCullingObject(CullingObject* cullingObject)
	{
		if (cullingObject->GetContainer() == nullptr)
		{
			const CullingObject::Type type = cullingObject->type;
			if (cullingObject->type < CullingObject::Type::COUNT)
			{
				cullingAdders[type]->RegisterCullingObject(cullingObject);
				return true;
			}
			else
				Debug::LogError("Couldn't add Culling Object of type " + std::to_string(type) + ", it's not supported, max: " + std::to_string(CullingObject::Type::COUNT - 1));
		}
		else
			Debug::LogError("Couldn't add Culling Object, it's already registered with a culling instance.");

		return false;
	}
}