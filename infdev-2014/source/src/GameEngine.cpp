#include "GameEngine.h"

#include <cppu/os/StackTrace.h>
#include <chrono>

#include "Utils/Data.h"
#include "Utils/Time.h"
#include "Utils/Diagnostics.h"

#include "Input/Input.h"

#include "General/Settings.h"
#include "Physics/Physics.h"
#include "Physics/Physics.h"

#include "World/World.h"
#include "World/Scene.h"

#ifndef WIN32
#include <pthread.h>
#include <sched.h>
#endif

namespace Esteem
{
	bool GameEngine::running = true;
	bool GameEngine::enabledFpsCap = !Settings::vSyncEnabled;
	float GameEngine::targetFps = 60;
	GameEngine* GameEngine::defaultEngine = nullptr;

	std::chrono::high_resolution_clock::duration GameEngine::targetSleepTime = std::chrono::microseconds((long)(1000000.f / float(GameEngine::targetFps)));
	
	GameEngine::GameEngine()
	{
		// Initialize data utility
		Data::Initialize();
		
		// Create our view
		view = std::make_unique<RenderView>();
		//view->SetWorld(world);

		if (defaultEngine == nullptr)
			defaultEngine = this;

	}

	GameEngine::~GameEngine()
	{
		taskWaiter.notify_all();
		for (uint i = 0; i < threads.size(); ++i)
			threads[i].join();

		// Make sure we delete factories as last
		Data::Destruct();

	}

	void GameEngine::Execute()
	{
#ifdef WIN32
		if (!SetPriorityClass(GetCurrentProcess(), ABOVE_NORMAL_PRIORITY_CLASS))
			Debug::Log("Could not set program's priority " + std::to_string(GetLastError()));

		WSADATA wsaData;
		(void)WSAStartup(1 << 8, &wsaData);

		timeval tv{ 0, 0 };
		fd_set dummy;
		SOCKET s = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
#else //  WIN32
		pthread_t thisThread = pthread_self();
		int retcode;
		int policy;
		struct sched_param schedParam;

		if ((retcode = pthread_getschedparam(thisThread, &policy, &schedParam)) != 0)
			std::cout << "pthread_getschedparam, error code: " << retcode << " (GameEngine)" << std::endl;

		schedParam.sched_priority = 4;
		policy = SCHED_FIFO;
		if ((retcode = pthread_setschedparam(thisThread, policy, &schedParam)) != 0)
			std::cout << "pthread_setschedparam, error code: " << retcode << " (GameEngine)" << std::endl;
#endif //  WIN32




		GameEngine::running = true;

		std::chrono::high_resolution_clock::time_point currentTime = std::chrono::high_resolution_clock::now();
		std::chrono::high_resolution_clock::time_point previousTime = currentTime;
		std::chrono::high_resolution_clock::time_point wakeTime = currentTime;
		std::chrono::high_resolution_clock::duration deltaTime = std::chrono::seconds(0);

		// threading
		threadCount = std::thread::hardware_concurrency() - 1;
		for (int i = 0; i < threadCount; ++i)
			threads.emplace_back(&GameEngine::Worker, this);

		batchesCount = threads.size() * 4;
		
		//try
		{
			while (GameEngine::running)
			{
				// get time before any logic and rendering
				currentTime = std::chrono::high_resolution_clock::now();
				Time::SetRenderTime(std::chrono::duration_cast<Time::microseconds>(currentTime.time_since_epoch()));

				// total delta time (includes FPS limit, vsync, etc.)
				deltaTime = currentTime - previousTime;
				previousTime = currentTime;
				Time::deltaTime = std::chrono::duration_cast<std::chrono::duration<float>>(deltaTime).count();
				//Time::deltaTime *= world->GetSimulationSpeed();
				
				// switch scene when requested
				if (newScene)
				{
					if (this->scene)
						this->scene->UnloadScene();

					this->scene = newScene;
					this->scene->LoadScene();

					this->newScene = cgc::strong_ptr<Scene>();
				}

				// Update events and input on main thread
				Input::Update();
				view->WindowUpdate();
				
				// Game logic: All components
				view->Update();

				for (auto& world : worlds)
				{
					batchedAmount = std::size_t(ceil(world->worldObjects.size() / float(batchesCount)));

					// Game logic: Update calls and LateUpdate calls
					ThreadLoop2(world);

					// Work along and wait until tasks are done
					MainWorker(world);

					// do some networking
					world->GetNetworkSystem().UpdateAsync();

					// update matrices
					ThreadLoop1(world);

					// Work along and wait until tasks are done
					MainWorker(world);
				}

				Diagnostics::frameTime = std::chrono::duration<float>(std::chrono::high_resolution_clock::now() - currentTime).count();

				// Render scene
				view->Render();

				//auto endFrameTime = std::chrono::high_resolution_clock::now();
				//currentTime = endFrameTime;

				// FPS Cap
				if (GameEngine::enabledFpsCap)
				{
#ifdef WIN32
					FD_ZERO(&dummy);
					FD_SET(s, &dummy);
					long frametime = long(std::chrono::duration_cast<std::chrono::microseconds>((currentTime + GameEngine::targetSleepTime) - std::chrono::high_resolution_clock::now()).count());
					tv.tv_usec = frametime;
					select(0, 0, 0, &dummy, &tv);
#else
					std::this_thread::sleep_until(currentTime + targetSleepTime);
#endif
				}
			}
		}
		/*catch (std::exception& e)
		{
			Debug::Log("GameEngine Exception: " + std::string(e.what()));

		}*/

#ifdef WIN32
		closesocket(s);
		WSACleanup();
#endif

		for(auto& world : worlds)
			world->GetNetworkSystem().Stop();

		if (this->scene)
			this->scene->UnloadScene();
	}

