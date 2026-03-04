#include "stdafx.h"
#include "UI.h"
#include "UIScene_PauseMenu.h"
#include "..\..\MinecraftServer.h"
#include "..\..\MultiplayerLocalPlayer.h"
#include "..\..\TexturePackRepository.h"
#include "..\..\TexturePack.h"
#include "..\..\DLCTexturePack.h"
#include "..\..\..\Minecraft.World\StringHelpers.h"



#if defined __PS3__
#define USE_SONY_REMOTE_STORAGE
#endif

UIScene_PauseMenu::UIScene_PauseMenu(int iPad, void *initData, UILayer *parentLayer) : UIScene(iPad, parentLayer)
{
	// Setup all the Iggy references we need for this scene
	initialiseMovie();
	m_bIgnoreInput=false;
	m_eAction=eAction_None;

	m_buttons[BUTTON_PAUSE_RESUMEGAME].init(app.GetString(IDS_RESUME_GAME),BUTTON_PAUSE_RESUMEGAME);
	m_buttons[BUTTON_PAUSE_HELPANDOPTIONS].init(app.GetString(IDS_HELP_AND_OPTIONS),BUTTON_PAUSE_HELPANDOPTIONS);
	m_buttons[BUTTON_PAUSE_LEADERBOARDS].init(app.GetString(IDS_LEADERBOARDS),BUTTON_PAUSE_LEADERBOARDS);
	m_buttons[BUTTON_PAUSE_ACHIEVEMENTS].init(app.GetString(IDS_ACHIEVEMENTS),BUTTON_PAUSE_ACHIEVEMENTS);
	m_buttons[BUTTON_PAUSE_SAVEGAME].init(app.GetString(IDS_SAVE_GAME),BUTTON_PAUSE_SAVEGAME);
	m_buttons[BUTTON_PAUSE_EXITGAME].init(app.GetString(IDS_EXIT_GAME),BUTTON_PAUSE_EXITGAME);

	if(!ProfileManager.IsFullVersion())
	{
		// hide the trial timer
		ui.ShowTrialTimer(false);
	}

	updateControlsVisibility();

	doHorizontalResizeCheck();

	// get rid of the quadrant display if it's on
	ui.HidePressStart();

#if TO_BE_IMPLEMENTED
	XuiSetTimer(m_hObj,IGNORE_KEYPRESS_TIMERID,IGNORE_KEYPRESS_TIME);
#endif

	if( g_NetworkManager.IsLocalGame() && g_NetworkManager.GetPlayerCount() == 1 )
	{
		app.SetXuiServerAction(ProfileManager.GetPrimaryPad(),eXuiServerAction_PauseServer,(void *)TRUE);
	}

	TelemetryManager->RecordMenuShown(m_iPad, eUIScene_PauseMenu, 0);
	TelemetryManager->RecordPauseOrInactive(m_iPad);

	Minecraft *pMinecraft = Minecraft::GetInstance();
	if(pMinecraft != NULL && pMinecraft->localgameModes[iPad] != NULL )
	{
		TutorialMode *gameMode = (TutorialMode *)pMinecraft->localgameModes[iPad];

		// This just allows it to be shown
		gameMode->getTutorial()->showTutorialPopup(false);
	}
	m_bErrorDialogRunning = false;
}

UIScene_PauseMenu::~UIScene_PauseMenu()
{
	Minecraft *pMinecraft = Minecraft::GetInstance();
	if(pMinecraft != NULL && pMinecraft->localgameModes[m_iPad] != NULL )
	{
		TutorialMode *gameMode = (TutorialMode *)pMinecraft->localgameModes[m_iPad];

		// This just allows it to be shown
		gameMode->getTutorial()->showTutorialPopup(true);
	}

	m_parentLayer->showComponent(m_iPad,eUIComponent_Panorama,false);
	m_parentLayer->showComponent(m_iPad,eUIComponent_MenuBackground,false);
	m_parentLayer->showComponent(m_iPad,eUIComponent_Logo,false);
}

wstring UIScene_PauseMenu::getMoviePath()
{
	if(app.GetLocalPlayerCount() > 1)
	{
		return L"PauseMenuSplit";
	}
	else
	{
		return L"PauseMenu";
	}
}

void UIScene_PauseMenu::tick()
{
	UIScene::tick();




}

void UIScene_PauseMenu::updateTooltips()
{
	bool bUserisClientSide = ProfileManager.IsSignedInLive(m_iPad);
	bool bIsisPrimaryHost=g_NetworkManager.IsHost() && (ProfileManager.GetPrimaryPad()==m_iPad);


	int iY = -1;
#if defined __PS3__
	if(m_iPad == ProfileManager.GetPrimaryPad() ) iY = IDS_TOOLTIPS_GAME_INVITES;
#endif
	int iRB = -1;
	int iX = -1;

	if(ProfileManager.IsFullVersion())
	{
		if(StorageManager.GetSaveDisabled())
		{
			iX = bIsisPrimaryHost?IDS_TOOLTIPS_SELECTDEVICE:-1;
			if( CSocialManager::Instance()->IsTitleAllowedToPostImages() && CSocialManager::Instance()->AreAllUsersAllowedToPostImages() && bUserisClientSide )
			{
#ifndef __PS3__
				iY = IDS_TOOLTIPS_SHARE;
#endif
			}		
		}
		else
		{
			iX = bIsisPrimaryHost?IDS_TOOLTIPS_CHANGEDEVICE:-1;
			if( CSocialManager::Instance()->IsTitleAllowedToPostImages() && CSocialManager::Instance()->AreAllUsersAllowedToPostImages() && bUserisClientSide)
			{
#ifndef __PS3__
				iY = IDS_TOOLTIPS_SHARE;
#endif
			}	
		}
	}
	ui.SetTooltips( m_iPad, IDS_TOOLTIPS_SELECT,IDS_TOOLTIPS_BACK,iX,iY, -1,-1,-1,iRB);
}

