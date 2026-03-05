#include "stdafx.h"
#include "UI.h"
#include "UIScene_Intro.h"


UIScene_Intro::UIScene_Intro(int iPad, void *initData, UILayer *parentLayer) : UIScene(iPad, parentLayer)
{
	// Setup all the Iggy references we need for this scene
	initialiseMovie();
	m_bIgnoreNavigate = false;
	m_bAnimationEnded = false;

	bool bSkipESRB = false;
#if defined(__PS3__)
	bSkipESRB = app.GetProductSKU() != e_sku_SCEA;
#endif

	// 4J Stu - These map to values in the Actionscript
#if defined(__PS3__)
	int platformIdx = 3;
#endif

	IggyDataValue result;
	IggyDataValue value[2];
	value[0].type = IGGY_DATATYPE_number;
	value[0].number = platformIdx;

	value[1].type = IGGY_DATATYPE_boolean;
	value[1].boolval = bSkipESRB;
	IggyResult out = IggyPlayerCallMethodRS ( getMovie() , &result, IggyPlayerRootPath( getMovie() ), m_funcSetIntroPlatform , 2 , value );

}

wstring UIScene_Intro::getMoviePath()
{
	return L"Intro";
}

void UIScene_Intro::handleInput(int iPad, int key, bool repeat, bool pressed, bool released, bool &handled)
{
	ui.AnimateKeyPress(m_iPad, key, repeat, pressed, released);

	switch(key)
	{
	case ACTION_MENU_OK:
		if(!m_bIgnoreNavigate)
		{
			m_bIgnoreNavigate = true;
			//ui.NavigateToHomeMenu();
#if defined(__PS3__)

			// has the user seen the EULA already ? We need their options file loaded for this
			C4JStorage::eOptionsCallback eStatus=app.GetOptionsCallbackStatus(0);
			switch(eStatus)
			{
			case C4JStorage::eOptions_Callback_Read:
			case C4JStorage::eOptions_Callback_Read_FileNotFound:
				// we've either read it, or it wasn't found
				if(app.GetGameSettings(0,eGameSetting_PS3_EULA_Read)==0)
				{
					ui.NavigateToScene(0,eUIScene_EULA);
				}
				else
				{
					ui.NavigateToScene(0,eUIScene_SaveMessage);
				}
				break;
			default:
				ui.NavigateToScene(0,eUIScene_EULA);			
				break;
			}
#else
			ui.NavigateToScene(0,eUIScene_SaveMessage);
#endif
		}
		break;
	}
}


void UIScene_Intro::handleAnimationEnd()
{
	if(!m_bIgnoreNavigate)
	{
		m_bIgnoreNavigate = true;
		//ui.NavigateToHomeMenu();
#if defined(__PS3__)
		// has the user seen the EULA already ? We need their options file loaded for this
		C4JStorage::eOptionsCallback eStatus=app.GetOptionsCallbackStatus(0);
		switch(eStatus)
		{
		case C4JStorage::eOptions_Callback_Read:
		case C4JStorage::eOptions_Callback_Read_FileNotFound:
			// we've either read it, or it wasn't found
			if(app.GetGameSettings(0,eGameSetting_PS3_EULA_Read)==0)
			{
				ui.NavigateToScene(0,eUIScene_EULA);
			}
			else
			{
				ui.NavigateToScene(0,eUIScene_SaveMessage);
			}
			break;
		default:
			ui.NavigateToScene(0,eUIScene_EULA);			
		break;
		}


#else
		ui.NavigateToScene(0,eUIScene_SaveMessage);
#endif
	}
}

void UIScene_Intro::handleGainFocus(bool navBack)
{
	// Only relevant on xbox one - if we didn't navigate to the main menu at animation end due to the timer or quadrant sign-in being up, then we'll need to
	// do it now in case the user has cancelled or joining a game failed
	if( m_bAnimationEnded )
	{
		ui.NavigateToScene(0,eUIScene_MainMenu);
	}
}