	void GameEngine::Worker()
	{
		std::function<void()> function;

		while (true)
		{

			{
				std::unique_lock<std::mutex> lock(taskLock);
				taskWaiter.wait(lock, [this] { return !GameEngine::running || !tasks.empty(); });

				if (!GameEngine::running)
					break;

				workersActive++;

				function = tasks.front().first;
				tasks.pop_front();
			}

			try
			{
				function();
			}
			catch (std::exception& e)
			{
				Debug::Log("GameEngine::Worker Exception: " + std::string(e.what()));
			}

			workersActive--;
		}
	}

	void GameEngine::MainWorker(cgc::raw_ptr<World> world)
	{
		std::function<void()> function;

		while (!tasks.empty())
		{

			{
				std::unique_lock<std::mutex> lock(taskLock);
				if (!GameEngine::running || tasks.empty())
					break;

				workersActive++;

				function = tasks.front().first;
				tasks.pop_front();
			}

			try
			{
				function();
			}
			catch (std::exception& e)
			{
				Debug::Log("GameEngine::Worker Exception: " + std::string(e.what()));
			}

			workersActive--;
		}

		// SYNC
		while (workersActive > 0)
			std::this_thread::yield();
	}

	cgc::strong_ptr<World> GameEngine::CreateWorld()
	{
		return cgc::construct_new<World>(false, false);
	}

	void GameEngine::ThreadLoop1(cgc::raw_ptr<World> world)
	{
		{
			/*std::unique_lock<std::mutex> lock(taskLock);
			IWorldObject* worldObject;
			while (world->dirtyWorldObjects.pop(worldObject))
				tasks.emplace_back(std::bind(&IWorldObject::DirtyCleanUp, worldObject));

			tasks.emplace_back(std::bind(&world::DirtyCleanUp, world));
			tasks.emplace_back(std::bind(&WorldController::DirtyCleanUp, world));*/
			
			bool test = true;

			IWorldObject* worldObject;
			while (world->dirtyWorldObjects.pop(worldObject))
			{
				worldObject->DirtyCleanUp();
				test = false;
			}
		}

		//taskWaiter.notify_all();
	}

	void GameEngine::ThreadLoop2(cgc::raw_ptr<World> world)
	{
		/*{
			std::unique_lock<std::mutex> lock(taskLock);

			// non concurrent test
			world->GetPhysicsEngine()->UpdateWorld(Time::deltaTime);

			world->UpdateThreadTasks(tasks, threadCount);

			for (size_t i = 0; i < batchesCount; ++i)
				tasks.emplace_back(std::bind(&GameEngine::WorldObjectsUpdate, this, i * batchedAmount, (i + 1) * batchedAmount));

			// append new task giver
			tasks.emplace_back(std::bind(&GameEngine::ThreadLoop3, this));
		}

		taskWaiter.notify_all();*/

		auto& entitiesVector = world->entities.get_arrays();

		for (std::size_t v = 0; v < entitiesVector.size(); ++v)
		{
			auto& entities = *entitiesVector[v];
			for (auto& entity : entities)
				entity.Update();
		}

		auto& worldObjects = world->worldObjects;
		for (size_t i = 0; i < worldObjects.size(); ++i)
			worldObjects[i]->Update();


		world->DirtyCleanUp();

		world->Update();

		world->Physics().UpdateWorld(Time::deltaTime);

		this->ThreadLoop3(world);
	}

	void GameEngine::ThreadLoop3(cgc::raw_ptr<World> world)
	{
		// SYNC: 2
		/*while (!tasks.empty() || workersActive > 1)
			std::this_thread::yield();

		{
			std::unique_lock<std::mutex> lock(taskLock);
			for (size_t i = 0; i < batchesCount; ++i)
				tasks.emplace_back(std::bind(&GameEngine::WorldObjectsLateUpdate, this, i * batchedAmount, (i + 1) * batchedAmount));
		}

		taskWaiter.notify_all();*/

		for (auto& entity : world->entities)
			entity.LateUpdate();

		for (auto& vector : world->entities.get_arrays())
		{
			for (auto& entity : *vector)
				entity.LateUpdate();
		}

		for (size_t i = 0; i < world->worldObjects.size(); ++i)
			world->worldObjects[i]->LateUpdate();


		world->LateUpdate();
	}

	void GameEngine::WorldObjectsUpdate(cgc::raw_ptr<World> world, size_t from, size_t to)
	{

		for (auto& vector : world->entities.get_arrays())
		{
			for (auto& entity : *vector)
				entity.Update();
		}

		for (size_t i = from; i < to && i < world->worldObjects.size(); ++i)
			world->worldObjects[i]->Update();
	}

	void GameEngine::WorldObjectsLateUpdate(cgc::raw_ptr<World> world, size_t from, size_t to)
	{
		for (auto& worldObject : world->worldObjects)
			worldObject->LateUpdate();
	}


	void GameEngine::SetScene(const cgc::strong_ptr<Scene>& scene)
	{
		if (scene)
			this->newScene = scene;
	}

	void GameEngine::SetTargetFPS(int limit)
	{
		GameEngine::enabledFpsCap = true;
		targetSleepTime = std::chrono::microseconds((long)(1000000.f / float(limit)));
		GameEngine::targetFps = (float)limit;
	}

	void GameEngine::EnableFPSLimit(bool state)
	{
		enabledFpsCap = state;
	}
	/// \brief
	void GameEngine::QuitEngine()
	{
		running = false;
	}

}