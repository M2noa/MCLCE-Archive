#pragma once

#include <unordered_map>
#include <forward_list>
#include "World/Systems/TriggerSystem/Objects/TriggerCommand.h"
#include "World/Systems/TriggerSystem/Objects/TriggerEvent.h"

namespace Esteem
{
	class TriggerSystem
	{
	private:
		std::unordered_map<std::string, std::vector<TriggerEvent*>> triggerEvents;
		std::unordered_map<std::size_t, TriggerCommandSerie> triggerSeries;
		std::forward_list<std::size_t> triggerList;

		void InsertTriggerInList(std::size_t itSerie);

	public:
		void Update();

		void AddEvent(TriggerEvent& triggerable);
		void RemoveEvent(TriggerEvent* triggerable);

		void TriggerNow(TriggerCommandSerie& serie);
		void TriggerDelayed(const TriggerCommandSerie& serie, std::chrono::microseconds delay);
		void TriggerRepeater(const TriggerCommandSerie& serie, uint repeatAmount, std::chrono::microseconds repeatDelay, std::chrono::microseconds delay);

		// TODO: insert threading
	};
}