void UIScene_PauseMenu::updateComponents()
{
	m_parentLayer->showComponent(m_iPad,eUIComponent_Panorama,false);
	m_parentLayer->showComponent(m_iPad,eUIComponent_MenuBackground,true);

	if( app.GetLocalPlayerCount() == 1 ) m_parentLayer->showComponent(m_iPad,eUIComponent_Logo,true);
	else m_parentLayer->showComponent(m_iPad,eUIComponent_Logo,false);
}

void UIScene_PauseMenu::handlePreReload()
{
}

void UIScene_PauseMenu::handleReload()
{
	updateTooltips();
	updateControlsVisibility();	


	doHorizontalResizeCheck();
}

void UIScene_PauseMenu::updateControlsVisibility()
{
	// are we the primary player?
	// 4J-PB - fix for 7844 & 7845 - 
	// TCR # 128:  XLA Pause Menu:   When in a multiplayer game as a client the Pause Menu does not have a Leaderboards option.
	// TCR # 128:  XLA Pause Menu:   When in a multiplayer game as a client the Pause Menu does not have an Achievements option.
	if(ProfileManager.GetPrimaryPad()==m_iPad) // && g_NetworkManager.IsHost()) 
	{
		// are we in splitscreen?
		// how many local players do we have?
		if( app.GetLocalPlayerCount()>1 )
		{
			// Hide the BUTTON_PAUSE_LEADERBOARDS and BUTTON_PAUSE_ACHIEVEMENTS
			removeControl( &m_buttons[BUTTON_PAUSE_LEADERBOARDS], false );
			removeControl( &m_buttons[BUTTON_PAUSE_ACHIEVEMENTS], false );
		}

		if( !g_NetworkManager.IsHost() )
		{
			// Hide the BUTTON_PAUSE_SAVEGAME
			removeControl( &m_buttons[BUTTON_PAUSE_SAVEGAME], false );
		}
	}
	else
	{
		// Hide the BUTTON_PAUSE_LEADERBOARDS, BUTTON_PAUSE_ACHIEVEMENTS and BUTTON_PAUSE_SAVEGAME
		removeControl( &m_buttons[BUTTON_PAUSE_LEADERBOARDS], false );
		removeControl( &m_buttons[BUTTON_PAUSE_ACHIEVEMENTS], false );
		removeControl( &m_buttons[BUTTON_PAUSE_SAVEGAME], false );
	}

	// is saving disabled?
	if(StorageManager.GetSaveDisabled())
	{
	}

#if defined(__PS3__)
	// We don't have a way to display trophies/achievements, so remove the button, and we're allowed to not have it on Xbox One
	removeControl( &m_buttons[BUTTON_PAUSE_ACHIEVEMENTS], false );
#endif

}

