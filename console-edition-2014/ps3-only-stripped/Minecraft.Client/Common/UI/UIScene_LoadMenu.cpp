#include "stdafx.h"
#include "UI.h"
#include "UIScene_LoadMenu.h"
#include "..\..\Minecraft.h"
#include "..\..\TexturePackRepository.h"
#include "..\..\Options.h"
#include "..\..\MinecraftServer.h"
#include "..\..\..\Minecraft.World\LevelSettings.h"
#include "..\..\..\Minecraft.World\StringHelpers.h"
#if defined(__PS3__)
#include "Common\Network\Sony\SonyHttp.h"
#endif
#include "..\..\DLCTexturePack.h"

#define GAME_CREATE_ONLINE_TIMER_ID 0
#define GAME_CREATE_ONLINE_TIMER_TIME 100
// 4J-PB - Only Xbox will not have trial DLC patched into the game

int UIScene_LoadMenu::m_iDifficultyTitleSettingA[4]=
{
	IDS_DIFFICULTY_TITLE_PEACEFUL,
	IDS_DIFFICULTY_TITLE_EASY,
	IDS_DIFFICULTY_TITLE_NORMAL,
	IDS_DIFFICULTY_TITLE_HARD
};

int UIScene_LoadMenu::LoadSaveDataThumbnailReturned(LPVOID lpParam,PBYTE pbThumbnail,DWORD dwThumbnailBytes)
{
	UIScene_LoadMenu *pClass= (UIScene_LoadMenu *)lpParam;

	app.DebugPrintf("Received data for a thumbnail\n");

	if(pbThumbnail && dwThumbnailBytes)
	{
		pClass->registerSubstitutionTexture(pClass->m_thumbnailName,pbThumbnail,dwThumbnailBytes);

		pClass->m_pbThumbnailData = pbThumbnail;
		pClass->m_uiThumbnailSize = dwThumbnailBytes;
		pClass->m_bSaveThumbnailReady = true;
	}
	else
	{
		app.DebugPrintf("Thumbnail data is NULL, or has size 0\n");
		pClass->m_bThumbnailGetFailed = true;
	}
	pClass->m_bRetrievingSaveThumbnail = false;

	return 0;
}

UIScene_LoadMenu::UIScene_LoadMenu(int iPad, void *initData, UILayer *parentLayer) : IUIScene_StartGame(iPad, parentLayer)
{
	// Setup all the Iggy references we need for this scene
	initialiseMovie();

	LoadMenuInitData *params = (LoadMenuInitData *)initData;

	//m_labelGameName.init(app.GetString(IDS_WORLD_NAME));
	m_labelSeed.init(L"");
	m_labelCreatedMode.init(app.GetString(IDS_CREATED_IN_SURVIVAL));

	m_buttonGamemode.init(app.GetString(IDS_GAMEMODE_SURVIVAL),eControl_GameMode);
	m_buttonMoreOptions.init(app.GetString(IDS_MORE_OPTIONS),eControl_MoreOptions);
	m_buttonLoadWorld.init(app.GetString(IDS_LOAD),eControl_LoadWorld);
	m_texturePackList.init(app.GetString(IDS_DLC_MENU_TEXTUREPACKS), eControl_TexturePackList);

	m_labelTexturePackName.init(L"");
	m_labelTexturePackDescription.init(L"");

	m_CurrentDifficulty=app.GetGameSettings(m_iPad,eGameSetting_Difficulty);
	WCHAR TempString[256];
	swprintf( (WCHAR *)TempString, 256, L"%ls: %ls", app.GetString( IDS_SLIDER_DIFFICULTY ),app.GetString(m_iDifficultyTitleSettingA[app.GetGameSettings(m_iPad,eGameSetting_Difficulty)]));	
	m_sliderDifficulty.init(TempString,eControl_Difficulty,0,3,app.GetGameSettings(m_iPad,eGameSetting_Difficulty));

	m_MoreOptionsParams.bGenerateOptions=FALSE;
	m_MoreOptionsParams.bPVP = TRUE;
	m_MoreOptionsParams.bTrust = TRUE;
	m_MoreOptionsParams.bFireSpreads = TRUE;
	m_MoreOptionsParams.bHostPrivileges = FALSE;
	m_MoreOptionsParams.bTNT = TRUE;
	m_MoreOptionsParams.iPad = iPad;

	m_iSaveGameInfoIndex=params->iSaveGameInfoIndex;
	m_levelGen = params->levelGen;

	m_bGameModeSurvival=true;
	m_bHasBeenInCreative = false;

	m_bSaveThumbnailReady = false;
	m_bRetrievingSaveThumbnail = true;
	m_bShowTimer = false;
	m_pDLCPack = NULL;
	m_bAvailableTexturePacksChecked=false;
	m_bRequestQuadrantSignin = false;
	m_iTexturePacksNotInstalled=0;
	m_bRebuildTouchBoxes = false;
	m_bThumbnailGetFailed = false;
	m_seed = 0;

	m_bMultiplayerAllowed = ProfileManager.IsSignedInLive( m_iPad ) && ProfileManager.AllowedToPlayMultiplayer(m_iPad);
	// 4J-PB - read the settings for the online flag. We'll only save this setting if the user changed it.
	bool bGameSetting_Online=(app.GetGameSettings(m_iPad,eGameSetting_Online)!=0);
	m_MoreOptionsParams.bOnlineSettingChangedBySystem=false;

	// Set the text for friends of friends, and default to on
	if( m_bMultiplayerAllowed)
	{
		m_MoreOptionsParams.bOnlineGame = bGameSetting_Online?TRUE:FALSE;
		if(bGameSetting_Online)
		{
			m_MoreOptionsParams.bInviteOnly = (app.GetGameSettings(m_iPad,eGameSetting_InviteOnly)!=0)?TRUE:FALSE;
			m_MoreOptionsParams.bAllowFriendsOfFriends = (app.GetGameSettings(m_iPad,eGameSetting_FriendsOfFriends)!=0)?TRUE:FALSE;
		}
		else
		{
			m_MoreOptionsParams.bInviteOnly = FALSE;
			m_MoreOptionsParams.bAllowFriendsOfFriends = FALSE;
		}
	}
	else
	{
		m_MoreOptionsParams.bOnlineGame = FALSE;
		m_MoreOptionsParams.bInviteOnly = FALSE;
		m_MoreOptionsParams.bAllowFriendsOfFriends = FALSE;
		if(bGameSetting_Online)
		{
			// The profile settings say Online, but either the player is offline, or they are not allowed to play online
			m_MoreOptionsParams.bOnlineSettingChangedBySystem=true;
		}	
	}

	

	// Level gen
	if(m_levelGen)
	{
		m_labelGameName.init(m_levelGen->getDisplayName());
		if(m_levelGen->requiresTexturePack())
		{
			m_MoreOptionsParams.dwTexturePack = m_levelGen->getRequiredTexturePackId();

			m_texturePackList.setEnabled(false);


			// retrieve the save icon from the texture pack, if there is one
			TexturePack *tp = Minecraft::GetInstance()->skins->getTexturePackById(m_MoreOptionsParams.dwTexturePack);
			DWORD dwImageBytes;
			PBYTE pbImageData = tp->getPackIcon(dwImageBytes);

			if(dwImageBytes > 0 && pbImageData)
			{
				wchar_t textureName[64];
				swprintf(textureName,64,L"loadsave");				
				registerSubstitutionTexture(textureName,pbImageData,dwImageBytes);
				m_bitmapIcon.setTextureName( textureName );
			}
		}
		// Set this level as created in creative mode, so that people can't use the themed worlds as an easy way to get achievements
		m_bHasBeenInCreative = true;
		m_labelCreatedMode.setLabel( app.GetString(IDS_CREATED_IN_CREATIVE) );
	}
	else
	{

#if defined(__PS3__)
		// convert to utf16
		uint16_t u16Message[MAX_SAVEFILENAME_LENGTH];
		size_t srclen,dstlen;
		srclen=MAX_SAVEFILENAME_LENGTH;
		dstlen=MAX_SAVEFILENAME_LENGTH;
#if defined(__PS3__)
		L10nResult lres= UTF8stoUTF16s((uint8_t *)params->saveDetails->UTF8SaveFilename,&srclen,u16Message,&dstlen);
#else
		{
			SceCesUcsContext Context;
			sceCesUcsContextInit( &Context );
			uint32_t utf8Len, utf16Len;
			sceCesUtf8StrToUtf16Str(&Context, (uint8_t *)params->saveDetails->UTF8SaveFilename, srclen, &utf8Len, u16Message, dstlen, &utf16Len);
		}
#endif
		m_thumbnailName = (wchar_t *)u16Message;
		if(params->saveDetails->pbThumbnailData)
		{
			m_pbThumbnailData = params->saveDetails->pbThumbnailData;
			m_uiThumbnailSize = params->saveDetails->dwThumbnailSize;
			m_bSaveThumbnailReady = true;
		}
		else
		{
			app.DebugPrintf("Requesting the save thumbnail\n");
			// set the save to load
			PSAVE_DETAILS pSaveDetails=StorageManager.ReturnSavesInfo();
			C4JStorage::ESaveGameState eLoadStatus=StorageManager.LoadSaveDataThumbnail(&pSaveDetails->SaveInfoA[(int)m_iSaveGameInfoIndex],&LoadSaveDataThumbnailReturned,this);
			m_bShowTimer = true;
		}

		m_labelGameName.init(params->saveDetails->UTF8SaveName);
#endif
	}

	TelemetryManager->RecordMenuShown(m_iPad, eUIScene_LoadMenu, 0);
	m_iTexturePacksNotInstalled=0;

	// block input if we're waiting for DLC to install, and wipe the saves list. The end of dlc mounting custom message will fill the list again
	if(app.StartInstallDLCProcess(m_iPad)==true)
	{
		// not doing a mount, so enable input
		m_bIgnoreInput=true;
	}
	else
	{
		m_bIgnoreInput = false;

		Minecraft *pMinecraft = Minecraft::GetInstance();
		int texturePacksCount = pMinecraft->skins->getTexturePackCount();
		for(unsigned int i = 0; i < texturePacksCount; ++i)
		{
			TexturePack *tp = pMinecraft->skins->getTexturePackByIndex(i);

			DWORD dwImageBytes;
			PBYTE pbImageData = tp->getPackIcon(dwImageBytes);

			if(dwImageBytes > 0 && pbImageData)
			{
				wchar_t imageName[64];
				swprintf(imageName,64,L"tpack%08x",tp->getId());
				registerSubstitutionTexture(imageName, pbImageData, dwImageBytes);
				m_texturePackList.addPack(i,imageName);
			}
		}
		m_currentTexturePackIndex = pMinecraft->skins->getTexturePackIndex(m_MoreOptionsParams.dwTexturePack);
		UpdateTexturePackDescription(m_currentTexturePackIndex);
		m_texturePackList.selectSlot(m_currentTexturePackIndex);

		// 4J-PB - Only Xbox will not have trial DLC patched into the game
	}


	if(params) delete params;
	addTimer(GAME_CREATE_ONLINE_TIMER_ID,GAME_CREATE_ONLINE_TIMER_TIME);
}

