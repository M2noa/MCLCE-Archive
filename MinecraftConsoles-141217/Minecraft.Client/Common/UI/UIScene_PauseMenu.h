#pragma once

#include "UIScene.h"
#include "IUIScene_PauseMenu.h"

#define BUTTON_PAUSE_RESUMEGAME			0
#define BUTTON_PAUSE_HELPANDOPTIONS		1
#define BUTTON_PAUSE_LEADERBOARDS		2

#ifdef _XBOX_ONE
#define BUTTON_PAUSE_XBOXHELP			3
#else
#define BUTTON_PAUSE_ACHIEVEMENTS		3
#endif

#define BUTTON_PAUSE_SAVEGAME			4
#define	BUTTON_PAUSE_EXITGAME			5
#define BUTTONS_PAUSE_MAX				BUTTON_PAUSE_EXITGAME + 1

class UIScene_PauseMenu : public UIScene, public IUIScene_PauseMenu
{
private:
	bool m_savesDisabled;
	bool m_bTrialTexturePack;
	bool m_bErrorDialogRunning;

	enum eActions
	{
		eAction_None=0,
#if defined(__PS3__) || defined(__PSVITA__) || defined(__ORBIS__)
		eAction_ViewLeaderboardsPSN,
		eAction_ViewInvitesPSN,
		eAction_SaveGamePSN,
		eAction_BuyTexturePackPSN
#endif

	};
	eActions m_eAction;

	UIControl_Button m_buttons[BUTTONS_PAUSE_MAX];
	UI_BEGIN_MAP_ELEMENTS_AND_NAMES(UIScene)		
		UI_MAP_ELEMENT( m_buttons[BUTTON_PAUSE_RESUMEGAME], "Button1")
		UI_MAP_ELEMENT( m_buttons[BUTTON_PAUSE_HELPANDOPTIONS], "Button2")
		UI_MAP_ELEMENT( m_buttons[BUTTON_PAUSE_LEADERBOARDS], "Button3")
#ifdef _DURANGO
		UI_MAP_ELEMENT( m_buttons[BUTTON_PAUSE_XBOXHELP], "Button4")
#else
		UI_MAP_ELEMENT( m_buttons[BUTTON_PAUSE_ACHIEVEMENTS], "Button4")
#endif
		UI_MAP_ELEMENT( m_buttons[BUTTON_PAUSE_SAVEGAME], "Button5")
		UI_MAP_ELEMENT( m_buttons[BUTTON_PAUSE_EXITGAME], "Button6")
	UI_END_MAP_ELEMENTS_AND_NAMES()

	virtual void HandleDLCMountingComplete();
	virtual void HandleDLCInstalled();
#ifdef _XBOX_ONE
	virtual void HandleDLCLicenseChange();
#endif
	static int UnlockFullSaveReturned(void *pParam,int iPad,C4JStorage::EMessageResult result);
	static int SaveGame_SignInReturned(void *pParam,bool bContinue, int iPad);

public:
	UIScene_PauseMenu(int iPad, void *initData, UILayer *parentLayer);
	virtual ~UIScene_PauseMenu();

	virtual EUIScene getSceneType() { return eUIScene_PauseMenu;}

	virtual void tick();

	virtual void updateTooltips();
	virtual void updateComponents();
	virtual void handlePreReload();
	virtual void handleReload();

protected:
	void updateControlsVisibility();

	// TODO: This should be pure virtual in this class
	virtual wstring getMoviePath();

public:
	// INPUT
	virtual void handleInput(int iPad, int key, bool repeat, bool pressed, bool released, bool &handled);

protected:
	void handlePress(F64 controlId, F64 childId);
	virtual void ShowScene(bool show);
	virtual void SetIgnoreInput(bool ignoreInput);
	bool m_bIgnoreInput;

private:
	void PerformActionSaveGame();

#if defined(__PS3__) || defined(__PSVITA__) || defined(__ORBIS__)
	static int MustSignInReturnedPSN(void *pParam,int iPad,C4JStorage::EMessageResult result);
	static int ViewLeaderboards_SignInReturned(void *pParam,bool bContinue, int iPad);
	static int ViewInvites_SignInReturned(void *pParam,bool bContinue, int iPad);
	static int BuyTexturePack_SignInReturned(void *pParam,bool bContinue, int iPad);
	static int WarningTrialTexturePackReturned(void *pParam,int iPad,C4JStorage::EMessageResult result);
	static int ExitGameSaveDialogReturned(void *pParam,int iPad,C4JStorage::EMessageResult result);
#endif

protected:
#ifdef _XBOX_ONE
	static int BanGameDialogReturned(void *pParam,int iPad,C4JStorage::EMessageResult result);
	virtual long long getDefaultGtcButtons() { return _360_GTC_BACK | _360_GTC_PLAY; }
#endif

#ifdef __ORBIS__
	bool CheckForPatch();
#endif
};