void UIScene_PauseMenu::handleInput(int iPad, int key, bool repeat, bool pressed, bool released, bool &handled)
{
	if(m_bIgnoreInput)
	{
		return;
	}

	//app.DebugPrintf("UIScene_DebugOverlay handling input for pad %d, key %d, down- %s, pressed- %s, released- %s\n", iPad, key, down?"TRUE":"FALSE", pressed?"TRUE":"FALSE", released?"TRUE":"FALSE");
	ui.AnimateKeyPress(iPad, key, repeat, pressed, released);


	switch(key)
	{
#if defined(__PS3__) // not for Orbis - we want to use the pause menu (touchpad press) to select a menu item
	case ACTION_MENU_PAUSEMENU:
#endif
	case ACTION_MENU_CANCEL:
		if(pressed)
		{

			if( iPad == ProfileManager.GetPrimaryPad() && g_NetworkManager.IsLocalGame() )
			{
				app.SetXuiServerAction(ProfileManager.GetPrimaryPad(),eXuiServerAction_PauseServer,(void *)FALSE);
			}

			ui.PlayUISFX(eSFX_Back);
			navigateBack();
			if(!ProfileManager.IsFullVersion())
			{
				ui.ShowTrialTimer(true);
			}
		}
		break;
	case ACTION_MENU_OK:
	case ACTION_MENU_UP:
	case ACTION_MENU_DOWN:
		if(pressed)
		{
			sendInputToMovie(key, repeat, pressed, released);
		}
		break;

#if TO_BE_IMPLEMENTED
	case VK_PAD_X:
		// Change device
		if(bIsisPrimaryHost)
		{	
			// we need a function to deal with the return from this - if it changes, we need to update the pause menu and tooltips
			// Fix for #12531 - TCR 001: BAS Game Stability: When a player selects to change a storage 
			// device, and repeatedly backs out of the SD screen, disconnects from LIVE, and then selects a SD, the title crashes.
			m_bIgnoreInput=true;

			StorageManager.SetSaveDevice(&UIScene_PauseMenu::DeviceSelectReturned,this,true);
		}
		rfHandled = TRUE;
		break;
#endif

	case ACTION_MENU_Y:
		{
			
#if defined(__PS3__)
		if(pressed && iPad == ProfileManager.GetPrimaryPad())
		{

			// Are we offline?
			if(!ProfileManager.IsSignedInLive(iPad))
			{
				m_eAction=eAction_ViewInvitesPSN;
					// get them to sign in to online
				UINT uiIDA[1];
					uiIDA[0]=IDS_PRO_NOTONLINE_ACCEPT;
				ui.RequestMessageBox(IDS_PRO_NOTONLINE_TITLE, IDS_PRO_NOTONLINE_TEXT, uiIDA, 1, iPad, &UIScene_PauseMenu::MustSignInReturnedPSN, this, app.GetStringTable(), NULL, 0, false);
				}
				else
				{
					int ret = sceNpBasicRecvMessageCustom(SCE_NP_BASIC_MESSAGE_MAIN_TYPE_INVITE, SCE_NP_BASIC_RECV_MESSAGE_OPTIONS_INCLUDE_BOOTABLE, SYS_MEMORY_CONTAINER_ID_INVALID);
					app.DebugPrintf("sceNpBasicRecvMessageCustom return %d ( %08x )\n", ret, ret);
				}
			}
#else
#if TO_BE_IMPLEMENTED
			if(bUserisClientSide)
			{			
				// 4J Stu - Added check in 1.8.2 bug fix (TU6) to stop repeat key presses
				bool bCanScreenshot = true;
				for(int j=0; j < XUSER_MAX_COUNT;++j)
				{
					if(app.GetXuiAction(j) == eAppAction_SocialPostScreenshot)
					{
						bCanScreenshot = false;
						break;
					}
				}
				if(bCanScreenshot) app.SetAction(pInputData->UserIndex,eAppAction_SocialPost);
			}
			rfHandled = TRUE;
#endif
#endif
		}
		break;
	}
}

