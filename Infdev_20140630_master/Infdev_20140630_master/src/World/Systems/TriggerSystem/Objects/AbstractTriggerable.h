/*#ifndef ESTEEM_DATA_WORLD_ABSTRACT_TRIGGERABLE_H
#define ESTEEM_DATA_WORLD_ABSTRACT_TRIGGERABLE_H

#include <unordered_map>
#include "./TriggerEvent.h"

namespace Esteem
{
	class AbstractTriggerable
	{
	friend class TriggerSystem;
	private:
		TriggerSystem* triggerSystem;
		std::unordered_map<std::string, TriggerEvent*> triggerEvents;

	protected:
		AbstractTriggerable(std::string name, TriggerSystem& triggerSystem);

		virtual void InitializeEvents();

	public:
		void AddEvent(std::string command, TriggerEvent& triggerEvent);
		void RemoveEvent(std::string command);
		void RemoveEvent(TriggerEvent* triggerEvent);

		virtual ~AbstractTriggerable();
	};
}

#endif*/