#pragma once
#include "Screen.h"
class EditBox;
class LevelStorageSource;
using namespace std;

class CreateWorldScreen : public Screen
{
private:
	Screen *lastScreen;
    EditBox *nameEdit;
    EditBox *seedEdit;
    wstring resultFolder;
    bool done;

public:
	CreateWorldScreen(Screen *lastScreen);
    virtual void tick();
    virtual void init();
private:
	void updateResultFolder();
public:
	static wstring findAvailableFolderName(LevelStorageSource *levelSource, const wstring& folder);
    virtual void removed();
protected:
	virtual void buttonClicked(Button *button);
    virtual void keyPressed(wchar_t ch, int eventKey);
    virtual void mouseClicked(int x, int y, int buttonNum);
public:
	virtual void render(int xm, int ym, float a);
    virtual void tabPressed();
};