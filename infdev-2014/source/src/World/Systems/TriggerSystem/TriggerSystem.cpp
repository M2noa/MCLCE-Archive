#include "./TriggerSystem.h"
#include "Utils/Debug.h"
#include "Utils/Time.h"

// TODO: replace direct lookup to a system in which commands will do a lookup on construction
//		 for later use (on trigger)

namespace Esteem
{
	void TriggerSystem::Update()
	{
		TriggerCommandSerie* commandSerie;
		std::forward_list<std::size_t>::iterator it;
		while ((it = triggerList.begin()) != triggerList.end())
		{
			if ((commandSerie = &triggerSeries[*it])->triggerTime > Time::RenderTime())
				break;

			TriggerNow(*commandSerie);
			if (commandSerie->repeatAmount != -1)
				--commandSerie->repeatAmount;

			if (commandSerie->repeatAmount != 0)
			{
				commandSerie->triggerTime = Time::RenderTime() + commandSerie->repeatTime;
				InsertTriggerInList(*it);
			}
			else
				triggerSeries.erase(*it);

			triggerList.pop_front();
		}
	}

	void TriggerSystem::AddEvent(TriggerEvent& triggerEvent)
	{
		triggerEvents[triggerEvent.GetCommand()].push_back(&triggerEvent);
	}

	void TriggerSystem::RemoveEvent(TriggerEvent* triggerEvent)
	{
		auto foundCommandList = triggerEvents.find(triggerEvent->GetCommand());
		if (foundCommandList != triggerEvents.end())
		{
			auto found = std::find(foundCommandList->second.begin(), foundCommandList->second.end(), triggerEvent);
			if (found != foundCommandList->second.end())
				foundCommandList->second.erase(found);
		}
	}

	void TriggerSystem::TriggerNow(TriggerCommandSerie& serie)
	{
		// TODO: thread safety?
		serie.triggerTime = Time::RenderTime();
		auto commands = serie.commands;
		for (uint commandIndex = 0; commandIndex < commands.size(); ++commandIndex)
		{
			auto found = triggerEvents.find(commands[commandIndex].command);
			if (found != triggerEvents.end())
			{
				for (uint eventIndex = 0; eventIndex < found->second.size(); ++eventIndex)
					found->second[eventIndex]->Getdelegate()(&commands[commandIndex]);
			}
		}
	}

	void TriggerSystem::TriggerDelayed(const TriggerCommandSerie& serie, std::chrono::microseconds delay)
	{
		std::size_t address = std::size_t(&serie);
		while (triggerSeries.find(address) != triggerSeries.end())
			++address;

		triggerSeries[address] = serie;
		triggerSeries[address].triggerTime = Time::RenderTime() + delay;

		InsertTriggerInList(address);
	}

	void TriggerSystem::TriggerRepeater(const TriggerCommandSerie& serie, uint repeatAmount, std::chrono::microseconds repeatDelay, std::chrono::microseconds delay)
	{
		std::size_t address = std::size_t(&serie);
		while (triggerSeries.find(address) != triggerSeries.end())
			++address;

		triggerSeries[address] = serie;
		triggerSeries[address];

		triggerSeries[address].triggerTime = Time::RenderTime() + delay;
		triggerSeries[address].repeatAmount = repeatAmount;
		triggerSeries[address].repeatTime = repeatDelay;

		InsertTriggerInList(address);
	}

	void TriggerSystem::InsertTriggerInList(std::size_t itSerie)
	{
		auto it = triggerList.begin();
		if (it == triggerList.end() || triggerSeries[*it].triggerTime > triggerSeries[itSerie].triggerTime)
			triggerList.push_front(itSerie);
		else
		{
			auto lastIt = it++;
			for (; it != triggerList.end(); lastIt = it++)
			{
				if (triggerSeries[*it].triggerTime > triggerSeries[itSerie].triggerTime)
					break;
			}

			triggerList.insert_after(lastIt, itSerie);
		}
	}
}