void UIScene_LoadMenu::updateTooltips()
{
	ui.SetTooltips( DEFAULT_XUI_MENU_USER, IDS_TOOLTIPS_SELECT,IDS_TOOLTIPS_BACK, -1, -1);
}

void UIScene_LoadMenu::updateComponents()
{
	m_parentLayer->showComponent(m_iPad,eUIComponent_Panorama,true);

	if(RenderManager.IsWidescreen())
	{
		m_parentLayer->showComponent(m_iPad,eUIComponent_Logo,true);
	}
	else
	{
		m_parentLayer->showComponent(m_iPad,eUIComponent_Logo,false);
	}
}

wstring UIScene_LoadMenu::getMoviePath()
{
	return L"LoadMenu";
}

UIControl* UIScene_LoadMenu::GetMainPanel()
{
	return &m_controlMainPanel;
}

void UIScene_LoadMenu::tick()
{
	if(m_bShowTimer)
	{
		m_bShowTimer = false;
		ui.NavigateToScene(m_iPad, eUIScene_Timer);
	}

	if( m_bThumbnailGetFailed )
	{
		// On Durango, this can happen if a save is still not been synchronised (user cancelled, or some error). Return back to give them a choice to pick another save.
		ui.NavigateBack(m_iPad, false, eUIScene_LoadOrJoinMenu);
		return;
	}

	if( m_bSaveThumbnailReady )
	{
		m_bSaveThumbnailReady = false;

		m_bitmapIcon.setTextureName( m_thumbnailName.c_str() );

		// retrieve the seed value from the image metadata
		bool bHostOptionsRead = false;
		unsigned int uiHostOptions = 0;

		char szSeed[50];
		ZeroMemory(szSeed,50);
		app.GetImageTextData(m_pbThumbnailData,m_uiThumbnailSize,(unsigned char *)&szSeed,uiHostOptions,bHostOptionsRead,m_MoreOptionsParams.dwTexturePack);


		// #ifdef _DEBUG
		// 			// dump out the thumbnail
		// 			HANDLE hThumbnail = CreateFile("GAME:\\thumbnail.png", GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_FLAG_RANDOM_ACCESS, NULL);
		// 			DWORD dwBytes;
		// 			WriteFile(hThumbnail,pbImageData,dwImageBytes,&dwBytes,NULL);
		// 			XCloseHandle(hThumbnail);
		// #endif

		if(szSeed[0]!=0)
		{
			WCHAR TempString[256];
			swprintf( (WCHAR *)TempString, 256, L"%ls: %hs", app.GetString( IDS_SEED ),szSeed);	
			m_labelSeed.setLabel(TempString);
		}
		else
		{
			m_labelSeed.setLabel(L"");
		}

		// Setup all the text and checkboxes to match what the game was saved with on
		if(bHostOptionsRead)
		{
			m_MoreOptionsParams.bPVP = app.GetGameHostOption(uiHostOptions,eGameHostOption_PvP)>0?TRUE:FALSE;
			m_MoreOptionsParams.bTrust = app.GetGameHostOption(uiHostOptions,eGameHostOption_TrustPlayers)>0?TRUE:FALSE;
			m_MoreOptionsParams.bFireSpreads = app.GetGameHostOption(uiHostOptions,eGameHostOption_FireSpreads)>0?TRUE:FALSE;
			m_MoreOptionsParams.bTNT = app.GetGameHostOption(uiHostOptions,eGameHostOption_TNT)>0?TRUE:FALSE;
			m_MoreOptionsParams.bHostPrivileges = app.GetGameHostOption(uiHostOptions,eGameHostOption_CheatsEnabled)>0?TRUE:FALSE;
			m_MoreOptionsParams.bDisableSaving = app.GetGameHostOption(uiHostOptions,eGameHostOption_DisableSaving)>0?TRUE:FALSE;

			// turn off creative mode on the save
			// #ifdef _DEBUG
			//  			uiHostOptions&=~GAME_HOST_OPTION_BITMASK_BEENINCREATIVE;
			//  			app.SetGameHostOption(eGameHostOption_HasBeenInCreative, 0);
			// #endif

			m_bHasBeenInCreative = app.GetGameHostOption(uiHostOptions,eGameHostOption_HasBeenInCreative)>0;
			if(app.GetGameHostOption(uiHostOptions,eGameHostOption_HasBeenInCreative)>0)
			{
				m_labelCreatedMode.setLabel( app.GetString(IDS_CREATED_IN_CREATIVE) );
			}
			else
			{
				m_labelCreatedMode.setLabel( app.GetString(IDS_CREATED_IN_SURVIVAL) );
			}

			if(app.GetGameHostOption(uiHostOptions,eGameHostOption_GameType)>0)
			{			
				m_buttonGamemode.setLabel(app.GetString(IDS_GAMEMODE_CREATIVE));
				m_bGameModeSurvival=false;
			}

			bool bGameSetting_Online=(app.GetGameSettings(m_iPad,eGameSetting_Online)!=0);
			if(app.GetGameHostOption(uiHostOptions,eGameHostOption_FriendsOfFriends) && !(m_bMultiplayerAllowed && bGameSetting_Online))
			{
				m_MoreOptionsParams.bAllowFriendsOfFriends = TRUE;
			}
		}

		Minecraft *pMinecraft = Minecraft::GetInstance();
		m_currentTexturePackIndex = pMinecraft->skins->getTexturePackIndex(m_MoreOptionsParams.dwTexturePack);

		UpdateTexturePackDescription(m_currentTexturePackIndex);

		m_texturePackList.selectSlot(m_currentTexturePackIndex);

		//m_labelGameName.setLabel(m_XContentData.szDisplayName);

		ui.NavigateBack(m_iPad, false, getSceneType() );
	}

	if(m_iSetTexturePackDescription >= 0 )
	{
		UpdateTexturePackDescription( m_iSetTexturePackDescription );
		m_iSetTexturePackDescription = -1;
	}
	if(m_bShowTexturePackDescription)
	{
		slideLeft();
		m_texturePackDescDisplayed = true;

		m_bShowTexturePackDescription = false;
	}

	if(m_bRequestQuadrantSignin)
	{
		m_bRequestQuadrantSignin = false;
		SignInInfo info;
		info.Func = &UIScene_LoadMenu::StartGame_SignInReturned;
		info.lpParam = this;
		info.requireOnline = m_MoreOptionsParams.bOnlineGame;
		ui.NavigateToScene(ProfileManager.GetPrimaryPad(),eUIScene_QuadrantSignin,&info);
	}


	UIScene::tick();
}


