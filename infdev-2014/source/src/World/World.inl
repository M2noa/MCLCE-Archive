#pragma once

#include "World.h"
#include "./Objects/IWorldObject.h"

namespace Esteem
{
	inline bool World::AddDirty(IWorldObject* worldObject)
	{
		return dirtyWorldObjects.push(worldObject);
	}
	
	inline void World::RemoveDirty(IWorldObject* worldObject)
	{
		throw("not implemented yet");
	}

	inline bool World::AddDirty(BoneMatrices* worldObject)
	{
		return dirtyBoneMatrices.push(worldObject);
	}

	inline void World::RemoveDirty(BoneMatrices* worldObject)
	{
		throw("not implemented yet");
	}

	inline const std::deque<cgc::strong_ptr<Transform>>&  World::GetWorldObjects() const
	{
		return worldObjects;
	}

	inline bool World::WorldIsLoaded() const
	{
		return loaded;
	}
	
	inline float World::GetSimulationSpeed() const
	{
		return simulationSpeed;
	}

	inline void World::SetSimulationSpeed(float simulationSpeed)
	{
		this->simulationSpeed = simulationSpeed;
	}

	inline TriggerSystem& World::GetTriggerSystem()
	{
		return triggerSystem;
	}

	inline NetworkSystem& World::GetNetworkSystem()
	{
		return networkSystem;
	}

	inline WorldDataConstituents& World::GetWorldConstituents()
	{
		return constituents;
	}

	inline Culling& World::GetCulling()
	{
		return culling;
	}

	inline Physics& World::Physics()
	{
		return physics;
	}

	inline const Physics& World::Physics() const
	{
		return physics;
	}

	inline IRenderData* World::GetRenderData() const
	{
		return renderData.get();
	}

	inline void World::SetRenderData(std::unique_ptr<IRenderData>&& renderData)
	{
		this->renderData = std::move(renderData);
	}

	inline std::vector<DevRenderObject>& World::GetDebugRenderObjects()
	{
		return devRenderObjects;
	}

	inline void World::AddSystem(ISystem* system)
	{
		systems.push_back(system);
	}

	inline void World::RemoveSystem(const ISystem* system)
	{
		(void)systems.erase(std::remove(systems.begin(), systems.end(), system));
	}
}