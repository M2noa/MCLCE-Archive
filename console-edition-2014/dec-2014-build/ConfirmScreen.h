#pragma once
#include "Screen.h"
using namespace std;

class ConfirmScreen : public Screen
{
private:
	Screen *parent;
    wstring title1;
    wstring title2;
    wstring yesButton;
    wstring noButton;
    int id;

public:
	ConfirmScreen(Screen *parent, const wstring& title1, const wstring& title2, int id);
    ConfirmScreen(Screen *parent, const wstring& title1, const wstring& title2, const wstring& yesButton, const wstring& noButton, int id);
    virtual void init();
protected:
	virtual void buttonClicked(Button *button);
public:
	virtual void render(int xm, int ym, float a);
};