void UIScene_PauseMenu::handlePress(F64 controlId, F64 childId)
{
	if(m_bIgnoreInput) return;

	switch((int)controlId)
	{
	case BUTTON_PAUSE_RESUMEGAME:
		if( m_iPad == ProfileManager.GetPrimaryPad() && g_NetworkManager.IsLocalGame() )
		{
			app.SetXuiServerAction(ProfileManager.GetPrimaryPad(),eXuiServerAction_PauseServer,(void *)FALSE);
		}
		navigateBack();
		break;
	case BUTTON_PAUSE_LEADERBOARDS:
		{
			UINT uiIDA[1];
			uiIDA[0]=IDS_OK;

			//4J Gordon: Being used for the leaderboards proper now
			// guests can't look at leaderboards
			if(ProfileManager.IsGuest(m_iPad))
			{
				ui.RequestMessageBox(IDS_PRO_GUESTPROFILE_TITLE, IDS_PRO_GUESTPROFILE_TEXT, uiIDA, 1, ProfileManager.GetPrimaryPad(),NULL,NULL, app.GetStringTable(), NULL, 0, false);
			}
			else if(!ProfileManager.IsSignedInLive(m_iPad))
			{

#if defined __PS3__
				// get them to sign in to online
				m_eAction=eAction_ViewLeaderboardsPSN;
				UINT uiIDA[1];
				uiIDA[0]=IDS_PRO_NOTONLINE_ACCEPT;
				ui.RequestMessageBox(IDS_PRO_NOTONLINE_TITLE, IDS_PRO_XBOXLIVE_NOTIFICATION, uiIDA, 1, ProfileManager.GetPrimaryPad(),&UIScene_PauseMenu::MustSignInReturnedPSN,this, app.GetStringTable(), NULL, 0, false);
#else
			UINT uiIDA[1] = { IDS_OK };
			ui.RequestMessageBox(IDS_PRO_NOTONLINE_TITLE, IDS_PRO_XBOXLIVE_NOTIFICATION, uiIDA, 1, m_iPad);
#endif
			}
			else
			{	
				bool bContentRestricted=false;
#if defined(__PS3__)
				ProfileManager.GetChatAndContentRestrictions(m_iPad,true,NULL,&bContentRestricted,NULL);
#endif
				if(bContentRestricted)
				{
#if !(0 || defined(_WIN64)) // 4J Stu - Temp to get the win build running, but so we check this for other platforms
					// you can't see leaderboards
					UINT uiIDA[1];
					uiIDA[0]=IDS_CONFIRM_OK;
					ui.RequestMessageBox(IDS_ONLINE_SERVICE_TITLE, IDS_CONTENT_RESTRICTION, uiIDA, 1, m_iPad,NULL,this, app.GetStringTable(), NULL, 0, false);
#endif
				}
				else
				{
					ui.NavigateToScene(m_iPad, eUIScene_LeaderboardsMenu);
				}
			}
		}
		break;
#if TO_BE_IMPLEMENTED
	case BUTTON_PAUSE_ACHIEVEMENTS:

		// guests can't look at achievements
		if(ProfileManager.IsGuest(pNotifyPressData->UserIndex))
		{
			UINT uiIDA[1];
			uiIDA[0]=IDS_OK;
			ui.RequestMessageBox(IDS_PRO_GUESTPROFILE_TITLE, IDS_PRO_GUESTPROFILE_TEXT, uiIDA, 1, ProfileManager.GetPrimaryPad(),NULL,NULL, app.GetStringTable(), NULL, 0, false);
		}
		else
		{
			XShowAchievementsUI( pNotifyPressData->UserIndex );
		}
		break;
#endif

	case BUTTON_PAUSE_HELPANDOPTIONS:
		ui.NavigateToScene(m_iPad,eUIScene_HelpAndOptionsMenu);	
		break;
	case BUTTON_PAUSE_SAVEGAME:
		PerformActionSaveGame();
		break;
	case BUTTON_PAUSE_EXITGAME:
		{
			Minecraft *pMinecraft = Minecraft::GetInstance();
			// Check if it's the trial version
			if(ProfileManager.IsFullVersion())
			{	
				UINT uiIDA[3];

				// is it the primary player exiting?
				if(m_iPad==ProfileManager.GetPrimaryPad())
				{
					int playTime = -1;
					if( pMinecraft->localplayers[m_iPad] != NULL )
					{
						playTime = (int)pMinecraft->localplayers[m_iPad]->getSessionTimer();
					}

					if(StorageManager.GetSaveDisabled())
					{
						uiIDA[0]=IDS_CONFIRM_CANCEL;
						uiIDA[1]=IDS_CONFIRM_OK;
						ui.RequestMessageBox(IDS_EXIT_GAME, IDS_CONFIRM_EXIT_GAME_PROGRESS_LOST, uiIDA, 2, m_iPad,&IUIScene_PauseMenu::ExitGameDialogReturned,this, app.GetStringTable(), NULL, 0, false);
					}
					else
					{
						if( g_NetworkManager.IsHost() )
						{	
							uiIDA[0]=IDS_CONFIRM_CANCEL;
							uiIDA[1]=IDS_EXIT_GAME_SAVE;
							uiIDA[2]=IDS_EXIT_GAME_NO_SAVE;

							if(g_NetworkManager.GetPlayerCount()>1)
							{
								ui.RequestMessageBox(IDS_EXIT_GAME, IDS_CONFIRM_EXIT_GAME_CONFIRM_DISCONNECT_SAVE, uiIDA, 3, m_iPad,&UIScene_PauseMenu::ExitGameSaveDialogReturned,this, app.GetStringTable(), NULL, 0, false);
							}
							else
							{
								ui.RequestMessageBox(IDS_EXIT_GAME, IDS_CONFIRM_EXIT_GAME, uiIDA, 3, m_iPad,&UIScene_PauseMenu::ExitGameSaveDialogReturned,this, app.GetStringTable(), NULL, 0, false);
							}
						}
						else
						{
							uiIDA[0]=IDS_CONFIRM_CANCEL;
							uiIDA[1]=IDS_CONFIRM_OK;

							ui.RequestMessageBox(IDS_EXIT_GAME, IDS_CONFIRM_EXIT_GAME, uiIDA, 2, m_iPad,&IUIScene_PauseMenu::ExitGameDialogReturned,this, app.GetStringTable(), NULL, 0, false);
						}
					}
				}
				else
				{
					int playTime = -1;
					if( pMinecraft->localplayers[m_iPad] != NULL )
					{
						playTime = (int)pMinecraft->localplayers[m_iPad]->getSessionTimer();
					}

					TelemetryManager->RecordLevelExit(m_iPad, eSen_LevelExitStatus_Exited);


					// just exit the player
					app.SetAction(m_iPad,eAppAction_ExitPlayer);
				}		
			}
			else
			{
				// is it the primary player exiting?
				if(m_iPad==ProfileManager.GetPrimaryPad())
				{
					int playTime = -1;
					if( pMinecraft->localplayers[m_iPad] != NULL )
					{
						playTime = (int)pMinecraft->localplayers[m_iPad]->getSessionTimer();
					}	

					// adjust the trial time played
					ui.ReduceTrialTimerValue();

					// exit the level
					UINT uiIDA[2];
					uiIDA[0]=IDS_CONFIRM_CANCEL;
					uiIDA[1]=IDS_CONFIRM_OK;
					ui.RequestMessageBox(IDS_EXIT_GAME, IDS_CONFIRM_EXIT_GAME_PROGRESS_LOST, uiIDA, 2, m_iPad,&IUIScene_PauseMenu::ExitGameDialogReturned, dynamic_cast<IUIScene_PauseMenu*>(this), app.GetStringTable(), NULL, 0, false);

				}
				else
				{
					int playTime = -1;
					if( pMinecraft->localplayers[m_iPad] != NULL )
					{
						playTime = (int)pMinecraft->localplayers[m_iPad]->getSessionTimer();
					}

					TelemetryManager->RecordLevelExit(m_iPad, eSen_LevelExitStatus_Exited);

					// just exit the player
					app.SetAction(m_iPad,eAppAction_ExitPlayer);
				}
			}
		}
		break;
	}
}

