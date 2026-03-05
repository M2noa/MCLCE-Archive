#include "stdafx.h"
#include "UI.h"
#include "UIScene_DebugEnhanced.h"

LPCWSTR UIScene_DebugEnhanced::m_CheatCheckboxText[25]=
{
	L"Flight Mode",
	L"NoClip (Walk Through Walls)",
	L"Super Speed",
	L"X-Ray Vision",
	L"Fullbright (No Darkness)",
	L"Infinite Reach",
	L"One Hit Kill",
	L"No Fall Damage",
	L"Unlimited Air (No Drowning)",
	L"Zoom Enabled",
	L"Freecam Mode",
	L"Freeze Time",
	L"Clear Weather",
	L"Instant Break Blocks",
	L"Auto Jump",
	L"Wall Climb",
	L"Step Assist (Auto Step Up)",
	L"God Mode (Invincible)",
	L"Infinite Items",
	L"Night Vision",
	L"Speed Mining",
	L"No Hunger",
	L"Mob ESP",
	L"Player ESP",
	L"Chest ESP",
};

UIScene_DebugEnhanced::UIScene_DebugEnhanced(int iPad, void *initData, UILayer *parentLayer) : UIScene(iPad, parentLayer)
{
	initialiseMovie();

	for(m_iTotalCheckboxElements = 0; m_iTotalCheckboxElements < 25; ++m_iTotalCheckboxElements)
	{
		wstring label(m_CheatCheckboxText[m_iTotalCheckboxElements]);
		bool isEnabled = false;
		
		switch(m_iTotalCheckboxElements)
		{
		case 0: isEnabled = (app.GetGameSettings(iPad,eGameSetting_Flight)!=0); break;
		case 1: isEnabled = (app.GetGameSettings(iPad,eGameSetting_NoClip)!=0); break;
		case 2: isEnabled = (app.GetGameSettings(iPad,eGameSetting_SuperSpeed)!=0); break;
		case 3: isEnabled = (app.GetGameSettings(iPad,eGameSetting_XRay)!=0); break;
		case 4: isEnabled = (app.GetGameSettings(iPad,eGameSetting_Fullbright)!=0); break;
		case 5: isEnabled = (app.GetGameSettings(iPad,eGameSetting_InfiniteReach)!=0); break;
		case 6: isEnabled = (app.GetGameSettings(iPad,eGameSetting_OneHitKill)!=0); break;
		case 7: isEnabled = (app.GetGameSettings(iPad,eGameSetting_NoFallDamage)!=0); break;
		case 8: isEnabled = (app.GetGameSettings(iPad,eGameSetting_UnlimitedAir)!=0); break;
		case 9: isEnabled = (app.GetGameSettings(iPad,eGameSetting_Zoom)!=0); break;
		case 10: isEnabled = (app.GetGameSettings(iPad,eGameSetting_Freecam)!=0); break;
		case 11: isEnabled = (app.GetGameSettings(iPad,eGameSetting_TimeControl)!=0); break;
		case 12: isEnabled = (app.GetGameSettings(iPad,eGameSetting_WeatherControl)!=0); break;
		case 13: isEnabled = (app.GetGameSettings(iPad,eGameSetting_InstantBreak)!=0); break;
		case 14: isEnabled = (app.GetGameSettings(iPad,eGameSetting_AutoJump)!=0); break;
		case 15: isEnabled = (app.GetGameSettings(iPad,eGameSetting_WallClimb)!=0); break;
		case 16: isEnabled = (app.GetGameSettings(iPad,eGameSetting_StepAssist)!=0); break;
		}
		
		m_checkboxes[m_iTotalCheckboxElements].init(label,m_iTotalCheckboxElements,isEnabled);
	}
}

wstring UIScene_DebugEnhanced::getMoviePath()
{
	return L"DebugOptionsMenu";
}

void UIScene_DebugEnhanced::handleInput(int iPad, int key, bool repeat, bool pressed, bool released, bool &handled)
{
	switch(key)
	{
	case ACTION_MENU_CANCEL:
		if(pressed)
		{
			app.SetGameSettings(m_iPad,eGameSetting_Flight,m_checkboxes[0].IsChecked()?1:0);
			app.SetGameSettings(m_iPad,eGameSetting_NoClip,m_checkboxes[1].IsChecked()?1:0);
			app.SetGameSettings(m_iPad,eGameSetting_SuperSpeed,m_checkboxes[2].IsChecked()?1:0);
			app.SetGameSettings(m_iPad,eGameSetting_XRay,m_checkboxes[3].IsChecked()?1:0);
			app.SetGameSettings(m_iPad,eGameSetting_Fullbright,m_checkboxes[4].IsChecked()?1:0);
			app.SetGameSettings(m_iPad,eGameSetting_InfiniteReach,m_checkboxes[5].IsChecked()?1:0);
			app.SetGameSettings(m_iPad,eGameSetting_OneHitKill,m_checkboxes[6].IsChecked()?1:0);
			app.SetGameSettings(m_iPad,eGameSetting_NoFallDamage,m_checkboxes[7].IsChecked()?1:0);
			app.SetGameSettings(m_iPad,eGameSetting_UnlimitedAir,m_checkboxes[8].IsChecked()?1:0);
			app.SetGameSettings(m_iPad,eGameSetting_Zoom,m_checkboxes[9].IsChecked()?1:0);
			app.SetGameSettings(m_iPad,eGameSetting_Freecam,m_checkboxes[10].IsChecked()?1:0);
			app.SetGameSettings(m_iPad,eGameSetting_TimeControl,m_checkboxes[11].IsChecked()?1:0);
			app.SetGameSettings(m_iPad,eGameSetting_WeatherControl,m_checkboxes[12].IsChecked()?1:0);
			app.SetGameSettings(m_iPad,eGameSetting_InstantBreak,m_checkboxes[13].IsChecked()?1:0);
			app.SetGameSettings(m_iPad,eGameSetting_AutoJump,m_checkboxes[14].IsChecked()?1:0);
			app.SetGameSettings(m_iPad,eGameSetting_WallClimb,m_checkboxes[15].IsChecked()?1:0);
			app.SetGameSettings(m_iPad,eGameSetting_StepAssist,m_checkboxes[16].IsChecked()?1:0);

			app.CheckGameSettingsChanged(true,iPad);

			navigateBack();
		}
		break;
	case ACTION_MENU_OK:
	case ACTION_MENU_UP:
	case ACTION_MENU_DOWN:
	case ACTION_MENU_PAGEUP:
	case ACTION_MENU_PAGEDOWN:
		sendInputToMovie(key, repeat, pressed, released);
		break;
	}
}