void UIScene_LoadMenu::handleInput(int iPad, int key, bool repeat, bool pressed, bool released, bool &handled)
{
	if(m_bIgnoreInput) return;

	ui.AnimateKeyPress(m_iPad, key, repeat, pressed, released);

	switch(key)
	{
	case ACTION_MENU_CANCEL:
		if(pressed)
		{
			app.SetCorruptSaveDeleted(false);
			navigateBack();
			handled = true;
		}
		break;
	case ACTION_MENU_OK:
		
	// 4J-JEV: Inform user why their game must be offline.

	case ACTION_MENU_UP:
	case ACTION_MENU_DOWN:
	case ACTION_MENU_LEFT:
	case ACTION_MENU_RIGHT:
	case ACTION_MENU_OTHER_STICK_UP:
	case ACTION_MENU_OTHER_STICK_DOWN:
		sendInputToMovie(key, repeat, pressed, released);
		
		handled = true;
		break;
	}
}

void UIScene_LoadMenu::handlePress(F64 controlId, F64 childId)
{
	if(m_bIgnoreInput) return;

	//CD - Added for audio
	ui.PlayUISFX(eSFX_Press);

	switch((int)controlId)
	{
	case eControl_GameMode:
		if(m_bGameModeSurvival)
		{
			m_buttonGamemode.setLabel(app.GetString(IDS_GAMEMODE_CREATIVE));
			m_bGameModeSurvival=false;
		}
		else
		{
			m_buttonGamemode.setLabel(app.GetString(IDS_GAMEMODE_SURVIVAL));
			m_bGameModeSurvival=true;
		}
		break;
	case eControl_MoreOptions:
		ui.NavigateToScene(m_iPad, eUIScene_LaunchMoreOptionsMenu, &m_MoreOptionsParams);
		break;
	case eControl_TexturePackList:
		{
			UpdateCurrentTexturePack((int)childId);
		}
		break;
	case eControl_LoadWorld:
		{
			{
				StartSharedLaunchFlow();
			}
		}
		break;
	};
}


