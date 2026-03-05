#include "./TriggerEvent.h"

namespace Esteem
{
	TriggerEvent::TriggerEvent(const std::string command, TriggerValueType type, TriggerEventdelegate func, std::string privateListenName)
		: command(command), type(type), func(func)
	{
		if (privateListenName.size() > 0)
			this->command = privateListenName + "." + command;
	};
}