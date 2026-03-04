#pragma once

#include "stdafx.h"

#include "General/Delegate.h"
#include <string>
#include <vector>
#include <chrono>
#include <functional>

namespace Esteem
{
	enum class TriggerValueType
	{
		UNKNOWN	= 0x0,

		BYTE	= 0x1,
		INT		= 0x2,
		UINT	= 0x3,
		FLOAT	= 0x4,
		DOUBLE	= 0x5,
		STRING	= 0x6,
		
		INT64	= 0x7
	};

	class TriggerEvent;

	class TriggerCommand
	{
	friend class TriggerSystem;
	private:
		/// \brief function which should be executed on trigger
		TriggerEvent* triggerEvent;

		TriggerCommand(const std::string command, const void* value, const int size);

	public:
		/// \brief stores the command
		std::string command;
		/// \brief type is saved as an enum
		TriggerValueType type;
		/// \brief value is saved as byte data
		std::vector<byte> value;
		
		static TriggerCommand CreateTriggerCommand(const std::string command, byte value);
		static TriggerCommand CreateTriggerCommand(const std::string command, int value);
		static TriggerCommand CreateTriggerCommand(const std::string command, uint value);
		static TriggerCommand CreateTriggerCommand(const std::string command, float value);
		static TriggerCommand CreateTriggerCommand(const std::string command, double value);
		static TriggerCommand CreateTriggerCommand(const std::string command, const std::string& value);
		static TriggerCommand CreateTriggerCommand(const std::string command, int64 value);

		template<typename ValueType>
		static TriggerCommand CreateTriggerCommand(const std::string command, ValueType value)
		{
			TriggerCommand tCommand = TriggerCommand(command, &value, sizeof(ValueType));
			tCommand.type = TriggerValueType::UNKNOWN;
			return tCommand;
		}

		/// \brief Returns the commands value by casting the value into the given <ValueType>
		template<typename ValueType>
		const ValueType GetValue() const { return *((ValueType*)&value[0]); }

		/// \brief Returns the commands value by reinterpeting the value directly into the given <ValueType>
		template<typename ValueType>
		const ValueType& GetValueUnsafe() const { return *reinterpret_cast<const ValueType*>(&value[0]); }

		void Trigger();
	};

	struct TriggerCommandSerie
	{
		std::chrono::microseconds triggerTime;
		uint repeatAmount;
		std::chrono::microseconds repeatTime;
		std::vector<TriggerCommand> commands;

		TriggerCommandSerie()
			: triggerTime(0)
			, repeatAmount(1)
			, repeatTime(0)
		{}

		TriggerCommandSerie(std::chrono::microseconds triggerTime, std::vector<TriggerCommand> commands)
			: triggerTime(triggerTime)
			, repeatAmount(1)
			, repeatTime()
			, commands(commands)
		{
		}

		TriggerCommandSerie(std::chrono::microseconds triggerTime, uint repeatAmount, std::chrono::microseconds repeatTime, std::vector<TriggerCommand> commands)
			: triggerTime(triggerTime)
			, repeatAmount(repeatAmount)
			, repeatTime(repeatTime)
			, commands(commands)
		{}
	};
}