void UIScene_LoadMenu::StartSharedLaunchFlow()
{
	Minecraft *pMinecraft=Minecraft::GetInstance();
	// Check if we need to upsell the texture pack
	if(m_MoreOptionsParams.dwTexturePack!=0)
	{
		// texture pack hasn't been set yet, so check what it will be
		TexturePack *pTexturePack = pMinecraft->skins->getTexturePackById(m_MoreOptionsParams.dwTexturePack);

		if(pTexturePack==NULL)
		{
#if TO_BE_IMPLEMENTED
			// They've selected a texture pack they don't have yet
			// upsell
			CXuiCtrl4JList::LIST_ITEM_INFO ListItem;
			// get the current index of the list, and then get the data
			ListItem=m_pTexturePacksList->GetData(m_currentTexturePackIndex);


			// upsell the texture pack
			// tell sentient about the upsell of the full version of the skin pack
			ULONGLONG ullOfferID_Full;
			app.GetDLCFullOfferIDForPackID(m_MoreOptionsParams.dwTexturePack,&ullOfferID_Full);

			TelemetryManager->RecordUpsellPresented(ProfileManager.GetPrimaryPad(), eSet_UpsellID_Texture_DLC, ullOfferID_Full & 0xFFFFFFFF);
#endif

			UINT uiIDA[2];

			uiIDA[0]=IDS_TEXTUREPACK_FULLVERSION;
			//uiIDA[1]=IDS_TEXTURE_PACK_TRIALVERSION;
			uiIDA[1]=IDS_CONFIRM_CANCEL;

			// Give the player a warning about the texture pack missing
			ui.RequestMessageBox(IDS_DLC_TEXTUREPACK_NOT_PRESENT_TITLE, IDS_DLC_TEXTUREPACK_NOT_PRESENT, uiIDA, 2, ProfileManager.GetPrimaryPad(),&TexturePackDialogReturned,this,app.GetStringTable(),NULL,0,false);
			return;
		}
	}
	m_bIgnoreInput = true;

	// if the profile data has been changed, then force a profile write (we save the online/invite/friends of friends settings)
	// It seems we're allowed to break the 5 minute rule if it's the result of a user action
	// check the checkboxes

	// Only save the online setting if the user changed it - we may change it because we're offline, but don't want that saved
	if(!m_MoreOptionsParams.bOnlineSettingChangedBySystem)
	{
		app.SetGameSettings(m_iPad,eGameSetting_Online,m_MoreOptionsParams.bOnlineGame?1:0);
	}
	app.SetGameSettings(m_iPad,eGameSetting_InviteOnly,m_MoreOptionsParams.bInviteOnly?1:0);
	app.SetGameSettings(m_iPad,eGameSetting_FriendsOfFriends,m_MoreOptionsParams.bAllowFriendsOfFriends?1:0);

	app.CheckGameSettingsChanged(true,m_iPad);

	// Check that we have the rights to use a texture pack we have selected.
	if(m_MoreOptionsParams.dwTexturePack!=0)
	{
		// texture pack hasn't been set yet, so check what it will be
		TexturePack *pTexturePack = pMinecraft->skins->getTexturePackById(m_MoreOptionsParams.dwTexturePack);
		DLCTexturePack *pDLCTexPack=(DLCTexturePack *)pTexturePack;
		m_pDLCPack=pDLCTexPack->getDLCInfoParentPack();

		// do we have a license?
		if(m_pDLCPack && !m_pDLCPack->hasPurchasedFile( DLCManager::e_DLCType_Texture, L"" ))
		{
			// no

			// We need to allow people to use a trial texture pack if they are offline - we only need them online if they want to buy it.

			/*
			UINT uiIDA[1];
			uiIDA[0]=IDS_OK;

			if(!ProfileManager.IsSignedInLive(m_iPad))
			{
				// need to be signed in to live
				ui.RequestMessageBox(IDS_PRO_NOTONLINE_TITLE, IDS_PRO_XBOXLIVE_NOTIFICATION, uiIDA, 1);
				m_bIgnoreInput = false;
				return;
			}
			else */
			{
				// upsell

#if defined(__PS3__)
				// trial pack warning
				UINT uiIDA[2];
				uiIDA[0]=IDS_CONFIRM_OK;
				uiIDA[1]=IDS_CONFIRM_CANCEL;
				ui.RequestMessageBox(IDS_WARNING_DLC_TRIALTEXTUREPACK_TITLE, IDS_USING_TRIAL_TEXUREPACK_WARNING, uiIDA, 2, m_iPad,&TrialTexturePackWarningReturned,this,app.GetStringTable(),NULL,0,false);
#endif

				return;
			}
		}			
	}


#if TO_BE_IMPLEMENTED
	// Reset the background downloading, in case we changed it by attempting to download a texture pack
	XBackgroundDownloadSetMode(XBACKGROUND_DOWNLOAD_MODE_AUTO);
#endif

	// Check if they have the Reset Nether flag set, and confirm they want to do this
	if(m_MoreOptionsParams.bResetNether==TRUE)
	{
		UINT uiIDA[2];
		uiIDA[0]=IDS_DONT_RESET_NETHER;
		uiIDA[1]=IDS_RESET_NETHER;

		ui.RequestMessageBox(IDS_RESETNETHER_TITLE, IDS_RESETNETHER_TEXT, uiIDA, 2, m_iPad,&UIScene_LoadMenu::CheckResetNetherReturned,this,app.GetStringTable(),NULL,0,false);
	}
	else
	{
		LaunchGame();
	}
}

void UIScene_LoadMenu::handleSliderMove(F64 sliderId, F64 currentValue)
{
	WCHAR TempString[256];
	int value = (int)currentValue;
	switch((int)sliderId)
	{
	case eControl_Difficulty:
		m_sliderDifficulty.handleSliderMove(value);

		app.SetGameSettings(m_iPad,eGameSetting_Difficulty,value);
		swprintf( (WCHAR *)TempString, 256, L"%ls: %ls", app.GetString( IDS_SLIDER_DIFFICULTY ),app.GetString(m_iDifficultyTitleSettingA[value]));		
		m_sliderDifficulty.setLabel(TempString);
		break;
	}
}

void UIScene_LoadMenu::handleTouchBoxRebuild()
{
	m_bRebuildTouchBoxes = true;
}


void UIScene_LoadMenu::handleTimerComplete(int id)
{

	switch(id)
	{
	case GAME_CREATE_ONLINE_TIMER_ID:
		{
			bool bMultiplayerAllowed = ProfileManager.IsSignedInLive( m_iPad ) && ProfileManager.AllowedToPlayMultiplayer(m_iPad);

			if(bMultiplayerAllowed != m_bMultiplayerAllowed)
			{
				if( bMultiplayerAllowed )
				{
					bool bGameSetting_Online=(app.GetGameSettings(m_iPad,eGameSetting_Online)!=0);
					m_MoreOptionsParams.bOnlineGame = bGameSetting_Online?TRUE:FALSE;
					if(bGameSetting_Online)
					{
						m_MoreOptionsParams.bInviteOnly = (app.GetGameSettings(m_iPad,eGameSetting_InviteOnly)!=0)?TRUE:FALSE;
						m_MoreOptionsParams.bAllowFriendsOfFriends = (app.GetGameSettings(m_iPad,eGameSetting_FriendsOfFriends)!=0)?TRUE:FALSE;
					}
					else
					{
						m_MoreOptionsParams.bInviteOnly = FALSE;
						m_MoreOptionsParams.bAllowFriendsOfFriends = FALSE;
					}
				}
				else
				{
					m_MoreOptionsParams.bOnlineGame = FALSE;
					m_MoreOptionsParams.bInviteOnly = FALSE;
					m_MoreOptionsParams.bAllowFriendsOfFriends = FALSE;
				}

				m_bMultiplayerAllowed = bMultiplayerAllowed;
			}
		}
		break;
		// 4J-PB - Only Xbox will not have trial DLC patched into the game
	}
}

