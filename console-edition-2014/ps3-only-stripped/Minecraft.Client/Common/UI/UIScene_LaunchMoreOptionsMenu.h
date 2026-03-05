#pragma once

#include "UIScene.h"

class UIScene_LaunchMoreOptionsMenu : public UIScene
{
private:
	enum EControls
	{
		// Add all checkboxes at the start as they also index into a checkboxes array
		eLaunchCheckbox_Online,
		eLaunchCheckbox_InviteOnly,
		eLaunchCheckbox_AllowFoF,
		eLaunchCheckbox_PVP,
		eLaunchCheckbox_TrustSystem,
		eLaunchCheckbox_FireSpreads,
		eLaunchCheckbox_TNT,
		eLaunchCheckbox_HostPrivileges,
		eLaunchCheckbox_ResetNether,
		eLaunchCheckbox_Structures,
		eLaunchCheckbox_FlatWorld,
		eLaunchCheckbox_BonusChest,
		eLaunchCheckbox_DisableSaving,

		eLaunchCheckboxes_Count,

		eControl_EditSeed,
		eControl_WorldSize,
	};

	UIControl m_gameOptions, m_worldOptions;
	UIControl_CheckBox m_checkboxes[eLaunchCheckboxes_Count];
	UIControl_Label m_labelWorldOptions, m_labelGameOptions, m_labelDescription;
	UIControl_Label m_labelSeed, m_labelRandomSeed, m_labelWorldSize;
	UIControl_TextInput m_editSeed;
	UIControl_Slider m_sliderWorldSize;
	IggyName m_funcSetMenuType, m_funcChangeTab;
	UI_BEGIN_MAP_ELEMENTS_AND_NAMES(UIScene)
		if(m_loadedResolution == eSceneResolution_1080)
		{
			UI_MAP_ELEMENT( m_labelGameOptions, "LabelGame")
			UI_MAP_ELEMENT( m_labelWorldOptions, "LabelWorld")

			UI_MAP_ELEMENT( m_gameOptions, "GameOptions")
			UI_BEGIN_MAP_CHILD_ELEMENTS(m_gameOptions)
				UI_MAP_ELEMENT( m_checkboxes[eLaunchCheckbox_Online], "CheckboxOnline")
				UI_MAP_ELEMENT( m_checkboxes[eLaunchCheckbox_InviteOnly], "CheckboxInviteOnly")
				UI_MAP_ELEMENT( m_checkboxes[eLaunchCheckbox_AllowFoF], "CheckboxAllowFoF")
				UI_MAP_ELEMENT( m_checkboxes[eLaunchCheckbox_PVP], "CheckboxPVP")
				UI_MAP_ELEMENT( m_checkboxes[eLaunchCheckbox_TrustSystem], "CheckboxTrustSystem")
				UI_MAP_ELEMENT( m_checkboxes[eLaunchCheckbox_FireSpreads], "CheckboxFireSpreads")
				UI_MAP_ELEMENT( m_checkboxes[eLaunchCheckbox_TNT], "CheckboxTNT")
				UI_MAP_ELEMENT( m_checkboxes[eLaunchCheckbox_HostPrivileges], "CheckboxHostPrivileges")
				UI_MAP_ELEMENT( m_checkboxes[eLaunchCheckbox_ResetNether], "CheckboxResetNether")
				UI_MAP_ELEMENT( m_checkboxes[eLaunchCheckbox_DisableSaving], "CheckboxDisableSaving")
			UI_END_MAP_CHILD_ELEMENTS()
			
			UI_MAP_ELEMENT(m_worldOptions, "WorldOptions")
			UI_BEGIN_MAP_CHILD_ELEMENTS(m_worldOptions)
				UI_MAP_ELEMENT( m_labelSeed, "Seed")
				UI_MAP_ELEMENT( m_editSeed, "EditSeed")
				UI_MAP_ELEMENT( m_labelRandomSeed, "RandomSeed")
				UI_MAP_ELEMENT( m_labelWorldSize, "WorldSize")
				UI_MAP_ELEMENT( m_sliderWorldSize, "WorldSizeSlider")
				UI_MAP_ELEMENT( m_checkboxes[eLaunchCheckbox_Structures], "CheckboxStructures")
				UI_MAP_ELEMENT( m_checkboxes[eLaunchCheckbox_BonusChest], "CheckboxBonusChest")
				UI_MAP_ELEMENT( m_checkboxes[eLaunchCheckbox_FlatWorld], "CheckboxFlatWorld")
			UI_END_MAP_CHILD_ELEMENTS()

			UI_MAP_NAME( m_funcChangeTab, L"ChangeTab")
		}
		else
		{
			UI_MAP_ELEMENT( m_checkboxes[eLaunchCheckbox_Online], "CheckboxOnline")
			UI_MAP_ELEMENT( m_checkboxes[eLaunchCheckbox_InviteOnly], "CheckboxInviteOnly")
			UI_MAP_ELEMENT( m_checkboxes[eLaunchCheckbox_AllowFoF], "CheckboxAllowFoF")
			UI_MAP_ELEMENT( m_checkboxes[eLaunchCheckbox_PVP], "CheckboxPVP")
			UI_MAP_ELEMENT( m_checkboxes[eLaunchCheckbox_TrustSystem], "CheckboxTrustSystem")
			UI_MAP_ELEMENT( m_checkboxes[eLaunchCheckbox_FireSpreads], "CheckboxFireSpreads")
			UI_MAP_ELEMENT( m_checkboxes[eLaunchCheckbox_TNT], "CheckboxTNT")
			UI_MAP_ELEMENT( m_checkboxes[eLaunchCheckbox_HostPrivileges], "CheckboxHostPrivileges")
			UI_MAP_ELEMENT( m_checkboxes[eLaunchCheckbox_ResetNether], "CheckboxResetNether")

			UI_MAP_ELEMENT( m_labelWorldOptions, "WorldOptions")
			UI_MAP_ELEMENT( m_checkboxes[eLaunchCheckbox_Structures], "CheckboxStructures")
			UI_MAP_ELEMENT( m_checkboxes[eLaunchCheckbox_FlatWorld], "CheckboxFlatWorld")
			UI_MAP_ELEMENT( m_checkboxes[eLaunchCheckbox_BonusChest], "CheckboxBonusChest")
		}

		UI_MAP_ELEMENT( m_labelDescription, "Description")

		UI_MAP_NAME( m_funcSetMenuType, L"SetMenuType")
	UI_END_MAP_ELEMENTS_AND_NAMES()

	LaunchMoreOptionsMenuInitData *m_params;
	bool m_bMultiplayerAllowed;
	bool m_bIgnoreInput;
	bool m_tabIndex;

public:
	UIScene_LaunchMoreOptionsMenu(int iPad, void *initData, UILayer *parentLayer);
	
	virtual void updateTooltips();
	virtual void updateComponents();

	virtual EUIScene getSceneType() { return eUIScene_LaunchMoreOptionsMenu;}

protected:
	// TODO: This should be pure virtual in this class
	virtual wstring getMoviePath();

public:
	virtual void tick();
	virtual void handleDestroy();
	// INPUT
	virtual void handleInput(int iPad, int key, bool repeat, bool pressed, bool released, bool &handled);
	virtual void handleFocusChange(F64 controlId, F64 childId);
	virtual void handleTimerComplete(int id);
	static int KeyboardCompleteSeedCallback(LPVOID lpParam,const bool bRes);
	virtual void handlePress(F64 controlId, F64 childId);
	virtual void handleSliderMove(F64 sliderId, F64 currentValue);

protected:
	void handleCheckboxToggled(F64 controlId, bool selected);
};
