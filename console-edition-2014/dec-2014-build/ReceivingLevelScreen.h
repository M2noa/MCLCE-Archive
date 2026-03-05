#pragma once
#include "Screen.h"
class ClientConnection;

class ReceivingLevelScreen : public Screen
{
private:
	ClientConnection *connection;
    int tickCount;

public:
	ReceivingLevelScreen(ClientConnection *connection);
protected:
	using Screen::keyPressed;

	virtual void keyPressed(char eventCharacter, int eventKey);
public:
	virtual void init();
    virtual void tick();
protected:
	virtual void buttonClicked(Button *button);
public:
	virtual void render(int xm, int ym, float a);
};
