#include "stdafx.h"
#include "GuiMessage.h"

GuiMessage::GuiMessage(const wstring& string)
{
	this->string = string;
	ticks = 0;
}