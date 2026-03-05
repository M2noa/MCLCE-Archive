#pragma once
#include "Screen.h"
using namespace std;
class Options;

class ControlsScreen : public Screen
{
private:
	Screen *lastScreen;
protected:
	wstring title;
private:
	Options *options;
    
	int selectedKey;

    static const int BUTTON_WIDTH = 70;
    static const int ROW_WIDTH = 160;

public:
	ControlsScreen(Screen *lastScreen, Options *options);
private:
	int getLeftScreenPosition();
public:
	void init();
protected:
	void buttonClicked(Button *button);
    void keyPressed(wchar_t eventCharacter, int eventKey);
public:
	void render(int xm, int ym, float a);
};