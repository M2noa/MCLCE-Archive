#include "Time.h"

namespace Esteem
{
	Time::microseconds Time::frameTime = {};
	float Time::deltaTime = 0.f;
	
	std::chrono::steady_clock::time_point Time::startupTime = std::chrono::steady_clock::now();
	Time::milliseconds Time::startupTimeMs = Time::milliseconds(GetTimeInMs());
}