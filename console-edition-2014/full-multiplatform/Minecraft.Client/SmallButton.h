#pragma once
#include "Button.h"
#include "Options.h"

class SmallButton : public Button
{
private:
	const Options::Option *option;

public:
	SmallButton(int id, int x, int y, const wstring& msg);
    SmallButton(int id, int x, int y, int width, int height, const wstring& msg);
    SmallButton(int id, int x, int y, const Options::Option *item, const wstring& msg);
    const Options::Option *getOption();
};