void UIScene_LoadMenu::LaunchGame(void)
{
	// stop the timer running that causes a check for new texture packs in TMS but not installed, since this will run all through the load game, and will crash if it tries to create an hbrush

	if( (m_bGameModeSurvival != true || m_bHasBeenInCreative) || m_MoreOptionsParams.bHostPrivileges == TRUE)
	{			
		UINT uiIDA[2];
		uiIDA[0]=IDS_CONFIRM_OK;
		uiIDA[1]=IDS_CONFIRM_CANCEL;
		if(m_bGameModeSurvival != true || m_bHasBeenInCreative)
		{
			// 4J-PB - Need different text for Survival mode with a level that has been saved in Creative
			if(m_bGameModeSurvival)
			{
				ui.RequestMessageBox(IDS_TITLE_START_GAME, IDS_CONFIRM_START_SAVEDINCREATIVE, uiIDA, 2, m_iPad,&UIScene_LoadMenu::ConfirmLoadReturned,this,app.GetStringTable(),NULL,0,false);
			}
			else // it's creative mode
			{
				// has it previously been saved in creative?
				if(m_bHasBeenInCreative)
				{
					// 4J-PB - We don't really need to tell the user this will have achievements disabled, since they already saved it in creative
					// and they got the warning then
					// inform them that leaderboard writes and achievements will be disabled
					//ui.RequestMessageBox(IDS_TITLE_START_GAME, IDS_CONFIRM_START_SAVEDINCREATIVE_CONTINUE, uiIDA, 1, m_iPad,&CScene_LoadGameSettings::ConfirmLoadReturned,this,app.GetStringTable());

					if(m_levelGen != NULL)
					{
						LoadLevelGen(m_levelGen);
					}
					else
					{

						// set the save to load
						PSAVE_DETAILS pSaveDetails=StorageManager.ReturnSavesInfo();
						app.DebugPrintf("Loading save s [%s]\n",pSaveDetails->SaveInfoA[(int)m_iSaveGameInfoIndex].UTF8SaveTitle,pSaveDetails->SaveInfoA[(int)m_iSaveGameInfoIndex].UTF8SaveFilename);
						C4JStorage::ESaveGameState eLoadStatus=StorageManager.LoadSaveData(&pSaveDetails->SaveInfoA[(int)m_iSaveGameInfoIndex],&LoadSaveDataReturned,this);

#if TO_BE_IMPLEMENTED
						if(eLoadStatus==C4JStorage::ELoadGame_DeviceRemoved)
						{
							// disable saving 
							StorageManager.SetSaveDisabled(true);
							StorageManager.SetSaveDeviceSelected(m_iPad,false);
							UINT uiIDA[1];
							uiIDA[0]=IDS_OK;
							ui.RequestMessageBox(IDS_STORAGEDEVICEPROBLEM_TITLE, IDS_FAILED_TO_LOADSAVE_TEXT, uiIDA, 1, m_iPad,&CScene_LoadGameSettings::DeviceRemovedDialogReturned,this);

						}
#endif
					}
				}
				else
				{
					// ask if they're sure they want to turn this into a creative map
					ui.RequestMessageBox(IDS_TITLE_START_GAME, IDS_CONFIRM_START_CREATIVE, uiIDA, 2, m_iPad,&UIScene_LoadMenu::ConfirmLoadReturned,this,app.GetStringTable(),NULL,0,false);
				}
			}
		}
		else
		{
			ui.RequestMessageBox(IDS_TITLE_START_GAME, IDS_CONFIRM_START_HOST_PRIVILEGES, uiIDA, 2, m_iPad,&UIScene_LoadMenu::ConfirmLoadReturned,this,app.GetStringTable(),NULL,0,false);
		}
	}
	else
	{
		if(m_levelGen != NULL)
		{
			LoadLevelGen(m_levelGen);
		}
		else
		{
			// set the save to load
			PSAVE_DETAILS pSaveDetails=StorageManager.ReturnSavesInfo();
			app.DebugPrintf("Loading save %s [%s]\n",pSaveDetails->SaveInfoA[(int)m_iSaveGameInfoIndex].UTF8SaveTitle,pSaveDetails->SaveInfoA[(int)m_iSaveGameInfoIndex].UTF8SaveFilename);
			C4JStorage::ESaveGameState eLoadStatus=StorageManager.LoadSaveData(&pSaveDetails->SaveInfoA[(int)m_iSaveGameInfoIndex],&LoadSaveDataReturned,this);

#if TO_BE_IMPLEMENTED
			if(eLoadStatus==C4JStorage::ELoadGame_DeviceRemoved)
			{
				// disable saving 
				StorageManager.SetSaveDisabled(true);
				StorageManager.SetSaveDeviceSelected(m_iPad,false);
				UINT uiIDA[1];
				uiIDA[0]=IDS_OK;
				ui.RequestMessageBox(IDS_STORAGEDEVICEPROBLEM_TITLE, IDS_FAILED_TO_LOADSAVE_TEXT, uiIDA, 1, m_iPad,&CScene_LoadGameSettings::DeviceRemovedDialogReturned,this);
			}
#endif
		}
	}
	//return 0;
}

int UIScene_LoadMenu::CheckResetNetherReturned(void *pParam,int iPad,C4JStorage::EMessageResult result)
{
	UIScene_LoadMenu* pClass = (UIScene_LoadMenu*)pParam;

	// results switched for this dialog
	if(result==C4JStorage::EMessage_ResultDecline) 
	{
		// continue and reset the nether
		pClass->LaunchGame();
	}
	else if(result==C4JStorage::EMessage_ResultAccept)
	{
		// turn off the reset nether and continue
		pClass->m_MoreOptionsParams.bResetNether=FALSE;
		pClass->LaunchGame();
	}
	else
	{
		// else they chose cancel
		pClass->m_bIgnoreInput=false;
	}
	return 0;
}

int UIScene_LoadMenu::ConfirmLoadReturned(void *pParam,int iPad,C4JStorage::EMessageResult result)
{
	UIScene_LoadMenu* pClass = (UIScene_LoadMenu*)pParam;

	if(result==C4JStorage::EMessage_ResultAccept) 
	{
		if(pClass->m_levelGen != NULL)
		{
			pClass->LoadLevelGen(pClass->m_levelGen);
		}
		else
		{
			// set the save to load
			PSAVE_DETAILS pSaveDetails=StorageManager.ReturnSavesInfo();
			app.DebugPrintf("Loading save %s [%s]\n",pSaveDetails->SaveInfoA[(int)pClass->m_iSaveGameInfoIndex].UTF8SaveTitle,pSaveDetails->SaveInfoA[(int)pClass->m_iSaveGameInfoIndex].UTF8SaveFilename);
			C4JStorage::ESaveGameState eLoadStatus=StorageManager.LoadSaveData(&pSaveDetails->SaveInfoA[(int)pClass->m_iSaveGameInfoIndex],&LoadSaveDataReturned,pClass);

#if TO_BE_IMPLEMENTED
			if(eLoadStatus==C4JStorage::ELoadGame_DeviceRemoved)
			{
				// disable saving 
				StorageManager.SetSaveDisabled(true);
				StorageManager.SetSaveDeviceSelected(m_iPad,false);
				UINT uiIDA[1];
				uiIDA[0]=IDS_OK;
				ui.RequestMessageBox(IDS_STORAGEDEVICEPROBLEM_TITLE, IDS_FAILED_TO_LOADSAVE_TEXT, uiIDA, 1, m_iPad,&CScene_LoadGameSettings::DeviceRemovedDialogReturned,this);
			}
#endif		
		}
	}
	else
	{
		pClass->m_bIgnoreInput=false;
	}
	return 0;
}

