#pragma once

#include "ChatScreen.h"

class InBedChatScreen : public ChatScreen
{
private:
	static const int WAKE_UP_BUTTON = 1;
public:
	virtual void init();
    virtual void removed();
protected:
	virtual void keyPressed(wchar_t ch, int eventKey);
public:
	virtual void render(int xm, int ym, float a);
protected:
	virtual void buttonClicked(Button *button);
private:
	void sendWakeUp();
};