void UIScene_PauseMenu::PerformActionSaveGame()
{
	// is the player trying to save in the trial version?
	if(!ProfileManager.IsFullVersion())
	{

		// Unlock the full version?
		if(!ProfileManager.IsSignedInLive(m_iPad))
		{
#if defined(__PS3__)
			m_eAction=eAction_SaveGamePSN;
			UINT uiIDA[2];
			uiIDA[0]=IDS_PRO_NOTONLINE_ACCEPT;
			uiIDA[1]=IDS_PRO_NOTONLINE_DECLINE;
			ui.RequestMessageBox(IDS_PRO_NOTONLINE_TITLE, IDS_PRO_XBOXLIVE_NOTIFICATION, uiIDA, 2, ProfileManager.GetPrimaryPad(),&UIScene_PauseMenu::MustSignInReturnedPSN,this, app.GetStringTable(), NULL, 0, false);
#endif
		}
		else
		{
			UINT uiIDA[2];
			uiIDA[0]=IDS_CONFIRM_OK;
			uiIDA[1]=IDS_CONFIRM_CANCEL;
			ui.RequestMessageBox(IDS_UNLOCK_TITLE, IDS_UNLOCK_TOSAVE_TEXT, uiIDA, 2,m_iPad,&UIScene_PauseMenu::UnlockFullSaveReturned,this,app.GetStringTable(), NULL, 0, false);
		}

		return;
	}

	// 4J-PB - Is the player trying to save but they are using a trial texturepack ?
	if(!Minecraft::GetInstance()->skins->isUsingDefaultSkin())
	{
		TexturePack *tPack = Minecraft::GetInstance()->skins->getSelected();
		DLCTexturePack *pDLCTexPack=(DLCTexturePack *)tPack;

		m_pDLCPack=pDLCTexPack->getDLCInfoParentPack();//tPack->getDLCPack();

		if(!m_pDLCPack->hasPurchasedFile( DLCManager::e_DLCType_Texture, L"" ))
		{					
			// upsell
			UINT uiIDA[2];
			uiIDA[0]=IDS_CONFIRM_OK;
			uiIDA[1]=IDS_CONFIRM_CANCEL;

			// Give the player a warning about the trial version of the texture pack
			{
				ui.RequestMessageBox(IDS_WARNING_DLC_TRIALTEXTUREPACK_TITLE, IDS_WARNING_DLC_TRIALTEXTUREPACK_TEXT, uiIDA, 2, m_iPad,&UIScene_PauseMenu::WarningTrialTexturePackReturned,this,app.GetStringTable(), NULL, 0, false);
			}

			return;					
		}
		else
		{
			m_bTrialTexturePack = false;
		}
	}

	// does the save exist?
	bool bSaveExists;
	C4JStorage::ESaveGameState result=StorageManager.DoesSaveExist(&bSaveExists);

	{
			// we need to ask if they are sure they want to overwrite the existing game
			if(bSaveExists)
			{
				UINT uiIDA[2];
				uiIDA[0]=IDS_CONFIRM_CANCEL;
				uiIDA[1]=IDS_CONFIRM_OK;
				ui.RequestMessageBox(IDS_TITLE_SAVE_GAME, IDS_CONFIRM_SAVE_GAME, uiIDA, 2, m_iPad,&IUIScene_PauseMenu::SaveGameDialogReturned,this, app.GetStringTable(), NULL, 0, false);
			}
			else
			{
				// flag a app action of save game
				app.SetAction(m_iPad,eAppAction_SaveGame);
			}
	}
}

void UIScene_PauseMenu::ShowScene(bool show)
{
	app.DebugPrintf("UIScene_PauseMenu::ShowScene is not implemented\n");
}

void UIScene_PauseMenu::HandleDLCInstalled()
{
	// mounted DLC may have changed
	if(app.StartInstallDLCProcess(m_iPad)==false)
	{
		// not doing a mount, so re-enable input
		//m_bIgnoreInput=false;
		app.DebugPrintf("UIScene_PauseMenu::HandleDLCInstalled - m_bIgnoreInput false\n");
	}
	else
	{
		// 4J-PB - Somehow, on th edisc build, we get in here, but don't call HandleDLCMountingComplete, so input locks up
		//m_bIgnoreInput=true;
		app.DebugPrintf("UIScene_PauseMenu::HandleDLCInstalled - m_bIgnoreInput true\n");
	}
	// this will send a CustomMessage_DLCMountingComplete when done
}


void UIScene_PauseMenu::HandleDLCMountingComplete()
{	
	// check if we should display the save option

	//m_bIgnoreInput=false;
	app.DebugPrintf("UIScene_PauseMenu::HandleDLCMountingComplete - m_bIgnoreInput false \n");

	// 	if(ProfileManager.IsFullVersion())
	// 	{
	// 		bool bIsisPrimaryHost=g_NetworkManager.IsHost() && (ProfileManager.GetPrimaryPad()==m_iPad);
	// 
	// 		if(bIsisPrimaryHost)
	// 		{
	// 			m_buttons[BUTTON_PAUSE_SAVEGAME].setEnable(true);
	// 		}
	// 	}
}

