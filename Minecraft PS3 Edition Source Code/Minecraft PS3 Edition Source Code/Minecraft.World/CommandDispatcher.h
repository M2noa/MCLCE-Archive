#pragma once

class Command;
class CommandSender;

class CommandDispatcher
{
private:
	unordered_map<EGameCommand, Command *> commandsById;
	unordered_set<Command *> commands;

public:
	void performCommand(shared_ptr<CommandSender> sender, EGameCommand command, byteArray commandData);
	Command *addCommand(Command *command);
};