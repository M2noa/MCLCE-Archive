#pragma once

#include "UIScene.h"

class UIScene_Intro : public UIScene
{
private:
	bool m_bIgnoreNavigate;
	bool m_bAnimationEnded;

	IggyName m_funcSetIntroPlatform;
	UI_BEGIN_MAP_ELEMENTS_AND_NAMES(UIScene)
		UI_MAP_NAME( m_funcSetIntroPlatform, L"SetIntroPlatform")
	UI_END_MAP_ELEMENTS_AND_NAMES()

public:
	UIScene_Intro(int iPad, void *initData, UILayer *parentLayer);

	virtual EUIScene getSceneType() { return eUIScene_Intro;}

	// Returns true if this scene has focus for the pad passed in
#ifndef __PS3__
	virtual bool hasFocus(int iPad) { return bHasFocus; }
#endif

protected:


	virtual wstring getMoviePath();


public:
	// INPUT
	virtual void handleInput(int iPad, int key, bool repeat, bool pressed, bool released, bool &handled);

	virtual void handleAnimationEnd();
	virtual void handleGainFocus(bool navBack);


};
