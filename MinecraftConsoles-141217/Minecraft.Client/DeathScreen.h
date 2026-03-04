#pragma once
#include "Screen.h"

class DeathScreen : public Screen
{
public:
	virtual void init();
protected:
	virtual void keyPressed(char eventCharacter, int eventKey);
    virtual void buttonClicked(Button *button);
public:
	virtual void render(int xm, int ym, float a);
    virtual bool isPauseScreen();
};