int UIScene_PauseMenu::UnlockFullSaveReturned(void *pParam,int iPad,C4JStorage::EMessageResult result)
{
	UIScene_PauseMenu* pClass = (UIScene_PauseMenu*)pParam;
	Minecraft *pMinecraft=Minecraft::GetInstance();

	if(result==C4JStorage::EMessage_ResultAccept)
	{
		if(ProfileManager.IsSignedInLive(pMinecraft->player->GetXboxPad()))
		{
			// 4J-PB - need to check this user can access the store
#if defined(__PS3__)
			bool bContentRestricted;
			ProfileManager.GetChatAndContentRestrictions(ProfileManager.GetPrimaryPad(),true,NULL,&bContentRestricted,NULL);
			if(bContentRestricted)
			{
				UINT uiIDA[1];
				uiIDA[0]=IDS_CONFIRM_OK;
				ui.RequestMessageBox(IDS_ONLINE_SERVICE_TITLE, IDS_CONTENT_RESTRICTION, uiIDA, 1, ProfileManager.GetPrimaryPad(),NULL,pClass, app.GetStringTable(), NULL, 0, false);
			}
			else
#endif
			{
				ProfileManager.DisplayFullVersionPurchase(false,pMinecraft->player->GetXboxPad(),eSen_UpsellID_Full_Version_Of_Game);
			}
		}
	}
	else
	{
		//SentientManager.RecordUpsellResponded(iPad, eSen_UpsellID_Full_Version_Of_Game, app.m_dwOfferID, eSen_UpsellOutcome_Declined);
	}

	return 0;
}

int UIScene_PauseMenu::SaveGame_SignInReturned(void *pParam,bool bContinue, int iPad)
{
	UIScene_PauseMenu* pClass = (UIScene_PauseMenu*)pParam;

	if(bContinue==true)
	{
		pClass->PerformActionSaveGame();
	}

	return 0;
}


#if defined(__PS3__)
int UIScene_PauseMenu::MustSignInReturnedPSN(void *pParam,int iPad,C4JStorage::EMessageResult result)
{
	UIScene_PauseMenu* pClass = (UIScene_PauseMenu*)pParam;

	if(result==C4JStorage::EMessage_ResultAccept) 
	{
#if defined(__PS3__)
		switch(pClass->m_eAction)
		{
		case eAction_ViewLeaderboardsPSN:
			SQRNetworkManager_PS3::AttemptPSNSignIn(&UIScene_PauseMenu::ViewLeaderboards_SignInReturned, pClass);
			break;
		case eAction_ViewInvitesPSN:
			SQRNetworkManager_PS3::AttemptPSNSignIn(&UIScene_PauseMenu::ViewInvites_SignInReturned, pClass);
			break;
		case eAction_SaveGamePSN:
			SQRNetworkManager_PS3::AttemptPSNSignIn(&UIScene_PauseMenu::SaveGame_SignInReturned, pClass);
			break;
		case eAction_BuyTexturePackPSN:
			SQRNetworkManager_PS3::AttemptPSNSignIn(&UIScene_PauseMenu::BuyTexturePack_SignInReturned, pClass);
			break;
		}
#else
		switch(pClass->m_eAction)
		{
		case eAction_ViewLeaderboardsPSN:
			SQRNetworkManager_Orbis::AttemptPSNSignIn(&UIScene_PauseMenu::ViewLeaderboards_SignInReturned, pClass, false, iPad);
			break;
		case eAction_ViewInvitesPSN:
			SQRNetworkManager_Orbis::AttemptPSNSignIn(&UIScene_PauseMenu::ViewInvites_SignInReturned, pClass, false, iPad);
			break;
		case eAction_SaveGamePSN:
			SQRNetworkManager_Orbis::AttemptPSNSignIn(&UIScene_PauseMenu::SaveGame_SignInReturned, pClass, false, iPad);
			break;
		case eAction_BuyTexturePackPSN:
			SQRNetworkManager_Orbis::AttemptPSNSignIn(&UIScene_PauseMenu::BuyTexturePack_SignInReturned, pClass, false, iPad);
			break;
		}
#endif
	}

	return 0;
}

int UIScene_PauseMenu::ViewLeaderboards_SignInReturned(void *pParam,bool bContinue, int iPad)
{
	UIScene_PauseMenu* pClass = (UIScene_PauseMenu*)pParam;

	if(bContinue==true)
	{
		UINT uiIDA[1];
		uiIDA[0]=IDS_CONFIRM_OK;

		// guests can't look at leaderboards
		if(ProfileManager.IsGuest(pClass->m_iPad))
		{
			ui.RequestMessageBox(IDS_PRO_GUESTPROFILE_TITLE, IDS_PRO_GUESTPROFILE_TEXT, uiIDA, 1, ProfileManager.GetPrimaryPad(),NULL,NULL, app.GetStringTable(), NULL, 0, false);
		}
		else if(ProfileManager.IsSignedInLive(iPad))
		{
			bool bContentRestricted=false;
			ProfileManager.GetChatAndContentRestrictions(pClass->m_iPad,true,NULL,&bContentRestricted,NULL);
			if(bContentRestricted)
			{
				// you can't see leaderboards
				ui.RequestMessageBox(IDS_ONLINE_SERVICE_TITLE, IDS_CONTENT_RESTRICTION, uiIDA, 1, ProfileManager.GetPrimaryPad(),NULL,pClass, app.GetStringTable(), NULL, 0, false);
			}
			else
			{
				ui.NavigateToScene(pClass->m_iPad, eUIScene_LeaderboardsMenu);
			}
		}
	}

	return 0;
}

