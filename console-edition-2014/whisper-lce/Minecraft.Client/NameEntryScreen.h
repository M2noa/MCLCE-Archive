#pragma once
#include "Screen.h"

class NameEntryScreen : public Screen
{
private:
	Screen *lastScreen;
protected:
	wstring title;
private:
	int slot;
    wstring name;
    int frame;
public:
	NameEntryScreen(Screen *lastScreen, const wstring& oldName, int slot);
    virtual void init();
    virtual void removed();
    virtual void tick();
protected:
	virtual void buttonClicked(Button button);
private:
	static const wstring allowedChars;
protected:
	virtual void keyPressed(wchar_t ch, int eventKey);
public:
	virtual void render(int xm, int ym, float a);
};