#pragma once
#include "Screen.h"
class Button;
class EditBox;
using namespace std;

class RenameWorldScreen : public Screen
{
private:
	Screen *lastScreen;
    EditBox *nameEdit;
    wstring levelId;

public:
	RenameWorldScreen(Screen *lastScreen, const wstring& levelId);
    virtual void tick();
    virtual void init() ;
    virtual void removed();
protected:
	virtual void buttonClicked(Button *button);
    virtual void keyPressed(wchar_t ch, int eventKey);
    virtual void mouseClicked(int x, int y, int buttonNum);
public:
	virtual void render(int xm, int ym, float a);
};