int UIScene_PauseMenu::WarningTrialTexturePackReturned(void *pParam,int iPad,C4JStorage::EMessageResult result)
{
	UIScene_PauseMenu* pClass = (UIScene_PauseMenu*)pParam;


#if defined(__PS3__)
	if(result==C4JStorage::EMessage_ResultAccept)
	{
		if(!ProfileManager.IsSignedInLive(iPad))
		{
			pClass->m_eAction=eAction_SaveGamePSN;
			// You're not signed in to PSN!
			UINT uiIDA[2];
			uiIDA[0]=IDS_PRO_NOTONLINE_ACCEPT;
			uiIDA[1]=IDS_PRO_NOTONLINE_DECLINE;
			ui.RequestMessageBox(IDS_PRO_NOTONLINE_TITLE, IDS_PRO_XBOXLIVE_NOTIFICATION, uiIDA, 2, iPad,&UIScene_PauseMenu::MustSignInReturnedPSN,pClass, app.GetStringTable(), NULL, 0, false);
		}
		else
		{
			// 4J-PB - need to check this user can access the store
			bool bContentRestricted=false;
			ProfileManager.GetChatAndContentRestrictions(ProfileManager.GetPrimaryPad(),true,NULL,&bContentRestricted,NULL);
			if(bContentRestricted)
			{
				UINT uiIDA[1];
				uiIDA[0]=IDS_CONFIRM_OK;
				ui.RequestMessageBox(IDS_ONLINE_SERVICE_TITLE, IDS_CONTENT_RESTRICTION, uiIDA, 1, iPad,NULL,pClass, app.GetStringTable(), NULL, 0, false);
			}
			else
			{
				// need to get info on the pack to see if the user has already downloaded it
				TexturePack *tPack = Minecraft::GetInstance()->skins->getSelected();
				DLCTexturePack *pDLCTexPack=(DLCTexturePack *)tPack;

				// retrieve the store name for the skin pack
				DLCPack *pDLCPack=pDLCTexPack->getDLCInfoParentPack();//tPack->getDLCPack();
				const char *pchPackName=wstringtofilename(pDLCPack->getName());
				app.DebugPrintf("Texture Pack - %s\n",pchPackName);
				SONYDLC *pSONYDLCInfo=app.GetSONYDLCInfo((char *)pchPackName);		

				if(pSONYDLCInfo!=NULL)
				{
					char chName[42];
					char chKeyName[20];
					char chSkuID[SCE_NP_COMMERCE2_SKU_ID_LEN];

					memset(chSkuID,0,SCE_NP_COMMERCE2_SKU_ID_LEN);
					// find the info on the skin pack
					// we have to retrieve the skuid from the store info, it can't be hardcoded since Sony may change it.
					// So we assume the first sku for the product is the one we want

					// MGH -  keyname in the DLC file is 16 chars long, but there's no space for a NULL terminating char
					memset(chKeyName, 0, sizeof(chKeyName));
					strncpy(chKeyName, pSONYDLCInfo->chDLCKeyname, 16);

					sprintf(chName,"%s-%s",app.GetCommerceCategory(),chKeyName);
					app.GetDLCSkuIDFromProductList(chName,chSkuID);

					// 4J-PB - need to check for an empty store
#if defined __PS3__
					if(app.CheckForEmptyStore(iPad)==false)
#endif
					{
						if(app.DLCAlreadyPurchased(chSkuID))
						{
							app.DownloadAlreadyPurchased(chSkuID);
						}
						else
						{
							app.Checkout(chSkuID);	
						}
					}
				}
			}
		}
	}
#endif

	return 0;
}

int UIScene_PauseMenu::BuyTexturePack_SignInReturned(void *pParam,bool bContinue, int iPad)
{
	UIScene_PauseMenu* pClass = (UIScene_PauseMenu*)pParam;

	if(bContinue==true)
	{
		// Check if we're signed in to LIVE
		if(ProfileManager.IsSignedInLive(iPad))
		{
#if defined(__PS3__)

			// 4J-PB - need to check this user can access the store
			bool bContentRestricted=false;
			ProfileManager.GetChatAndContentRestrictions(iPad,true,NULL,&bContentRestricted,NULL);
			if(bContentRestricted)
			{
				UINT uiIDA[1];
				uiIDA[0]=IDS_CONFIRM_OK;
				ui.RequestMessageBox(IDS_ONLINE_SERVICE_TITLE, IDS_CONTENT_RESTRICTION, uiIDA, 1, iPad,NULL,pClass, app.GetStringTable(), NULL, 0, false);
			}
			else
			{
				// need to get info on the pack to see if the user has already downloaded it
				TexturePack *tPack = Minecraft::GetInstance()->skins->getSelected();
				DLCTexturePack *pDLCTexPack=(DLCTexturePack *)tPack;

				// retrieve the store name for the skin pack
				DLCPack *pDLCPack=pDLCTexPack->getDLCInfoParentPack();//tPack->getDLCPack();
				const char *pchPackName=wstringtofilename(pDLCPack->getName());
				app.DebugPrintf("Texture Pack - %s\n",pchPackName);
				SONYDLC *pSONYDLCInfo=app.GetSONYDLCInfo((char *)pchPackName);		

				if(pSONYDLCInfo!=NULL)
				{
					char chName[42];
					char chKeyName[20];
					char chSkuID[SCE_NP_COMMERCE2_SKU_ID_LEN];

					memset(chSkuID,0,SCE_NP_COMMERCE2_SKU_ID_LEN);
					// find the info on the skin pack
					// we have to retrieve the skuid from the store info, it can't be hardcoded since Sony may change it.
					// So we assume the first sku for the product is the one we want

					// MGH -  keyname in the DLC file is 16 chars long, but there's no space for a NULL terminating char
					memset(chKeyName, 0, sizeof(chKeyName));
					strncpy(chKeyName, pSONYDLCInfo->chDLCKeyname, 16);

					sprintf(chName,"%s-%s",app.GetCommerceCategory(),chKeyName);
					app.GetDLCSkuIDFromProductList(chName,chSkuID);

					// 4J-PB - need to check for an empty store
#if defined __PS3__
					if(app.CheckForEmptyStore(iPad)==false)
#endif
					{	
						if(app.DLCAlreadyPurchased(chSkuID))
						{
							app.DownloadAlreadyPurchased(chSkuID);
						}
						else
						{
							app.Checkout(chSkuID);	
						}
					}
				}
			}
#else
			// TO BE IMPEMENTED FOR ORBIS
#endif
		}
	}
	return 0;
}

