#pragma once

#include <vector>
#include <string>
#include <map>

#include "General/Delegate.h"
#include "General/Settings.h"

#include "Utils/Hash.h"

namespace Esteem
{
	typedef delegate<void(const std::string&, const std::string&)> CommandDelegate;
	typedef delegate<void(const std::string&)> NewCommandDelegate;

	class Command;

	class CommandHolder
	{
		friend class Command;
	private:
		cppu::hash_t hash;
		const CommandDelegate* delegate;

		CommandHolder(cppu::hash_t hash, const CommandDelegate& delegate);

	public:
		CommandHolder() = default;
		~CommandHolder();
	};

	class NewCommandHolder
	{
		friend class Command;
	private:
		const NewCommandDelegate* delegate;

		NewCommandHolder(const NewCommandDelegate& delegate);

	public:
		NewCommandHolder() = default;
		~NewCommandHolder();
	};

	class Command
	{
	private:
		static std::map<hash_t, std::vector<CommandDelegate>> listeners;
		static std::vector<NewCommandDelegate> newCommandListeners;

	public:
		static void RegisterListener(const std::string& command, const CommandDelegate& listener, CommandHolder* holder = nullptr);
		static void UnRegisterListener(const std::string& command, const CommandDelegate& listener);
		static void UnRegisterListener(cppu::hash_t command, const CommandDelegate& listener);

		static void RegisterListener(const NewCommandDelegate& listener, NewCommandHolder* holder = nullptr);
		static void UnRegisterListener(const NewCommandDelegate& listener);

		static void ExecuteCommand(const std::string& command, const std::string& value);
	};
}
