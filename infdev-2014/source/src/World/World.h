#pragma once

#include "stdafx.h"

#include <thread>
#include <deque>
#include <list>
#include <vector>
#include <unordered_map>
#include <map>
#include <mpmc_queue.h>
#include <cppu/stor/lock/deque.h>
#include <cppu/cgc/m_array.h>

#include "General/Delegate.h"
#include "World/Objects/DelayedAction.h"
#include "World/Objects/Entity.h"

#include "./WorldDataConstituents.h"

#include "./Systems/ISystem.h"
#include "./Systems/TriggerSystem/TriggerSystem.h"
#include "./Systems/EnvironmentControl/EnvironmentControl.h"

#include "Rendering/Objects/IRenderData.h"
#include "Rendering/Objects/RenderCamera.h"

#include "Physics/Physics.h"
#include "Network/NetworkSystem.h"
#include "Culling/Culling.h"

#include "Utils/Time.h"


namespace Esteem
{
	class RenderObject;
	class Transform;
	class IWorldObject;
	class WorldDataPart;
	class KinematicBody;
	class CullingObject;
	class BoneMatrices;
	class IAudioSystem;
	class AbstractComponent;
	
	class World
	{
		friend class GameEngine;
	private:
		bool loaded;
		float simulationSpeed;

		Time thisTimeInfo;
		cgc::m_array<RenderCamera> renderCamerasData;

		// Constituents
		WorldDataConstituents constituents; // stores multiple types of constituents
		std::vector<cgc::strong_ptr<Camera>> cameras;
		std::vector<LightRenderData> lights;

		// Components
		std::list<AbstractComponent*> components;

		// Systems
		Physics physics;
		Culling culling;

		std::vector<ISystem*> systems;
		EnvironmentControl environmentControl;
		NetworkSystem networkSystem;
		TriggerSystem triggerSystem;
		IAudioSystem* audio; // audio manager, handles all the audio

		// Camera
		cgc::weak_ptr<RenderCamera> defaultRenderCamera;
		cgc::strong_ptr<Camera> renderCamera; // Camera we use to render our scene
		cgc::strong_ptr<Camera> defaultCamera;
				
		// World Objects
		cgc::m_array<Entity> entities;
		cgc::m_array<GraphicalOverlay> graphicalOverlays;

		std::deque<cgc::strong_ptr<Transform>> worldObjects;
		std::vector<cgc::strong_ptr<Transform>> nonTransformWorldObjects;

		// Rendering
		std::unique_ptr<IRenderData> renderData;
		cppu::stor::lock::deque<cgc::strong_ptr<RenderObject>> renderObjects;
		std::vector<cgc::strong_ptr<RenderObject>> overlayRenderObjects;
		std::vector<DevRenderObject> devRenderObjects; // render list for debug data
				
		// Cleaning
		std::deque<IWorldObject*> trashedWorldObjects;
		cppu::stor::mpmc_queue<IWorldObject*> dirtyWorldObjects;
		cppu::stor::mpmc_queue<BoneMatrices*> dirtyBoneMatrices;
		
		std::queue<DelayedAction> delayedActions;

	protected:
		std::string worldName;

	public:
		World(bool fixedSizeWorld, bool enableSoftBody);
		~World() = default;

		void Trash(IWorldObject* worldObject);

		std::map<int, std::map<int, std::map<int, WorldDataPart*>>> worldParts;
		const cgc::strong_ptr<Camera>& GetPrimaryCamera();

		std::vector<LightRenderData>& GetLights() { return lights; };
		void AddLight(const LightRenderData& lightData) { lights.emplace_back(lightData); };

		cgc::strong_ptr<Entity> CreateEntity();
		cgc::strong_ptr<GraphicalOverlay> CreateGraphicalOverlay(const std::string& name);

		void AddWorldObject(const cgc::strong_ptr<Transform>& worldObject);
		void RemoveWorldObject(const cgc::raw_ptr<Transform>& worldObject);

		cgc::strong_ptr<RenderCamera> CreateRenderCamera();

		void AddCullingObject(CullingObject* cullingObject);

		void AddPhysicsObject(btCollisionObject& object, int filterGroup, int filterMask);
		void RemovePhysicsObject(btCollisionObject& object);

		void AddPhysicsCharacter(KinematicBody& character);
		void RemovePhysicsCharacter(KinematicBody& character);

		bool AddDirty(IWorldObject* worldObject);
		void RemoveDirty(IWorldObject* worldObject);

		void DirtyCleanUp();
		void DirtyRenderCleanUp();

		bool AddDirty(BoneMatrices* BoneMatrices);
		void RemoveDirty(BoneMatrices* BoneMatrices);

		const std::deque<cgc::strong_ptr<Transform>>& GetWorldObjects() const;

		// prev world controller
		bool LoadWorld(std::string world);
		bool WorldIsLoaded() const;

		float GetSimulationSpeed() const;
		void SetSimulationSpeed(float simulationSpeed);

		void OnEntityMove(const cgc::strong_ptr<Entity>& entity);

		void Update();
		void UpdateThreadTasks(std::deque<std::function<void()>>& taskQueue, uint8 threadCount);

		void LateUpdate();

		void AddSystem(ISystem* systems);
		void RemoveSystem(const ISystem* systems);

		WorldDataConstituents& GetWorldConstituents();

		TriggerSystem& GetTriggerSystem();
		NetworkSystem& GetNetworkSystem();

		Culling& GetCulling();

		Esteem::Physics& Physics();
		const Esteem::Physics& Physics() const;

		std::vector<DevRenderObject>& GetDebugRenderObjects();

		IRenderData* GetRenderData() const;
		void SetRenderData(std::unique_ptr<IRenderData>&& renderData);

		// Camera stuff
		/// \brief set the view to this render camera
		virtual void SetRenderCamera(const cgc::strong_ptr<Camera>& renderCamera);

		virtual const cgc::strong_ptr<Camera>& GetRenderCamera() { return renderCamera; }

		/// \brief ideal function to call when any render camera has been destroyed, this view will find a suitable render camera
		virtual void ResetRenderCamera();

		void AddDelayedAction(std::chrono::microseconds timeToCall, const std::function<void()>& action);

	public:
		static cgc::strong_ptr<World> CreateWorld();
	};
}

#include "World.inl"