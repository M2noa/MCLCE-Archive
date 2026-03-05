#include "World.h"

#include <thread>

#include "Window/View.h"
#include "Utils/Time.h"

#include "Utils/Data.h"
#include "./Objects/WorldDataPart.h"
#include "Rendering/Objects/RenderCamera.h"
#include "Rendering/Objects/LightObject.h"
#include "Culling/Objects/CullingObject.h"
#include "./Objects/Transform.h"

#include "GameEngine.h"

namespace Esteem
{
	World::World(bool fixedSizeWorld, bool enableSoftBody)
		: audio(nullptr)
		, simulationSpeed(1.f)
		, physics(fixedSizeWorld, enableSoftBody)
		, defaultCamera()
		, renderCamera()
	{ }

	const cgc::strong_ptr<Camera>& World::GetPrimaryCamera()
	{
		return !cameras.empty() ? cameras[0] : defaultCamera;
	}

	void World::Trash(IWorldObject* worldObject)
	{
		trashedWorldObjects.push_back(worldObject);
	}

#pragma region Adders/Removers
	void World::AddWorldObject(const cgc::strong_ptr<Transform>& worldObject)
	{
		if(std::find(nonTransformWorldObjects.begin(), nonTransformWorldObjects.end(), worldObject) == nonTransformWorldObjects.end())
		{
			worldObjects.push_back(worldObject);
			nonTransformWorldObjects.push_back(worldObject);
		}
	}

	void World::RemoveWorldObject(const cgc::raw_ptr<Transform>& worldObject)
	{
		auto foundObject = std::find(nonTransformWorldObjects.begin(), nonTransformWorldObjects.end(), worldObject);
		if(foundObject != nonTransformWorldObjects.end())
			nonTransformWorldObjects.erase(foundObject);

		auto foundWorldObject = std::find(worldObjects.begin(), worldObjects.end(), worldObject);
		if (foundWorldObject != worldObjects.end())
			worldObjects.erase(foundWorldObject);
	}

	cgc::strong_ptr<Entity> World::CreateEntity()
	{
		return entities.emplace(this);
	}

	cgc::strong_ptr<GraphicalOverlay> World::CreateGraphicalOverlay(const std::string& name)
	{
		return graphicalOverlays.emplace(name);
	}

	cgc::strong_ptr<RenderCamera> World::CreateRenderCamera()
	{
		return renderCamerasData.emplace();
	}
	
	void World::AddCullingObject(CullingObject* cullingObject)
	{
		culling.RegisterCullingObject(cullingObject);
	}

	void World::AddPhysicsObject(btCollisionObject& object, int filterGroup, int filterMask)
	{
		physics.AddPhysicsObject(object, filterGroup, filterMask);
	}

	void World::RemovePhysicsObject(btCollisionObject& object)
	{
		physics.RemovePhysicsObject(object);
	}

	void World::AddPhysicsCharacter(KinematicBody& character)
	{
		physics.AddPhysicsCharacter(character);
	}

	void World::RemovePhysicsCharacter(KinematicBody& character)
	{
		physics.RemovePhysicsCharacter(character);
	}

	void World::DirtyCleanUp()
	{
		while(!trashedWorldObjects.empty())
		{
			IWorldObject* worldObject = trashedWorldObjects.front();

			delete worldObject;
			trashedWorldObjects.pop_front();
		}

		physics.DirtyCleanUp();


	}

	void World::DirtyRenderCleanUp()
	{
		//constituents.cameras.DirtyCleanUp();
		constituents.meshRenderers.DirtyCleanUp();

		BoneMatrices* boneMatrices;
		while(dirtyBoneMatrices.pop(boneMatrices))
			boneMatrices->UpdateMatrices();
	}
	
	bool World::LoadWorld(std::string world)
	{
		loaded = false;

		if (Data::FileExists(RESOURCES_PATH + WORLD_PARTS_PATH + world + "/"))
		{
			worldName = world;
			loaded = true;
		}

		return loaded;
	}
	
	void World::OnEntityMove(const cgc::strong_ptr<Entity>& entity)
	{
		RemoveWorldObject(cgc::raw_ptr<Transform>(entity));
		AddWorldObject(entity);
	}

	void World::Update()
	{
		while (!delayedActions.empty() && delayedActions.front().timeToCall < Time::RenderTime())
		{
			delayedActions.front().function();
			delayedActions.pop();
		}

		constituents.animators.Update();

		for (auto* system : systems)
			system->Update();
	}

	void World::LateUpdate()
	{
		/*while (!delayedActions.empty() && delayedActions.front().timeToCall < Time::RenderTime())
		{
			delayedActions.front().function();
			delayedActions.pop();
		}

		constituents.animators.Update();*/

		for (auto* system : systems)
			system->LateUpdate();
	}
	
	void World::UpdateThreadTasks(std::deque<std::function<void()>>& taskQueue, uint8 threadCount)
	{
		// Update Trigger system
		taskQueue.emplace_back(std::bind(&World::Update, this));
		taskQueue.emplace_back(std::bind(&TriggerSystem::Update, &triggerSystem));
		taskQueue.emplace_back(std::bind(&NetworkSystem::Update, &networkSystem));

		// Update Physics engine
		//taskQueue.emplace_back(std::bind(&IPhysicsEngine::UpdateWorld, physicsEngine, Time::deltaTime));
		//physicsEngine->UpdateWorld(Time::deltaTime);
	}

	void World::SetRenderCamera(const cgc::strong_ptr<Camera>& renderCamera)
	{
		//if (renderCamera != nullptr)
		{
			this->renderCamera = renderCamera;
			if (renderData)
				renderData->SetRenderCamera(renderCamera);
		}
	}

	void World::ResetRenderCamera()
	{
		const cgc::strong_ptr<Camera>& newCamera = GetPrimaryCamera();
		this->renderCamera = newCamera ? newCamera : defaultCamera;

		if (renderData)
			renderData->SetRenderCamera(renderCamera);
	}

	void World::AddDelayedAction(std::chrono::microseconds timeToCall, const std::function<void()>& action)
	{
		delayedActions.emplace(timeToCall, action);
	}

	cgc::strong_ptr<World> World::CreateWorld()
	{
		return GameEngine::CreateWorld();
	}
}