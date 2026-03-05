#pragma once
using namespace std;

class GuiMessage
{
public:
	wstring string;
	int ticks;
	GuiMessage(const wstring& string);
};