#include "Debug.h"

namespace Esteem
{
	std::chrono::high_resolution_clock::time_point Debug::startHighResolutionClock = std::chrono::high_resolution_clock::time_point();
	std::mutex Debug::outLock;
}