#pragma once

#include "stdafx.h"
#include <chrono>

#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
#include <sysinfoapi.h>

namespace
{
	inline std::chrono::milliseconds GetTimeInMs()
	{
		if constexpr (sizeof(size_t) == 4)
			return std::chrono::milliseconds(GetTickCount());
		else
			return std::chrono::milliseconds(GetTickCount64());
	}
}

#endif


namespace Esteem
{
	class GameEngine;

	class Time
	{
		friend class GameEngine;

	public:
		typedef std::chrono::microseconds microseconds;
		typedef std::chrono::milliseconds milliseconds;

	private:
		static microseconds frameTime;
		static float deltaTime;
		static float simulationDeltaTime;

		static std::chrono::steady_clock::time_point startupTime;
#ifdef _WIN32
		static milliseconds startupTimeMs;
#endif

		static void ResetStartupTime();
		static void SetRenderTime(const microseconds& timePoint);

	public:
		static milliseconds TimeSinceStartup();
		static microseconds RenderTime();

		static float RenderDeltaTime();

		static float SimulationDeltaTime();
		static float SimDeltaTime();
	};

	inline void Time::ResetStartupTime()
	{
			startupTime = std::chrono::steady_clock::now();
#ifdef _WIN32
			startupTimeMs = milliseconds(GetTimeInMs());
#endif
	}

	inline Time::milliseconds Time::TimeSinceStartup()
	{
#ifdef _WIN32
		return milliseconds(GetTimeInMs()) - startupTimeMs;
#else
		return DurationSinceStartup().count();
#endif
	}

	inline void Time::SetRenderTime(const microseconds& timePoint)
	{
		frameTime = timePoint;
	}

	inline Time::microseconds Time::RenderTime()
	{
		return frameTime;
	}

	inline float Time::RenderDeltaTime()
	{
		return deltaTime;
	}

	inline float Time::SimulationDeltaTime()
	{
		return simulationDeltaTime;
	}

	inline float Time::SimDeltaTime()
	{
		return SimulationDeltaTime();
	}
}