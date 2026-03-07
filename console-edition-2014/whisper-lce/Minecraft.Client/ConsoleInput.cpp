#include "stdafx.h"
#include "ConsoleInput.h"

ConsoleInput::ConsoleInput(const wstring& msg, ConsoleInputSource *source)
{
	this->msg = msg;
	this->source = source;
}