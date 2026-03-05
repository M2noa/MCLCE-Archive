#pragma once
#include "Button.h"
#include "Options.h"
class Minecraft;

class SlideButton : public Button
{
public:
	float value;
    bool sliding;
private:
	const Options::Option *option;

public:
	SlideButton(int id, int x, int y, const Options::Option *option, const wstring& msg, float value);
protected:
	virtual int getYImage(bool hovered);
    virtual void renderBg(Minecraft *minecraft, int xm, int ym);
public:
	virtual bool clicked(Minecraft *minecraft, int mx, int my);
    virtual void released(int mx, int my);
};