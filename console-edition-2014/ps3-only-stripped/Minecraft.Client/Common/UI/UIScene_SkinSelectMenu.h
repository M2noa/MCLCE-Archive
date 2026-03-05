#pragma once
#include "..\..\..\Minecraft.World\Definitions.h"
#include "UIScene.h"
#include "UIControl_PlayerSkinPreview.h"

class UIScene_SkinSelectMenu : public UIScene
{
private:
	static WCHAR *wchDefaultNamesA[eDefaultSkins_Count];

	// 4J Stu - How many to show on each side of the main control
	static const BYTE sidePreviewControls = 4;


	enum ESkinSelectNavigation
	{
		eSkinNavigation_Pack,
		eSkinNavigation_Skin,
		
		eSkinNavigation_Count,
	};

	enum ECharacters
	{
		eCharacter_Current,
		eCharacter_Next1,
		eCharacter_Next2,
		eCharacter_Next3,
		eCharacter_Next4,
		eCharacter_Previous1,
		eCharacter_Previous2,
		eCharacter_Previous3,
		eCharacter_Previous4,

		eCharacter_COUNT,
	};

	UIControl_PlayerSkinPreview m_characters[eCharacter_COUNT];
	UIControl_Label m_labelSkinName, m_labelSkinOrigin;
	UIControl_Label m_labelSelected;
	UIControl m_controlSkinNamePlate, m_controlSelectedPanel, m_controlIggyCharacters, m_controlTimer;
	IggyName m_funcSetPlayerCharacterSelected, m_funcSetCharacterLocked;
	IggyName m_funcSetLeftLabel, m_funcSetRightLabel, m_funcSetCentreLabel;
	UI_BEGIN_MAP_ELEMENTS_AND_NAMES(UIScene)
		UI_MAP_ELEMENT( m_controlSkinNamePlate, "SkinNamePlate")
		UI_BEGIN_MAP_CHILD_ELEMENTS( m_controlSkinNamePlate )
			UI_MAP_ELEMENT( m_labelSkinName, "SkinTitle1")
			UI_MAP_ELEMENT( m_labelSkinOrigin, "SkinTitle2")
		UI_END_MAP_CHILD_ELEMENTS()
		
		UI_MAP_ELEMENT( m_controlSelectedPanel, "SelectedPanel" )
		UI_BEGIN_MAP_CHILD_ELEMENTS( m_controlSelectedPanel )
			UI_MAP_ELEMENT( m_labelSelected, "SelectedPanelLabel" )
		UI_END_MAP_CHILD_ELEMENTS()

		UI_MAP_ELEMENT( m_controlTimer, "Timer" )

		// 4J Stu - These aren't really used a AS3 controls, but adding here means that they get ticked by the scene
		UI_MAP_ELEMENT( m_controlIggyCharacters, "IggyCharacters" )
		UI_BEGIN_MAP_CHILD_ELEMENTS( m_controlIggyCharacters )
			UI_MAP_ELEMENT( m_characters[eCharacter_Current], "iggy_Character0" )

			UI_MAP_ELEMENT( m_characters[eCharacter_Next1], "iggy_Character1" )
			UI_MAP_ELEMENT( m_characters[eCharacter_Next2], "iggy_Character2" )
			UI_MAP_ELEMENT( m_characters[eCharacter_Next3], "iggy_Character3" )
			UI_MAP_ELEMENT( m_characters[eCharacter_Next4], "iggy_Character4" )
		
			UI_MAP_ELEMENT( m_characters[eCharacter_Previous1], "iggy_Character5" )
			UI_MAP_ELEMENT( m_characters[eCharacter_Previous2], "iggy_Character6" )
			UI_MAP_ELEMENT( m_characters[eCharacter_Previous3], "iggy_Character7" )
			UI_MAP_ELEMENT( m_characters[eCharacter_Previous4], "iggy_Character8" )
		UI_END_MAP_CHILD_ELEMENTS()

		UI_MAP_NAME( m_funcSetPlayerCharacterSelected, L"SetPlayerCharacterSelected" )
		UI_MAP_NAME( m_funcSetCharacterLocked, L"SetCharacterLocked" )

		UI_MAP_NAME( m_funcSetLeftLabel, L"SetLeftLabel" )
		UI_MAP_NAME( m_funcSetCentreLabel, L"SetCenterLabel" )
		UI_MAP_NAME( m_funcSetRightLabel, L"SetRightLabel" )
	UI_END_MAP_ELEMENTS_AND_NAMES()

	DLCPack *m_currentPack;
	DWORD m_packIndex, m_skinIndex;
	DWORD m_originalSkinId;
	wstring m_currentSkinPath, m_selectedSkinPath, m_selectedCapePath;
	vector<SKIN_BOX *> *m_vAdditionalSkinBoxes;

	bool m_bSlidingSkins, m_bAnimatingMove;
	ESkinSelectNavigation m_currentNavigation;

	bool m_bNoSkinsToShow;
	DWORD m_currentPackCount;
	bool m_bIgnoreInput;
	bool m_bSkinIndexChanged;
	wstring m_leftLabel, m_centreLabel, m_rightLabel;

	S32 m_iTouchXStart;
	bool m_bTouchScrolled;
public:
	UIScene_SkinSelectMenu(int iPad, void *initData, UILayer *parentLayer);

	virtual void tick();
	
	virtual void updateTooltips();
	virtual void updateComponents();

	virtual EUIScene getSceneType() { return eUIScene_SkinSelectMenu;}

	virtual void handleAnimationEnd();


protected:
	// TODO: This should be pure virtual in this class
	virtual wstring getMoviePath();

public:
	// INPUT
	virtual void handleInput(int iPad, int key, bool repeat, bool pressed, bool released, bool &handled);

	virtual void customDraw(IggyCustomDrawCallbackRegion *region);

private:
	void handleSkinIndexChanged();
	int getNextSkinIndex(DWORD sourceIndex);
	int getPreviousSkinIndex(DWORD sourceIndex);

	TEXTURE_NAME getTextureId(int skinIndex);
	
	void handlePackIndexChanged();
	void updatePackDisplay();
	int getNextPackIndex(DWORD sourceIndex);
	int getPreviousPackIndex(DWORD sourceIndex);

	void setCharacterSelected(bool selected);
	void setCharacterLocked(bool locked);

	void setLeftLabel(const wstring &label);
	void setCentreLabel(const wstring &label);
	void setRightLabel(const wstring &label);

	virtual void HandleDLCMountingComplete();
	virtual void HandleDLCInstalled();

	void showNotOnlineDialog(int iPad);

	static int UnlockSkinReturned(void *pParam,int iPad,C4JStorage::EMessageResult result);
	static int RenableInput(LPVOID lpVoid, int, int);
	void AddFavoriteSkin(int iPad,int iSkinID);

	void InputActionOK(unsigned int iPad);
	virtual void handleReload();


};
