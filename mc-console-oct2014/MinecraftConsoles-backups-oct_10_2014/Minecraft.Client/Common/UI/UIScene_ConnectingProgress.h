#pragma once

#include "UIScene.h"

class UIScene_ConnectingProgress : public UIScene
{
private:
	bool m_runFailTimer;
	int m_timerTime;
	bool m_showTooltips;
	bool m_removeLocalPlayer;
	bool m_showingButton;
	void (*m_cancelFunc)(LPVOID param);
	LPVOID m_cancelFuncParam;

	enum EControls
	{
		eControl_Confirm
	};

protected:
	UIControl_Progress m_progressBar;
	UIControl_Label m_labelTitle, m_labelTip;
	UIControl_Button m_buttonConfirm;
	UIControl m_controlTimer;
	UI_BEGIN_MAP_ELEMENTS_AND_NAMES(UIScene)		
		UI_MAP_ELEMENT( m_progressBar, "ProgressBar")
		UI_MAP_ELEMENT( m_labelTitle, "Title")
		UI_MAP_ELEMENT( m_labelTip, "Tip")
		UI_MAP_ELEMENT( m_buttonConfirm, "Confirm")
		UI_MAP_ELEMENT( m_controlTimer, "Timer")
	UI_END_MAP_ELEMENTS_AND_NAMES()
public:
	UIScene_ConnectingProgress(int iPad, void *initData, UILayer *parentLayer);
	virtual ~UIScene_ConnectingProgress();

	virtual void tick();

	virtual EUIScene getSceneType() { return eUIScene_ConnectingProgress;}
	
	virtual void updateTooltips();
	virtual void handleGainFocus(bool navBack);
	virtual void handleLoseFocus();

	void handleTimerComplete(int id);

protected:
	// TODO: This should be pure virtual in this class
	virtual wstring getMoviePath();

#ifdef _DURANGO	
	virtual long long getDefaultGtcButtons() { return 0; }
#endif

public:
	// INPUT
	virtual void handleInput(int iPad, int key, bool repeat, bool pressed, bool released, bool &handled);

protected:
	void handlePress(F64 controlId, F64 childId);
};
