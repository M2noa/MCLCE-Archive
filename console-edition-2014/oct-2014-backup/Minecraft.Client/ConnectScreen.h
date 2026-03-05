#pragma once
#include "Screen.h"
class ClientConnection;
class Minecraft;

using namespace std;

class ConnectScreen : public Screen
{
private:
	ClientConnection *connection;
    bool aborted;
public:
	ConnectScreen(Minecraft *minecraft, const wstring& ip, int port);
    virtual void tick();
protected:
	virtual void keyPressed(char eventCharacter, int eventKey);
public:
	virtual void init();
protected:
	virtual void buttonClicked(Button *button);
public:
	virtual void render(int xm, int ym, float a);
};