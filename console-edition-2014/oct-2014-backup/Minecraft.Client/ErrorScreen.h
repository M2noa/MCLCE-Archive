#pragma once
#include "Screen.h"

class ErrorScreen : public Screen
{
private:
	wstring title, message;
public:
	ErrorScreen(const wstring& title, const wstring& message);
    virtual void init();
    virtual void render(int xm, int ym, float a);
protected:
	virtual void keyPressed(wchar_t eventCharacter, int eventKey);
};