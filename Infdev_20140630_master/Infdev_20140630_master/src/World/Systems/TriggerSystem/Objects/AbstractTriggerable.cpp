/*#include "./AbstractTriggerable.h"
#include "Utils/Debug.h"
#include "World/Systems/TriggerSystem/TriggerSystem.h"

namespace Esteem
{
	AbstractTriggerable::AbstractTriggerable(std::string name, TriggerSystem& triggerSystem)
		: triggerSystem(&triggerSystem)
	{
		triggerSystem.AddTriggerable(name, this);
		InitializeEvents();
	}

	AbstractTriggerable::~AbstractTriggerable()
	{
		triggerSystem->RemoveTriggerable(this);
		for (auto it = triggerEvents.begin(); it != triggerEvents.end(); it++)
			delete (it++)->second;
	}

	void AbstractTriggerable::InitializeEvents()
	{

	}

	void AbstractTriggerable::AddEvent(std::string command, TriggerEvent& triggerEvent)
	{
		auto found = triggerEvents.find(command);
		if (found == triggerEvents.end())
			triggerEvents[command] = &triggerEvent;
		else
			Debug::LogWarning("AbstractTriggerable::AddEvent: Could not overwrite: '" + command + ", it already exist.");
	}

	void AbstractTriggerable::RemoveEvent(std::string command)
	{
		auto found = triggerEvents.find(command);
		if (found != triggerEvents.end())
		{
			delete found->second;
			triggerEvents.erase(found);

		}
	}

	void AbstractTriggerable::RemoveEvent(TriggerEvent* triggerEvent)
	{
		auto found = std::find_if(triggerEvents.begin(), triggerEvents.end(),
			[triggerEvent](const std::unordered_map<std::string, TriggerEvent*>::value_type& e)
			-> bool { return e.second == triggerEvent; });

		if (found != triggerEvents.end())
		{
			delete found->second;
			triggerEvents.erase(found);
		}
	}
}*/