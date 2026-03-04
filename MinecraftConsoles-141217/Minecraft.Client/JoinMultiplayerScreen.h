#pragma once
#include "Screen.h"
class EditBox;
class Button;

class JoinMultiplayerScreen : public Screen
{
private:
	Screen *lastScreen;
    EditBox *ipEdit;

public:
	JoinMultiplayerScreen(Screen *lastScreen);
    virtual void tick();
    virtual void init();
    virtual void removed();
protected:
	virtual void buttonClicked(Button *button);
private:
	virtual int parseInt(const wstring& str, int def);
protected:
	virtual void keyPressed(wchar_t ch, int eventKey);
    virtual void mouseClicked(int x, int y, int buttonNum);
public:
	virtual void render(int xm, int ym, float a);
};