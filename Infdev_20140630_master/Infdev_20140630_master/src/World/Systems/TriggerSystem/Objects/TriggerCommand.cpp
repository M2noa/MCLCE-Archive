#include "./TriggerCommand.h"

#include <cstring>

namespace Esteem
{
	TriggerCommand::TriggerCommand(const std::string command, const void* value, const int size)
		: command(command)
	{
		this->value.resize(size);
		memcpy(&this->value[0], value, size);
	}

	TriggerCommand TriggerCommand::CreateTriggerCommand(const std::string command, byte value)
	{
		TriggerCommand tCommand = TriggerCommand(command, &value, sizeof(byte));
		tCommand.type = TriggerValueType::BYTE;
		return tCommand;
	}

	TriggerCommand TriggerCommand::CreateTriggerCommand(const std::string command, int value)
	{
		TriggerCommand tCommand = TriggerCommand(command, &value, sizeof(int));
		tCommand.type = TriggerValueType::INT;
		return tCommand;
	}

	TriggerCommand TriggerCommand::CreateTriggerCommand(const std::string command, uint value)
	{
		TriggerCommand tCommand = TriggerCommand(command, &value, sizeof(uint));
		tCommand.type = TriggerValueType::UINT;
		return tCommand;
	}

	TriggerCommand TriggerCommand::CreateTriggerCommand(const std::string command, float value)
	{
		TriggerCommand tCommand = TriggerCommand(command, &value, sizeof(float));
		tCommand.type = TriggerValueType::FLOAT;
		return tCommand;
	}

	TriggerCommand TriggerCommand::CreateTriggerCommand(const std::string command, double value)
	{
		TriggerCommand tCommand = TriggerCommand(command, &value, sizeof(double));
		tCommand.type = TriggerValueType::DOUBLE;
		return tCommand;
	}

	TriggerCommand TriggerCommand::CreateTriggerCommand(const std::string command, const std::string& value)
	{
		TriggerCommand tCommand = TriggerCommand(command, &value, sizeof(std::string));
		tCommand.type = TriggerValueType::STRING;
		return tCommand;
	}

	TriggerCommand TriggerCommand::CreateTriggerCommand(const std::string command, int64 value)
	{
		TriggerCommand tCommand = TriggerCommand(command, &value, sizeof(int64));
		tCommand.type = TriggerValueType::INT64;
		return tCommand;
	}

	void TriggerCommand::Trigger()
	{
		//auto func = triggerEvent->Getdelegate();
		//func(this);

		//auto testFunc = DELEGATE(&TriggerCommand::CreateTriggerCommand, this);

	}
}