int UIScene_LoadMenu::LoadDataComplete(void *pParam)
{
	UIScene_LoadMenu* pClass = (UIScene_LoadMenu*)pParam;

	if(!pClass->m_bIsCorrupt)
	{
		int iPrimaryPad = ProfileManager.GetPrimaryPad();
		bool isSignedInLive = true;
		bool isOnlineGame = pClass->m_MoreOptionsParams.bOnlineGame;
		int iPadNotSignedInLive = -1;
		bool isLocalMultiplayerAvailable = app.IsLocalMultiplayerAvailable();

		for(unsigned int i = 0; i < XUSER_MAX_COUNT; ++i)
		{
			if (ProfileManager.IsSignedIn(i) && ((i == iPrimaryPad) || isLocalMultiplayerAvailable))
			{
				if (isSignedInLive && !ProfileManager.IsSignedInLive(i))
				{
					// Record the first non signed in live pad
					iPadNotSignedInLive = i;
				}

				isSignedInLive = isSignedInLive && ProfileManager.IsSignedInLive(i);
			}
		}

		// If this is an online game but not all players are signed in to Live, stop!
		if (isOnlineGame && !isSignedInLive)
		{
			pClass->m_bIgnoreInput=false;
			UINT uiIDA[1];
			uiIDA[0]=IDS_CONFIRM_OK;
			ui.RequestMessageBox( IDS_PRO_NOTONLINE_TITLE, IDS_PRO_NOTONLINE_TEXT, uiIDA,1,ProfileManager.GetPrimaryPad(),NULL,NULL, app.GetStringTable(),NULL,0,false);
			return 0;
		}

		// Check if user-created content is allowed, as we cannot play multiplayer if it's not
		bool noUGC = false;
		BOOL pccAllowed = TRUE;
		BOOL pccFriendsAllowed = TRUE;
		bool bContentRestricted = false;
		ProfileManager.AllowedPlayerCreatedContent(ProfileManager.GetPrimaryPad(),false,&pccAllowed,&pccFriendsAllowed);
#if defined(__PS3__)
		if(isOnlineGame)
		{
			ProfileManager.GetChatAndContentRestrictions(ProfileManager.GetPrimaryPad(),false,NULL,&bContentRestricted,NULL);
		}
#endif

		noUGC = !pccAllowed && !pccFriendsAllowed;

		if(!isOnlineGame || !isLocalMultiplayerAvailable)
		{
			if(isOnlineGame && noUGC )
			{
				pClass->setVisible( true );

				ui.RequestUGCMessageBox();

				pClass->m_bIgnoreInput=false;
			}
			else if(isOnlineGame && bContentRestricted )
			{
				pClass->setVisible( true );

				ui.RequestContentRestrictedMessageBox();
				pClass->m_bIgnoreInput=false;
			}
			else
			{
				
				DWORD dwLocalUsersMask = CGameNetworkManager::GetLocalPlayerMask(ProfileManager.GetPrimaryPad());

				// No guest problems so we don't need to force a sign-in of players here
				StartGameFromSave(pClass, dwLocalUsersMask);
			}
		}
		else
		{
			// 4J-PB not sure why we aren't checking the content restriction for the main player here when multiple controllers are connected - adding now
			if(isOnlineGame && noUGC )
			{
				pClass->setVisible( true );
				ui.RequestUGCMessageBox();
				pClass->m_bIgnoreInput=false;
			}
			else if(isOnlineGame && bContentRestricted )
			{
				pClass->setVisible( true );
				ui.RequestContentRestrictedMessageBox();
				pClass->m_bIgnoreInput=false;
			}
			else
			{
				pClass->m_bRequestQuadrantSignin = true;
			}
		}
	}
	else
	{
		// the save is corrupt!
		pClass->m_bIgnoreInput=false;

		// give the option to delete the save
		UINT uiIDA[2];
		uiIDA[0]=IDS_CONFIRM_CANCEL;
		uiIDA[1]=IDS_CONFIRM_OK;
		ui.RequestMessageBox(IDS_CORRUPT_OR_DAMAGED_SAVE_TITLE, IDS_CORRUPT_OR_DAMAGED_SAVE_TEXT, uiIDA, 2, pClass->m_iPad,&UIScene_LoadMenu::DeleteSaveDialogReturned,pClass, app.GetStringTable(),NULL,0,false);

	}

	return 0;
}

int UIScene_LoadMenu::LoadSaveDataReturned(void *pParam,bool bIsCorrupt, bool bIsOwner)
{
	UIScene_LoadMenu* pClass = (UIScene_LoadMenu*)pParam;

	pClass->m_bIsCorrupt=bIsCorrupt;
	if(bIsOwner)
	{
		LoadDataComplete(pClass);
	}
	else
	{
		// messagebox
		pClass->m_bIgnoreInput=false;

#if defined(__PS3__)
		// show the message that trophies are disabled
		UINT uiIDA[1];
		uiIDA[0]=IDS_CONFIRM_OK;
		ui.RequestMessageBox(IDS_SAVEDATA_COPIED_TITLE, IDS_SAVEDATA_COPIED_TEXT, uiIDA, 1, 
			pClass->m_iPad,&UIScene_LoadMenu::TrophyDialogReturned,pClass, app.GetStringTable());
#endif
	}


	return 0;
}

int UIScene_LoadMenu::TrophyDialogReturned(void *pParam,int iPad,C4JStorage::EMessageResult result)
{
	UIScene_LoadMenu* pClass = (UIScene_LoadMenu*)pParam;
	return LoadDataComplete(pClass);
}

int UIScene_LoadMenu::DeleteSaveDialogReturned(void *pParam,int iPad,C4JStorage::EMessageResult result)
{
	UIScene_LoadMenu* pClass = (UIScene_LoadMenu*)pParam;

	// results switched for this dialog
	if(result==C4JStorage::EMessage_ResultDecline) 
	{
		PSAVE_DETAILS pSaveDetails=StorageManager.ReturnSavesInfo();
		StorageManager.DeleteSaveData(&pSaveDetails->SaveInfoA[(int)pClass->m_iSaveGameInfoIndex],UIScene_LoadMenu::DeleteSaveDataReturned,pClass);
	}
	else
	{
		pClass->m_bIgnoreInput=false;
	}
	return 0;
}

int UIScene_LoadMenu::DeleteSaveDataReturned(void *pParam,bool bSuccess)
{
	UIScene_LoadMenu* pClass = (UIScene_LoadMenu*)pParam;

	app.SetCorruptSaveDeleted(true);
	pClass->navigateBack();

	return 0;
}

