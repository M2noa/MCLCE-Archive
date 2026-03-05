#pragma once
#include "ConsoleInputSource.h"
using namespace std;

class ConsoleInput
{
public:
	wstring msg;
	ConsoleInputSource *source;

	ConsoleInput(const wstring& msg, ConsoleInputSource *source);
};