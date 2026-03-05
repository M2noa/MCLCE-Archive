#pragma once
#include "Screen.h"
using namespace std;

class DisconnectedScreen : public Screen
{
private:
	wstring title, reason;

public:
	DisconnectedScreen(const wstring& title, const wstring reason, void *reasonObjects, ...);
    virtual void tick();
protected:
	using Screen::keyPressed;

	virtual void keyPressed(char eventCharacter, int eventKey);
public:
	virtual void init();
protected:
	virtual void buttonClicked(Button *button);
public:
	virtual void render(int xm, int ym, float a);
};
