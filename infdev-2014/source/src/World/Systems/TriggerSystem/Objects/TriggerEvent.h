#pragma once

#include "stdafx.h"

#include "General/Delegate.h"
#include "./TriggerCommand.h"
#include <string>
#include <vector>
#include <functional>

namespace Esteem
{
	typedef delegate<void(const TriggerCommand*)> TriggerEventdelegate;

	class TriggerEvent
	{
	private:
		TriggerEventdelegate func;
		TriggerValueType type;
		std::string command;

	public:
		TriggerEvent(const std::string command, TriggerValueType type, TriggerEventdelegate func, std::string privateListenName = "");

		inline TriggerEventdelegate Getdelegate() { return func; }

		const std::string& GetCommand() const { return command; }
	};
}