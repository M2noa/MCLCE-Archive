#pragma once

#include "UIScene.h"

class UIScene_SaveMessage : public UIScene
{
private:
	enum EControls
	{
		eControl_Confirm,
	};

	bool m_bIgnoreInput;

	UIControl_Button m_buttonConfirm;
	UIControl_Label m_labelDescription;
	IggyName m_funcAutoResize;
	UI_BEGIN_MAP_ELEMENTS_AND_NAMES(UIScene)
		UI_MAP_ELEMENT(m_buttonConfirm, "Confirm")
		UI_MAP_ELEMENT(m_labelDescription, "Description")
		UI_MAP_NAME( m_funcAutoResize, L"AutoResize")
	UI_END_MAP_ELEMENTS_AND_NAMES()

#if defined(__PS3__) || defined(__ORBIS__) || defined(__PSVITA__)
	static int DeleteOptionsDialogReturned(void *pParam,int iPad,C4JStorage::EMessageResult result);
#endif

public:
	UIScene_SaveMessage(int iPad, void *initData, UILayer *parentLayer);
	~UIScene_SaveMessage();

	virtual EUIScene getSceneType() { return eUIScene_SaveMessage;}
	// Returns true if this scene has focus for the pad passed in
#ifndef __PS3__
	virtual bool hasFocus(int iPad) { return bHasFocus; }
#endif
	virtual void updateTooltips();

protected:
	virtual wstring getMoviePath();

public:
	// INPUT
	virtual void handleInput(int iPad, int key, bool repeat, bool pressed, bool released, bool &handled);
	virtual void handleTimerComplete(int id);

protected:
	void handlePress(F64 controlId, F64 childId);

	virtual long long getDefaultGtcButtons() { return 0; }
};
