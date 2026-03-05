#pragma once
#include "Screen.h"

class PauseScreen : public Screen
{
private:
	int saveStep;
    int visibleTime;
public:
	PauseScreen();  // 4J added
	virtual void init();
protected:using Screen::buttonClicked;

	virtual void buttonClicked(Button button);
public:
	virtual void tick();
    virtual void render(int xm, int ym, float a);
};
