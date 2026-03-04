#pragma once

#include <chrono>
#include <functional>

namespace Esteem
{
	struct DelayedAction
	{
		std::chrono::microseconds timeToCall;
		std::function<void()> function;

		DelayedAction(std::chrono::microseconds timeToCall, std::function<void()> function)
			: timeToCall(timeToCall)
			, function(function)
		{

		}
	};
}