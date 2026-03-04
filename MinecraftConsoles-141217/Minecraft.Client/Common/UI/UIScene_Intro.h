#pragma once

#include "UIScene.h"

class UIScene_Intro : public UIScene
{
private:
	bool m_bIgnoreNavigate;
	bool m_bAnimationEnded;

	IggyName m_funcSetIntroPlatform;
#ifdef __PSVITA__
	UIControl_Touch m_TouchToSkip;
#endif
	UI_BEGIN_MAP_ELEMENTS_AND_NAMES(UIScene)
#ifdef __PSVITA__
		UI_MAP_ELEMENT( m_TouchToSkip, "TouchToSkip" )
#endif
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

#ifdef _DURANGO	
	virtual long long getDefaultGtcButtons() { return 0; }
#endif

public:
	// INPUT
	virtual void handleInput(int iPad, int key, bool repeat, bool pressed, bool released, bool &handled);

	virtual void handleAnimationEnd();
	virtual void handleGainFocus(bool navBack);

#ifdef __PSVITA__
	virtual void handleTouchInput(unsigned int iPad, S32 x, S32 y, int iId, bool bPressed, bool bRepeat, bool bReleased);
#endif

};
