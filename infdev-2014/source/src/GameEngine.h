#pragma once

#include "stdafx.h"

#include <thread>
#include <condition_variable>
#include <mutex>
#include <atomic>
#include <deque>
#include <chrono>

#include "World/World.h"
#include "Client/RenderView.h"
#include "Window/View.h"
#include "Utils/Data.h"

#include "World/Scene.h"

namespace Esteem
{
	class GameEngine
	{
	private:
		/// \brief main game-loop check, if true it should run
		static bool running;
		/// \brief True if we cap/limit the fps
		static bool enabledFpsCap;
		/// \brief target fps limit in frames per second
		static float targetFps;
		// first running game engine
		static GameEngine* defaultEngine;

		/// \brief the view that will represent our world/game to the world
		/// This can be any view that's necesary, e.g.: form applications, OpengGL Renderer,
		/// Vulkan Renderer, DirectX Renderer, or simply one that does nothing.
		std::unique_ptr<IView> view;

		/// \brief data pointer that belongs to the world
		std::vector<cgc::strong_ptr<World>> worlds;

		/// \briefthe amount of threads that'll be run
		uint8 threadCount;

		/// \brief count (operations) per batch
		std::size_t batchesCount;
		/// \brief amount of batches that need to be processed
		std::size_t batchedAmount;

		/// \brief fixed fps limiter
		static std::chrono::high_resolution_clock::duration targetSleepTime;
		
		// Threading
		/// \brief mutex lock to lock all 
		std::mutex taskLock;
		/// \brief condition_variable to wake up sleeping workers
		std::condition_variable taskWaiter;
		/// \brief current count of active/working threads
		std::atomic<uint8> workersActive;

		/// \brief thread handlers
		std::vector<std::thread> threads;
		/// \brief tasks that should be executed in the next sequence
		std::deque<std::pair<std::function<void()>, World*>> tasks;
		/// \brief which task should be done next
		std::atomic<uint> nextTask;

		cgc::strong_ptr<Scene> scene;
		cgc::strong_ptr<Scene> newScene;

		/// \brief Worker thread function to execute all parallel operations
		void Worker();
		/// \brief Function for the main thread to help along with the other workers
		void MainWorker(cgc::raw_ptr<World>);

		/// \brief task collector TODO: requires rename
		void ThreadLoop1(cgc::raw_ptr<World> world);
		/// \brief task collector TODO: requires rename
		void ThreadLoop2(cgc::raw_ptr<World> world);
		/// \brief task collector TODO: requires rename
		void ThreadLoop3(cgc::raw_ptr<World> world);

		/// \brief Helper function to easier loop a certain size of a list/queue
		void WorldObjectsUpdate(cgc::raw_ptr<World> world, size_t from, size_t to);
		/// \brief Helper function to easier loop a certain size of a list/queue
		void WorldObjectsLateUpdate(cgc::raw_ptr<World> world, size_t from, size_t to);

	public:
		/// \brief contruct the GameEngine
		GameEngine();
		/// \brief destruct the GameEngine
		~GameEngine();

		/// \brief get all world
		inline const std::vector<cgc::strong_ptr<World>>& GetWorlds() const { return worlds; }

		inline void AddWorld(const cgc::strong_ptr<World>& world) { if (worlds.size() == 0) view->SetWorld(world.ptr()); if (world) worlds.push_back(world);}
		inline void RemoveWorld(const cgc::strong_ptr<World>& world) { worlds.erase(std::find(worlds.begin(), worlds.end(), world)); }
		/// \brief Create a new dummy world to start or load our world in
		static cgc::strong_ptr<World> CreateWorld();

		inline IView* GetView() const {	return view.get(); }

		void SetScene(const cgc::strong_ptr<Scene>& scene);

		/// \brief thread function, main execution for the game engine (runs apart from initial thread)
		void Execute();

		/// \brief enable the fps limiter
		static void EnableFPSLimit(bool state);
		/// \brief check if the fps limiter is enabled
		static inline bool IsFPSLimitEnabled() { return enabledFpsCap; }

		/// \brief set fps limit target
		static void SetTargetFPS(int limit);
		/// \brief get the current set fps limit value
		static inline float GetTargetFPS() { return targetFps; }

		/// \brief check if the engine is still running
		static inline bool IsRunning() { return running; }

		/// \brief Quit the entire game engine
		static GameEngine* GetEngine() { return defaultEngine; }
		static void QuitEngine();
	};
}