int UIScene_PauseMenu::ViewInvites_SignInReturned(void *pParam,bool bContinue, int iPad)
{
	if(bContinue==true)
	{
		// Check if we're signed in to LIVE
		if(ProfileManager.IsSignedInLive(iPad))
		{
#if defined __PS3__
			int ret = sceNpBasicRecvMessageCustom(SCE_NP_BASIC_MESSAGE_MAIN_TYPE_INVITE, SCE_NP_BASIC_RECV_MESSAGE_OPTIONS_INCLUDE_BOOTABLE, SYS_MEMORY_CONTAINER_ID_INVALID);
			app.DebugPrintf("sceNpBasicRecvMessageCustom return %d ( %08x )\n", ret, ret);
#else
			PSVITA_STUBBED;
#endif
		}
	}
	return 0;
}


int UIScene_PauseMenu::ExitGameSaveDialogReturned(void *pParam,int iPad,C4JStorage::EMessageResult result)
{
	UIScene_PauseMenu *pClass = (UIScene_PauseMenu *)pParam;
	// Exit with or without saving
	// Decline means save in this dialog
	if(result==C4JStorage::EMessage_ResultDecline || result==C4JStorage::EMessage_ResultThirdOption) 
	{
		if( result==C4JStorage::EMessage_ResultDecline ) // Save
		{
			// 4J-PB - Is the player trying to save but they are using a trial texturepack ?
			if(!Minecraft::GetInstance()->skins->isUsingDefaultSkin())
			{
				TexturePack *tPack = Minecraft::GetInstance()->skins->getSelected();
				DLCTexturePack *pDLCTexPack=(DLCTexturePack *)tPack;

				DLCPack *pDLCPack=pDLCTexPack->getDLCInfoParentPack();//tPack->getDLCPack();
				if(!pDLCPack->hasPurchasedFile( DLCManager::e_DLCType_Texture, L"" ))
				{					

					UINT uiIDA[2];
					uiIDA[0]=IDS_CONFIRM_OK;
					uiIDA[1]=IDS_CONFIRM_CANCEL;

					// Give the player a warning about the trial version of the texture pack
					ui.RequestMessageBox(IDS_WARNING_DLC_TRIALTEXTUREPACK_TITLE, IDS_WARNING_DLC_TRIALTEXTUREPACK_TEXT, uiIDA, 2, ProfileManager.GetPrimaryPad() ,&UIScene_PauseMenu::WarningTrialTexturePackReturned, dynamic_cast<IUIScene_PauseMenu*>(pClass),app.GetStringTable(), NULL, 0, false);

					return S_OK;					
				}
			}

			// does the save exist?
			bool bSaveExists;
			StorageManager.DoesSaveExist(&bSaveExists);
			// 4J-PB - we check if the save exists inside the libs
			// we need to ask if they are sure they want to overwrite the existing game
			if(bSaveExists)
			{
				UINT uiIDA[2];
				uiIDA[0]=IDS_CONFIRM_CANCEL;
				uiIDA[1]=IDS_CONFIRM_OK;
				ui.RequestMessageBox(IDS_TITLE_SAVE_GAME, IDS_CONFIRM_SAVE_GAME, uiIDA, 2, ProfileManager.GetPrimaryPad(),&IUIScene_PauseMenu::ExitGameAndSaveReturned, dynamic_cast<IUIScene_PauseMenu*>(pClass), app.GetStringTable(), NULL, 0, false);
				return 0;
			}
			else
			{
				MinecraftServer::getInstance()->setSaveOnExit( true );
			}
		}
		else
		{
			// been a few requests for a confirm on exit without saving
			UINT uiIDA[2];
			uiIDA[0]=IDS_CONFIRM_CANCEL;
			uiIDA[1]=IDS_CONFIRM_OK;
			ui.RequestMessageBox(IDS_TITLE_DECLINE_SAVE_GAME, IDS_CONFIRM_DECLINE_SAVE_GAME, uiIDA, 2, ProfileManager.GetPrimaryPad(),&IUIScene_PauseMenu::ExitGameDeclineSaveReturned, dynamic_cast<IUIScene_PauseMenu*>(pClass), app.GetStringTable(), NULL, 0, false);
			return 0;
		}

		app.SetAction(iPad,eAppAction_ExitWorld);
	}
	return 0;
}

#endif

void UIScene_PauseMenu::SetIgnoreInput(bool ignoreInput)
{
	m_bIgnoreInput = ignoreInput;
}