// 4J Stu - Shared functionality that is the same whether we needed a quadrant sign-in or not
void UIScene_LoadMenu::StartGameFromSave(UIScene_LoadMenu* pClass, DWORD dwLocalUsersMask)
{
	INT saveOrCheckpointId = 0;
	bool validSave = StorageManager.GetSaveUniqueNumber(&saveOrCheckpointId);
	TelemetryManager->RecordLevelResume(pClass->m_iPad, eSen_FriendOrMatch_Playing_With_Invited_Friends, eSen_CompeteOrCoop_Coop_and_Competitive, app.GetGameSettings(pClass->m_iPad,eGameSetting_Difficulty), app.GetLocalPlayerCount(), g_NetworkManager.GetOnlinePlayerCount(), saveOrCheckpointId);

	bool isClientSide = ProfileManager.IsSignedInLive(ProfileManager.GetPrimaryPad()) && pClass->m_MoreOptionsParams.bOnlineGame;

	bool isPrivate = (app.GetGameSettings(pClass->m_iPad,eGameSetting_InviteOnly)>0)?true:false;

	PSAVE_DETAILS pSaveDetails=StorageManager.ReturnSavesInfo();

	NetworkGameInitData *param = new NetworkGameInitData();
	param->seed = pClass->m_seed;
	param->saveData = NULL;
	param->texturePackId = pClass->m_MoreOptionsParams.dwTexturePack;

	Minecraft *pMinecraft = Minecraft::GetInstance();
	pMinecraft->skins->selectTexturePackById(pClass->m_MoreOptionsParams.dwTexturePack);
	//pMinecraft->skins->updateUI();

	app.SetGameHostOption(eGameHostOption_Difficulty,Minecraft::GetInstance()->options->difficulty);
	app.SetGameHostOption(eGameHostOption_FriendsOfFriends,app.GetGameSettings(pClass->m_iPad,eGameSetting_FriendsOfFriends));
	app.SetGameHostOption(eGameHostOption_Gamertags,app.GetGameSettings(pClass->m_iPad,eGameSetting_GamertagsVisible));

	app.SetGameHostOption(eGameHostOption_BedrockFog,app.GetGameSettings(pClass->m_iPad,eGameSetting_BedrockFog)?1:0);

	app.SetGameHostOption(eGameHostOption_PvP,pClass->m_MoreOptionsParams.bPVP);
	app.SetGameHostOption(eGameHostOption_TrustPlayers,pClass->m_MoreOptionsParams.bTrust );
	app.SetGameHostOption(eGameHostOption_FireSpreads,pClass->m_MoreOptionsParams.bFireSpreads );
	app.SetGameHostOption(eGameHostOption_TNT,pClass->m_MoreOptionsParams.bTNT );
	app.SetGameHostOption(eGameHostOption_HostCanFly,pClass->m_MoreOptionsParams.bHostPrivileges);
	app.SetGameHostOption(eGameHostOption_HostCanChangeHunger,pClass->m_MoreOptionsParams.bHostPrivileges);
	app.SetGameHostOption(eGameHostOption_HostCanBeInvisible,pClass->m_MoreOptionsParams.bHostPrivileges );

	// flag if the user wants to reset the Nether to force a Fortress with netherwart etc.
	app.SetResetNether((pClass->m_MoreOptionsParams.bResetNether==TRUE)?true:false);
	// clear out the app's terrain features list
	app.ClearTerrainFeaturePosition();

	app.SetGameHostOption(eGameHostOption_GameType,pClass->m_bGameModeSurvival?GameType::SURVIVAL->getId():GameType::CREATIVE->getId() );

	g_NetworkManager.HostGame(dwLocalUsersMask,isClientSide,isPrivate,MINECRAFT_NET_MAX_PLAYERS,0);

	param->settings = app.GetGameHostOption( eGameHostOption_All );

	g_NetworkManager.FakeLocalPlayerJoined();

	LoadingInputParams *loadingParams = new LoadingInputParams();
	loadingParams->func = &CGameNetworkManager::RunNetworkGameThreadProc;
	loadingParams->lpParam = (LPVOID)param;

	// Reset the autosave time
	app.SetAutosaveTimerTime();

	UIFullscreenProgressCompletionData *completionData = new UIFullscreenProgressCompletionData();
	completionData->bShowBackground=TRUE;
	completionData->bShowLogo=TRUE;
	completionData->type = e_ProgressCompletion_CloseAllPlayersUIScenes;
	completionData->iPad = DEFAULT_XUI_MENU_USER;
	loadingParams->completionData = completionData;

	ui.NavigateToScene(ProfileManager.GetPrimaryPad(),eUIScene_FullscreenProgress, loadingParams);
}

void UIScene_LoadMenu::checkStateAndStartGame()
{
	// Check if they have the Reset Nether flag set, and confirm they want to do this
	if(m_MoreOptionsParams.bResetNether==TRUE)
	{
		UINT uiIDA[2];
		uiIDA[0]=IDS_DONT_RESET_NETHER;
		uiIDA[1]=IDS_RESET_NETHER;

		ui.RequestMessageBox(IDS_RESETNETHER_TITLE, IDS_RESETNETHER_TEXT, uiIDA, 2, m_iPad,&UIScene_LoadMenu::CheckResetNetherReturned,this,app.GetStringTable(),NULL,0,false);
	}
	else
	{
		LaunchGame();
	}
}

