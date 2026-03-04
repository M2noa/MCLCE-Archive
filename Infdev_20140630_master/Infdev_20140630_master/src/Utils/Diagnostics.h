#pragma once
#include "stdafx.h"
#include <chrono>

#include "./Time.h"

namespace Esteem
{
	class Diagnostics
	{
	public:
		static float frameTime;
		static float renderTime;
		static float physicsTime;
		static uint drawCalls;
	};
}
