#include "Command.h"

#include <algorithm>

namespace Esteem
{

	std::map<hash_t, std::vector<CommandDelegate>> Command::listeners;
	std::vector<NewCommandDelegate> Command::newCommandListeners;
	
	void Command::RegisterListener(const NewCommandDelegate& listener, NewCommandHolder* holder)
	{
		auto& delegate = newCommandListeners.emplace_back(listener);
		if (holder)
			holder->delegate = &delegate;
	}
	
	void Command::UnRegisterListener(const NewCommandDelegate& listener)
	{
		newCommandListeners.erase(std::find(newCommandListeners.begin(), newCommandListeners.end(), listener));
	}

	void Command::RegisterListener(const std::string& command, const CommandDelegate& listener, CommandHolder* holder)
	{
		hash_t hash = RT_HASH(command);
		if (listeners.count(hash) == 0)
		{
			for (size_t i = 0; i < newCommandListeners.size(); ++i)
				newCommandListeners[i](command);
		}

		auto& delegate = listeners[hash].emplace_back(listener);
		if (holder)
		{
			holder->hash = hash;
			holder->delegate = &delegate;
		}
	}

	void Command::UnRegisterListener(const std::string& command, const CommandDelegate& listener)
	{
		UnRegisterListener(RT_HASH(command), listener);
	}

	void Command::UnRegisterListener(cppu::hash_t command, const CommandDelegate& listener)
	{
		auto commandList = listeners.find(command);
		if (commandList != listeners.end())
		{
			std::vector<CommandDelegate>& listenersList = commandList->second;

			auto listenerFound = std::find(listenersList.begin(), listenersList.end(), listener);
			if (listenerFound != listenersList.end())
			{
				listenersList.erase(listenerFound);

				// remove command if non is present
				if (listenersList.empty())
					listeners.erase(commandList);
			}
		}
	}

	void Command::ExecuteCommand(const std::string& command, const std::string& value)
	{
		auto commandList = listeners.find(RT_HASH(command));
		if(commandList != listeners.end())
		{
			std::vector<CommandDelegate>& listenersList = commandList->second;
			for(size_t i = 0; i < listenersList.size(); ++i)
				listenersList[i](command, value);
		}
	}

	CommandHolder::CommandHolder(cppu::hash_t hash, const CommandDelegate& delegate)
		: hash(hash)
		, delegate(&delegate)
	{ }

	CommandHolder::~CommandHolder()
	{
		if (delegate)
			Command::UnRegisterListener(hash, *delegate);
	}

	NewCommandHolder::NewCommandHolder(const NewCommandDelegate& delegate)
		: delegate(&delegate)
	{ }

	NewCommandHolder::~NewCommandHolder()
	{
		if(delegate)
			Command::UnRegisterListener(*delegate);
	}

}
