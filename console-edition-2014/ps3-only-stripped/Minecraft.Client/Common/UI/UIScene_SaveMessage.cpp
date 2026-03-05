#include "stdafx.h"
#include "UI.h"
#include "UIScene_SaveMessage.h"

#define PROFILE_LOADED_TIMER_ID 0
#define PROFILE_LOADED_TIMER_TIME 50

UIScene_SaveMessage::UIScene_SaveMessage(int iPad, void *initData, UILayer *parentLayer) : UIScene(iPad, parentLayer)
{
	// Setup all the Iggy references we need for this scene
	initialiseMovie();

	parentLayer->addComponent(iPad,eUIComponent_Panorama);
	parentLayer->addComponent(iPad,eUIComponent_Logo);

	m_buttonConfirm.init(app.GetString(IDS_CONFIRM_OK),eControl_Confirm);
	m_labelDescription.init(app.GetString(IDS_SAVE_ICON_MESSAGE));

	IggyDataValue result;

	// Russian needs to resize the box
	IggyResult out = IggyPlayerCallMethodRS ( getMovie() , &result, IggyPlayerRootPath( getMovie() ), m_funcAutoResize , 0 , NULL );

	// 4J-PB - If we have a signed in user connected, let's get the DLC now
	for(unsigned int i = 0; i < XUSER_MAX_COUNT; ++i)
	{
		if( (InputManager.IsPadConnected(i) || ProfileManager.IsSignedIn(i)) ) 
		{
			if(!app.DLCInstallProcessCompleted() && !app.DLCInstallPending()) 
			{
				app.StartInstallDLCProcess(i);
				break;
			}
		}
	}

	m_bIgnoreInput=false;

	// 4J-TomK - rebuild touch after auto resize
}

UIScene_SaveMessage::~UIScene_SaveMessage()
{
	m_parentLayer->removeComponent(eUIComponent_Panorama);
	m_parentLayer->removeComponent(eUIComponent_Logo);
}

wstring UIScene_SaveMessage::getMoviePath()
{
	return L"SaveMessage";
}

void UIScene_SaveMessage::updateTooltips()
{
	ui.SetTooltips( DEFAULT_XUI_MENU_USER, IDS_TOOLTIPS_SELECT );
}

void UIScene_SaveMessage::handleInput(int iPad, int key, bool repeat, bool pressed, bool released, bool &handled)
{
	if(m_bIgnoreInput) return;

	ui.AnimateKeyPress(m_iPad, key, repeat, pressed, released);

	switch(key)
	{
	case ACTION_MENU_OK:
		sendInputToMovie(key, repeat, pressed, released);
		break;
		// #ifdef __PS3__
		// 	case ACTION_MENU_Y:
		// 		if(pressed)
		// 		{		
		// 			// language select - switch to Greek for now
		// 			if(app.GetMinecraftLanguage(iPad)==MINECRAFT_LANGUAGE_DEFAULT)
		// 			{
		// 				app.SetMinecraftLanguage(iPad,MINECRAFT_LANGUAGE_GREEK);
		// 			}
		// 			else
		// 			{
		// 				app.SetMinecraftLanguage(iPad,MINECRAFT_LANGUAGE_DEFAULT);
		// 			}
		// 			// reload the string table
		// 			ui.SetupFont();
		// 			app.loadStringTable();
		// 			handleReload();
		// 		}
		// 		break;
		// #endif
	}
}

void UIScene_SaveMessage::handlePress(F64 controlId, F64 childId)
{
	switch((int)controlId)
	{
	case eControl_Confirm:

		//CD - Added for audio
		ui.PlayUISFX(eSFX_Press);

		m_bIgnoreInput=true;

#if defined(__PS3__)
		// wait for the profile to be read - this has been kicked off earlier, so should be read by now
		addTimer(PROFILE_LOADED_TIMER_ID,PROFILE_LOADED_TIMER_TIME);
#else
		ui.NavigateToHomeMenu();
#endif
		break;
	};
}

void UIScene_SaveMessage::handleTimerComplete(int id)
{
	switch(id)
	{
	case PROFILE_LOADED_TIMER_ID:
		{
#if defined(__PS3__)
			C4JStorage::eOptionsCallback eStatus=app.GetOptionsCallbackStatus(0);

			switch(eStatus)
			{
			case C4JStorage::eOptions_Callback_Read:
			case C4JStorage::eOptions_Callback_Read_FileNotFound:
			case C4JStorage::eOptions_Callback_Read_Fail:
				// set defaults - which has already been done				
				killTimer(PROFILE_LOADED_TIMER_ID);
				ui.NavigateToHomeMenu();
				SQRNetworkManager::SafeToRespondToGameBootInvite();
				app.SetOptionsCallbackStatus(0,C4JStorage::eOptions_Callback_Idle);
				break;
			case C4JStorage::eOptions_Callback_Read_CorruptDeleted:
				killTimer(PROFILE_LOADED_TIMER_ID);
				ui.NavigateToHomeMenu();
				SQRNetworkManager::SafeToRespondToGameBootInvite();
				app.SetOptionsCallbackStatus(0,C4JStorage::eOptions_Callback_Idle);
				break;
			case C4JStorage::eOptions_Callback_Read_Corrupt:
				// get the user to delete the options file
				app.DebugPrintf("Corrupt options file\n");
				app.SetOptionsCallbackStatus(0,C4JStorage::eOptions_Callback_Read_CorruptDeletePending);
				m_bIgnoreInput=false;
				// give the option to delete the save
				UINT uiIDA[1];
				uiIDA[0]=IDS_CONFIRM_OK;
				ui.RequestMessageBox(IDS_CORRUPT_FILE, IDS_CORRUPT_OPTIONS, uiIDA, 1, 
					0,&UIScene_SaveMessage::DeleteOptionsDialogReturned,this, app.GetStringTable());
				break;
			}
#endif
		}

		break;
	}
}

#if defined(__PS3__)
int UIScene_SaveMessage::DeleteOptionsDialogReturned(void *pParam,int iPad,C4JStorage::EMessageResult result)
{
	//UIScene_SaveMessage* pClass = (UIScene_SaveMessage*)pParam;

	// kick off the delete
	StorageManager.DeleteOptionsData(iPad);

	return 0;
}
#endif