void UIScene_LoadMenu::LoadLevelGen(LevelGenerationOptions *levelGen)
{
	bool isClientSide = ProfileManager.IsSignedInLive(ProfileManager.GetPrimaryPad()) && m_MoreOptionsParams.bOnlineGame;

	// 4J Stu - If we only have one controller connected, then don't show the sign-in UI again
	DWORD connectedControllers = 0;
	for(unsigned int i = 0; i < XUSER_MAX_COUNT; ++i)
	{
		if( InputManager.IsPadConnected(i) || ProfileManager.IsSignedIn(i) ) ++connectedControllers;
	}

	if(!isClientSide || connectedControllers == 1 || !RenderManager.IsHiDef())
	{

		// Check if user-created content is allowed, as we cannot play multiplayer if it's not
		bool noUGC = false;
		BOOL pccAllowed = TRUE;
		BOOL pccFriendsAllowed = TRUE;

		ProfileManager.AllowedPlayerCreatedContent(ProfileManager.GetPrimaryPad(),false,&pccAllowed,&pccFriendsAllowed);
		if(!pccAllowed && !pccFriendsAllowed) noUGC = true;

		if(isClientSide && noUGC )
		{
			m_bIgnoreInput=false;
			UINT uiIDA[1];
			uiIDA[0]=IDS_CONFIRM_OK;
			ui.RequestMessageBox( IDS_FAILED_TO_CREATE_GAME_TITLE, IDS_NO_USER_CREATED_CONTENT_PRIVILEGE_CREATE, uiIDA,1,ProfileManager.GetPrimaryPad(),NULL,NULL, app.GetStringTable(),NULL,0,false);
			return;
		}

	}

	DWORD dwLocalUsersMask = 0;

	dwLocalUsersMask |= CGameNetworkManager::GetLocalPlayerMask(ProfileManager.GetPrimaryPad());
	// Load data from disc
	//File saveFile( L"Tutorial\\Tutorial" );
	//LoadSaveFromDisk(&saveFile);

	StorageManager.ResetSaveData();
	// Make our next save default to the name of the level
	StorageManager.SetSaveTitle(levelGen->getDefaultSaveName().c_str());

	bool isPrivate = (app.GetGameSettings(m_iPad,eGameSetting_InviteOnly)>0)?true:false;

	g_NetworkManager.HostGame(dwLocalUsersMask,isClientSide,isPrivate,MINECRAFT_NET_MAX_PLAYERS,0);

	NetworkGameInitData *param = new NetworkGameInitData();
	param->seed = 0;
	param->saveData = NULL;
	param->levelGen = levelGen;

	if(levelGen->requiresTexturePack())
	{
		param->texturePackId = levelGen->getRequiredTexturePackId();

		Minecraft *pMinecraft = Minecraft::GetInstance();
		pMinecraft->skins->selectTexturePackById(param->texturePackId);
		//pMinecraft->skins->updateUI();
	}


	app.SetGameHostOption(eGameHostOption_Difficulty,Minecraft::GetInstance()->options->difficulty);
	app.SetGameHostOption(eGameHostOption_FriendsOfFriends,app.GetGameSettings(m_iPad,eGameSetting_FriendsOfFriends));
	app.SetGameHostOption(eGameHostOption_Gamertags,app.GetGameSettings(m_iPad,eGameSetting_GamertagsVisible));

	app.SetGameHostOption(eGameHostOption_BedrockFog,app.GetGameSettings(m_iPad,eGameSetting_BedrockFog)?1:0);

	app.SetGameHostOption(eGameHostOption_PvP,m_MoreOptionsParams.bPVP);
	app.SetGameHostOption(eGameHostOption_TrustPlayers,m_MoreOptionsParams.bTrust );
	app.SetGameHostOption(eGameHostOption_FireSpreads,m_MoreOptionsParams.bFireSpreads );
	app.SetGameHostOption(eGameHostOption_TNT,m_MoreOptionsParams.bTNT );
	app.SetGameHostOption(eGameHostOption_HostCanFly,m_MoreOptionsParams.bHostPrivileges);
	app.SetGameHostOption(eGameHostOption_HostCanChangeHunger,m_MoreOptionsParams.bHostPrivileges);
	app.SetGameHostOption(eGameHostOption_HostCanBeInvisible,m_MoreOptionsParams.bHostPrivileges );

	// flag if the user wants to reset the Nether to force a Fortress with netherwart etc.
	app.SetResetNether((m_MoreOptionsParams.bResetNether==TRUE)?true:false);
	// clear out the app's terrain features list
	app.ClearTerrainFeaturePosition();

	app.SetGameHostOption(eGameHostOption_GameType,m_bGameModeSurvival?GameType::SURVIVAL->getId():GameType::CREATIVE->getId() );

	param->settings = app.GetGameHostOption( eGameHostOption_All );

	g_NetworkManager.FakeLocalPlayerJoined();

	LoadingInputParams *loadingParams = new LoadingInputParams();
	loadingParams->func = &CGameNetworkManager::RunNetworkGameThreadProc;
	loadingParams->lpParam = (LPVOID)param;

	// Reset the autosave time
	app.SetAutosaveTimerTime();

	UIFullscreenProgressCompletionData *completionData = new UIFullscreenProgressCompletionData();
	completionData->bShowBackground=TRUE;
	completionData->bShowLogo=TRUE;
	completionData->type = e_ProgressCompletion_CloseAllPlayersUIScenes;
	completionData->iPad = DEFAULT_XUI_MENU_USER;
	loadingParams->completionData = completionData;

	ui.NavigateToScene(ProfileManager.GetPrimaryPad(),eUIScene_FullscreenProgress, loadingParams);
}

int UIScene_LoadMenu::StartGame_SignInReturned(void *pParam,bool bContinue, int iPad)
{
	UIScene_LoadMenu* pClass = (UIScene_LoadMenu*)pParam;

	if(bContinue==true)
	{
		// It's possible that the player has not signed in - they can back out
		if(ProfileManager.IsSignedIn(pClass->m_iPad))
		{
			int primaryPad = ProfileManager.GetPrimaryPad();
			bool noPrivileges = false;
			DWORD dwLocalUsersMask = 0;
			bool isSignedInLive = ProfileManager.IsSignedInLive(primaryPad);
			bool isOnlineGame = pClass->m_MoreOptionsParams.bOnlineGame;
			int iPadNotSignedInLive = -1;
			bool isLocalMultiplayerAvailable = app.IsLocalMultiplayerAvailable();

			for(unsigned int i = 0; i < XUSER_MAX_COUNT; ++i)
			{
				if (ProfileManager.IsSignedIn(i) && ((i == primaryPad) || isLocalMultiplayerAvailable))
				{
					if (isSignedInLive && !ProfileManager.IsSignedInLive(i))
					{
						// Record the first non signed in live pad
						iPadNotSignedInLive = i;
					}

					if( !ProfileManager.AllowedToPlayMultiplayer(i) ) noPrivileges = true;
					dwLocalUsersMask |= CGameNetworkManager::GetLocalPlayerMask(i);
					isSignedInLive = isSignedInLive && ProfileManager.IsSignedInLive(i);
				}
			}

			// If this is an online game but not all players are signed in to Live, stop!
			if (isOnlineGame && !isSignedInLive)
			{
				pClass->m_bIgnoreInput=false;
				UINT uiIDA[1];
				uiIDA[0]=IDS_CONFIRM_OK;
				ui.RequestMessageBox( IDS_PRO_NOTONLINE_TITLE, IDS_PRO_NOTONLINE_TEXT, uiIDA,1,ProfileManager.GetPrimaryPad(),NULL,NULL, app.GetStringTable(),NULL,0,false);
				return 0;
			}

			// Check if user-created content is allowed, as we cannot play multiplayer if it's not
			bool noUGC = false;
			BOOL pccAllowed = TRUE;
			BOOL pccFriendsAllowed = TRUE;

			ProfileManager.AllowedPlayerCreatedContent(ProfileManager.GetPrimaryPad(),false,&pccAllowed,&pccFriendsAllowed);
			if(!pccAllowed && !pccFriendsAllowed) noUGC = true;

			if(isSignedInLive && isOnlineGame && (noPrivileges || noUGC) )
			{
				if( noUGC )
				{
					pClass->m_bIgnoreInput = false;
					pClass->setVisible( true );
					UINT uiIDA[1];
					uiIDA[0]=IDS_CONFIRM_OK;
					ui.RequestMessageBox( IDS_FAILED_TO_CREATE_GAME_TITLE, IDS_NO_USER_CREATED_CONTENT_PRIVILEGE_CREATE, uiIDA,1,ProfileManager.GetPrimaryPad(),NULL,NULL, app.GetStringTable(),NULL,0,false);
				}
				else
				{
					pClass->m_bIgnoreInput = false;
					pClass->setVisible( true );
					UINT uiIDA[1];
					uiIDA[0]=IDS_CONFIRM_OK;
					ui.RequestMessageBox( IDS_NO_MULTIPLAYER_PRIVILEGE_TITLE, IDS_NO_MULTIPLAYER_PRIVILEGE_HOST_TEXT, uiIDA,1,ProfileManager.GetPrimaryPad(),NULL,NULL, app.GetStringTable(),NULL,0,false);
				}
			}
			else
			{
				// This is NOT called from a storage manager thread, and is in fact called from the main thread in the Profile library tick. Therefore we use the main threads IntCache.
				StartGameFromSave(pClass, dwLocalUsersMask);
			}
		}
	}
	else
	{
		pClass->m_bIgnoreInput=false;
	}

	return 0;
}

void UIScene_LoadMenu::handleGainFocus(bool navBack)
{
	if(navBack)
	{
		
	}
}
