#include "stdafx.h"
#include "UI.h"
#include "UIScene_LoadOrJoinMenu.h"

#include "..\..\..\Minecraft.World\StringHelpers.h"
#include "..\..\..\Minecraft.World\net.minecraft.world.item.h"
#include "..\..\..\Minecraft.World\net.minecraft.world.level.h"
#include "..\..\..\Minecraft.World\net.minecraft.world.level.chunk.storage.h"
#include "..\..\..\Minecraft.World\ConsoleSaveFile.h"
#include "..\..\..\Minecraft.World\ConsoleSaveFileOriginal.h"
#include "..\..\..\Minecraft.World\ConsoleSaveFileSplit.h"
#include "..\..\ProgressRenderer.h"
#include "..\..\MinecraftServer.h"
#include "..\..\TexturePackRepository.h"
#include "..\..\TexturePack.h"
#include "..\Network\SessionInfo.h"
#if defined(__PS3__)
#include "Common\Network\Sony\SonyHttp.h"
#include "Common\Network\Sony\SonyRemoteStorage.h"
#endif


#ifdef SONY_REMOTE_STORAGE_DOWNLOAD
unsigned long UIScene_LoadOrJoinMenu::m_ulFileSize=0L;
wstring UIScene_LoadOrJoinMenu::m_wstrStageText=L"";
#endif


#define JOIN_LOAD_ONLINE_TIMER_ID 0
#define JOIN_LOAD_ONLINE_TIMER_TIME 100



int UIScene_LoadOrJoinMenu::LoadSaveDataThumbnailReturned(LPVOID lpParam,PBYTE pbThumbnail,DWORD dwThumbnailBytes)
{
    UIScene_LoadOrJoinMenu *pClass= (UIScene_LoadOrJoinMenu *)lpParam;

    app.DebugPrintf("Received data for save thumbnail\n");

    if(pbThumbnail && dwThumbnailBytes)
    {
        pClass->m_saveDetails[pClass->m_iRequestingThumbnailId].pbThumbnailData = new BYTE[dwThumbnailBytes];
        memcpy(pClass->m_saveDetails[pClass->m_iRequestingThumbnailId].pbThumbnailData, pbThumbnail, dwThumbnailBytes);
        pClass->m_saveDetails[pClass->m_iRequestingThumbnailId].dwThumbnailSize = dwThumbnailBytes;
    }
    else
    {
        pClass->m_saveDetails[pClass->m_iRequestingThumbnailId].pbThumbnailData = NULL;
        pClass->m_saveDetails[pClass->m_iRequestingThumbnailId].dwThumbnailSize = 0;
        app.DebugPrintf("Save thumbnail data is NULL, or has size 0\n");
    }
    pClass->m_bSaveThumbnailReady = true;

    return 0;
}

int UIScene_LoadOrJoinMenu::LoadSaveCallback(LPVOID lpParam,bool bRes)
{
    //UIScene_LoadOrJoinMenu *pClass= (UIScene_LoadOrJoinMenu *)lpParam;
    // Get the save data now
    if(bRes)
    {
        app.DebugPrintf("Loaded save OK\n");
    }
    return 0;
}

UIScene_LoadOrJoinMenu::UIScene_LoadOrJoinMenu(int iPad, void *initData, UILayer *parentLayer) : UIScene(iPad, parentLayer)
{
    // Setup all the Iggy references we need for this scene
    initialiseMovie();
    app.SetLiveLinkRequired( true );

    m_iRequestingThumbnailId = 0;
    m_iSaveInfoC=0;
    m_bIgnoreInput = false;
    m_bShowingPartyGamesOnly = false;
    m_bInParty = false;
    m_currentSessions = NULL;
    m_iState=e_SavesIdle;
    //m_bRetrievingSaveInfo=false;

    m_buttonListSaves.init(eControl_SavesList);
    m_buttonListGames.init(eControl_GamesList);

    m_labelSavesListTitle.init( app.GetString(IDS_START_GAME) );
    m_labelJoinListTitle.init( app.GetString(IDS_JOIN_GAME) );
    m_labelNoGames.init( app.GetString(IDS_NO_GAMES_FOUND) );
    m_labelNoGames.setVisible( false );
    m_controlSavesTimer.setVisible( true );
    m_controlJoinTimer.setVisible( true );


    m_bUpdateSaveSize = false;

    m_bAllLoaded = false;
    m_bRetrievingSaveThumbnails = false;
    m_bSaveThumbnailReady = false;
    m_bExitScene=false;
    m_pSaveDetails=NULL;
    m_bSavesDisplayed=false;
    m_saveDetails = NULL;
    m_iSaveDetailsCount = 0;
    m_iTexturePacksNotInstalled = 0;
	m_bCopying = false;
	m_bCopyingCancelled = false;

    m_bSaveTransferCancelled=false;
    m_bSaveTransferInProgress=false;
	m_eAction = eAction_None;

    m_bMultiplayerAllowed = ProfileManager.IsSignedInLive( m_iPad ) && ProfileManager.AllowedToPlayMultiplayer(m_iPad);



    int iLB = -1;


#if defined(__PS3__)
    // Always clear the saves when we enter this menu
    StorageManager.ClearSavesInfo();
#endif

    // block input if we're waiting for DLC to install, and wipe the saves list. The end of dlc mounting custom message will fill the list again
    if(app.StartInstallDLCProcess(m_iPad)==true || app.DLCInstallPending())
    {
        // if we're waiting for DLC to mount, don't fill the save list. The custom message on end of dlc mounting will do that
        m_bIgnoreInput = true;
    }
    else
    {
        Initialise();
    }


    UpdateGamesList();

    g_NetworkManager.SetSessionsUpdatedCallback( &UpdateGamesListCallback, this );

    m_initData= new JoinMenuInitData();

    // 4J Stu - Fix for #12530 -TCR 001 BAS Game Stability: Title will crash if the player disconnects while starting a new world and then opts to play the tutorial once they have been returned to the Main Menu.
    MinecraftServer::resetFlags();

    // If we're not ignoring input, then we aren't still waiting for the DLC to mount, and can now check for corrupt dlc. Otherwise this will happen when the dlc has finished mounting.
    if( !m_bIgnoreInput)
    {
        app.m_dlcManager.checkForCorruptDLCAndAlert();
    }

    // 4J-PB - Only Xbox will not have trial DLC patched into the game

#ifdef SONY_REMOTE_STORAGE_DOWNLOAD
    m_eSaveTransferState = eSaveTransfer_Idle;
#endif
}


UIScene_LoadOrJoinMenu::~UIScene_LoadOrJoinMenu()
{
    g_NetworkManager.SetSessionsUpdatedCallback( NULL, NULL );
    app.SetLiveLinkRequired( false );

    if(m_currentSessions)
    {
        for(AUTO_VAR(it, m_currentSessions->begin()); it < m_currentSessions->end(); ++it)
        {
            delete (*it);
        }
    }

#if TO_BE_IMPLEMENTED
    // Reset the background downloading, in case we changed it by attempting to download a texture pack
    XBackgroundDownloadSetMode(XBACKGROUND_DOWNLOAD_MODE_AUTO);
#endif

    if(m_saveDetails)
    {
        for(int i = 0; i < m_iSaveDetailsCount; ++i)
        {
            delete m_saveDetails[i].pbThumbnailData;
        }
        delete [] m_saveDetails;
    }
}

void UIScene_LoadOrJoinMenu::updateTooltips()
{
    // update the tooltips
    // if the saves list has focus, then we should show the Delete Save tooltip
    // if the games list has focus, then we should the the View Gamercard tooltip
    int iRB=-1;	
    int iY = -1;
    int iLB = -1;
    int iX=-1;
    if (DoesGamesListHaveFocus() && m_buttonListGames.getItemCount() > 0)
    {
        iY = IDS_TOOLTIPS_VIEW_GAMERCARD;
    }
    else if (DoesSavesListHaveFocus())
    {
        if((m_iDefaultButtonsC > 0) && (m_iSaveListIndex >= m_iDefaultButtonsC))
        {		
            if(StorageManager.GetSaveDisabled())
            {
                iRB=IDS_TOOLTIPS_DELETESAVE;
            }
            else
            {
                if(StorageManager.EnoughSpaceForAMinSaveGame())
                {
                    iRB=IDS_TOOLTIPS_SAVEOPTIONS;
                }
                else
                {
                    iRB=IDS_TOOLTIPS_DELETESAVE;
                }
            }
        }
    }
	else if(DoesMashUpWorldHaveFocus())
	{
		// If it's a mash-up pack world, give the Hide option
		iRB=IDS_TOOLTIPS_HIDE;
	}

    if(m_bInParty)
    {
        if( m_bShowingPartyGamesOnly ) iLB = IDS_TOOLTIPS_ALL_GAMES;
        else iLB = IDS_TOOLTIPS_PARTY_GAMES;
    }

#if defined(__PS3__)
    if(m_iPad == ProfileManager.GetPrimaryPad() ) iY = IDS_TOOLTIPS_GAME_INVITES;
#endif

    if(ProfileManager.IsFullVersion()==false )
    {
        iRB = -1;
    }
    else if(StorageManager.GetSaveDisabled())
    {
    }
    else
    {
#if defined SONY_REMOTE_STORAGE_DOWNLOAD
        // Is there a save from PS3 or PSVita available?
		// Sony asked that this be displayed at all times so users are aware of the functionality. We'll display some text when there's no save available
        //if(app.getRemoteStorage()->saveIsAvailable())
		{		
			bool bSignedInLive = ProfileManager.IsSignedInLive(m_iPad);
			if(bSignedInLive)
			{
				iX=IDS_TOOLTIPS_SAVETRANSFER_DOWNLOAD;
			}
		}
#else
        iX = IDS_TOOLTIPS_CHANGEDEVICE;
#endif
    }

    ui.SetTooltips( DEFAULT_XUI_MENU_USER, IDS_TOOLTIPS_SELECT, IDS_TOOLTIPS_BACK, iX, iY,-1,-1,iLB,iRB);
}

// 
void UIScene_LoadOrJoinMenu::Initialise()
{
    m_iSaveListIndex = 0;
	m_iGameListIndex = 0;

    m_iDefaultButtonsC = 0;
	m_iMashUpButtonsC=0;

    // Check if we're in the trial version
    if(ProfileManager.IsFullVersion()==false)
    {


        AddDefaultButtons();

#if TO_BE_IMPLEMENTED
        m_pSavesList->SetCurSelVisible(0);
#endif
    }
    else if(StorageManager.GetSaveDisabled())
    {
#if defined(__PS3__)
        GetSaveInfo();
#else

#if TO_BE_IMPLEMENTED
        if(StorageManager.GetSaveDeviceSelected(m_iPad))
#endif
        {
            // saving is disabled, but we should still be able to load from a selected save device



            GetSaveInfo();
        }
#if TO_BE_IMPLEMENTED
        else
        {
            AddDefaultButtons();
            m_controlSavesTimer.setVisible( false );
        }
#endif
#endif
    }
    else
    {
        // 4J-PB - we need to check that there is enough space left to create a copy of the save (for a rename)
        bool bCanRename = StorageManager.EnoughSpaceForAMinSaveGame();

        GetSaveInfo();
    }

    m_bIgnoreInput=false;
    app.m_dlcManager.checkForCorruptDLCAndAlert();
}

void UIScene_LoadOrJoinMenu::updateComponents()
{
    m_parentLayer->showComponent(m_iPad,eUIComponent_Panorama,true);
    m_parentLayer->showComponent(m_iPad,eUIComponent_Logo,true);
}

void UIScene_LoadOrJoinMenu::handleDestroy()
{

	// shut down the keyboard if it is displayed
#if defined __PS3__
	InputManager.DestroyKeyboard();
#endif
}

void UIScene_LoadOrJoinMenu::handleGainFocus(bool navBack)
{
    UIScene::handleGainFocus(navBack);

    updateTooltips();

    // Add load online timer
    addTimer(JOIN_LOAD_ONLINE_TIMER_ID,JOIN_LOAD_ONLINE_TIMER_TIME);

    if(navBack)
    {
        app.SetLiveLinkRequired( true );

        m_bMultiplayerAllowed = ProfileManager.IsSignedInLive( m_iPad ) && ProfileManager.AllowedToPlayMultiplayer(m_iPad); 

        // re-enable button presses
        m_bIgnoreInput=false;

        // block input if we're waiting for DLC to install, and wipe the saves list. The end of dlc mounting custom message will fill the list again
        if(app.StartInstallDLCProcess(m_iPad)==false)
        {
            // not doing a mount, so re-enable input
            m_bIgnoreInput=false;
        }
        else
        {
            m_bIgnoreInput=true;
            m_buttonListSaves.clearList();
            m_controlSavesTimer.setVisible(true);
        }

        if( m_bMultiplayerAllowed )
        {
#if TO_BE_IMPLEMENTED
            HXUICLASS hClassFullscreenProgress = XuiFindClass( L"CScene_FullscreenProgress" );
            HXUICLASS hClassConnectingProgress = XuiFindClass( L"CScene_ConnectingProgress" );

            // If we are navigating back from a full screen progress scene, then that means a connection attempt failed
            if( XuiIsInstanceOf( hSceneFrom, hClassFullscreenProgress ) || XuiIsInstanceOf( hSceneFrom, hClassConnectingProgress ) )
            {
                UpdateGamesList();
            }
#endif
        }
        else
        {
            m_buttonListGames.clearList();
            m_controlJoinTimer.setVisible(true);
            m_labelNoGames.setVisible(false);
#if TO_BE_IMPLEMENTED
            m_SavesList.InitFocus(m_iPad);
#endif
        }

        // are we back here because of a delete of a corrupt save?

        if(app.GetCorruptSaveDeleted())
        {
            // wipe the list and repopulate it
            m_iState=e_SavesRepopulateAfterDelete;
            app.SetCorruptSaveDeleted(false);
        }
    }
}

void UIScene_LoadOrJoinMenu::handleLoseFocus()
{
    // Kill load online timer
    killTimer(JOIN_LOAD_ONLINE_TIMER_ID);
}

wstring UIScene_LoadOrJoinMenu::getMoviePath()
{
    return L"LoadOrJoinMenu";
}

void UIScene_LoadOrJoinMenu::tick()
{
    UIScene::tick();

#if defined  __PS3__
    if(m_bExitScene) // navigate forward or back
    {
        if(!m_bRetrievingSaveThumbnails)
        {
            // need to wait for any callback retrieving thumbnail to complete
            navigateBack();
        }
    }
    // Stop loading thumbnails if we navigate forwards
    if(hasFocus(m_iPad))
    {
        // Display the saves if we have them
        if(!m_bSavesDisplayed)
        {
            m_pSaveDetails=StorageManager.ReturnSavesInfo();
            if(m_pSaveDetails!=NULL)
            {
                //CD - Fix - Adding define for ORBIS/XBOXONE

                AddDefaultButtons();
                m_bSavesDisplayed=true;
                UpdateGamesList();

                if(m_saveDetails!=NULL)
                {
                    for(unsigned int i = 0; i < m_iSaveDetailsCount; ++i)
                    {
                        if(m_saveDetails[i].pbThumbnailData!=NULL)
                        {
                            delete m_saveDetails[i].pbThumbnailData;
                        }
                    }
                    delete m_saveDetails;
                }
                m_saveDetails = new SaveListDetails[m_pSaveDetails->iSaveC];

                m_iSaveDetailsCount = m_pSaveDetails->iSaveC;
                for(unsigned int i = 0; i < m_pSaveDetails->iSaveC; ++i)
                {
                    m_buttonListSaves.addItem(m_pSaveDetails->SaveInfoA[i].UTF8SaveTitle, L"");

                    m_saveDetails[i].saveId = i;
                    memcpy(m_saveDetails[i].UTF8SaveName, m_pSaveDetails->SaveInfoA[i].UTF8SaveTitle, 128);
                    memcpy(m_saveDetails[i].UTF8SaveFilename, m_pSaveDetails->SaveInfoA[i].UTF8SaveFilename, MAX_SAVEFILENAME_LENGTH);
                }
                m_controlSavesTimer.setVisible( false );

                // set focus on the first button

			}
        }

        if(!m_bExitScene && m_bSavesDisplayed && !m_bRetrievingSaveThumbnails && !m_bAllLoaded)
        {
            if( m_iRequestingThumbnailId < (m_buttonListSaves.getItemCount() - m_iDefaultButtonsC ))
            {
                m_bRetrievingSaveThumbnails = true;
                app.DebugPrintf("Requesting the first thumbnail\n");
                // set the save to load
                PSAVE_DETAILS pSaveDetails=StorageManager.ReturnSavesInfo();
                C4JStorage::ESaveGameState eLoadStatus=StorageManager.LoadSaveDataThumbnail(&pSaveDetails->SaveInfoA[(int)m_iRequestingThumbnailId],&LoadSaveDataThumbnailReturned,this);

                if(eLoadStatus!=C4JStorage::ESaveGame_GetSaveThumbnail)
                {
                    // something went wrong
                    m_bRetrievingSaveThumbnails=false;
                    m_bAllLoaded = true;
                }
            }
        }
        else if (m_bSavesDisplayed && m_bSaveThumbnailReady)
        {
            m_bSaveThumbnailReady = false;

            // check we're not waiting to exit the scene
            if(!m_bExitScene)
            {
                // convert to utf16
                uint16_t u16Message[MAX_SAVEFILENAME_LENGTH];
#ifdef __PS3
                size_t srcmax,dstmax;
#else
                uint32_t srcmax,dstmax;
                uint32_t srclen,dstlen;
#endif
                srcmax=MAX_SAVEFILENAME_LENGTH;
                dstmax=MAX_SAVEFILENAME_LENGTH;

#if defined(__PS3__)
                L10nResult lres= UTF8stoUTF16s((uint8_t *)m_saveDetails[m_iRequestingThumbnailId].UTF8SaveFilename,&srcmax,u16Message,&dstmax);
#else
                SceCesUcsContext context;
                sceCesUcsContextInit(&context);

                sceCesUtf8StrToUtf16Str(&context, (uint8_t *)m_saveDetails[m_iRequestingThumbnailId].UTF8SaveFilename,srcmax,&srclen,u16Message,dstmax,&dstlen);
#endif
                if( m_saveDetails[m_iRequestingThumbnailId].pbThumbnailData )
                {
                    registerSubstitutionTexture((wchar_t *)u16Message,m_saveDetails[m_iRequestingThumbnailId].pbThumbnailData,m_saveDetails[m_iRequestingThumbnailId].dwThumbnailSize);
                }
                m_buttonListSaves.setTextureName(m_iRequestingThumbnailId + m_iDefaultButtonsC, (wchar_t *)u16Message);

                ++m_iRequestingThumbnailId;
                if( m_iRequestingThumbnailId < (m_buttonListSaves.getItemCount() - m_iDefaultButtonsC ))
                {
                    app.DebugPrintf("Requesting another thumbnail\n");
                    // set the save to load
                    PSAVE_DETAILS pSaveDetails=StorageManager.ReturnSavesInfo();
                    C4JStorage::ESaveGameState eLoadStatus=StorageManager.LoadSaveDataThumbnail(&pSaveDetails->SaveInfoA[(int)m_iRequestingThumbnailId],&LoadSaveDataThumbnailReturned,this);
                    if(eLoadStatus!=C4JStorage::ESaveGame_GetSaveThumbnail)
                    {
                        // something went wrong
                        m_bRetrievingSaveThumbnails=false;
                        m_bAllLoaded = true;
                    }
                }
                else
                {
                    m_bRetrievingSaveThumbnails = false;
                    m_bAllLoaded = true;
                }
            }
            else
            {
                // stop retrieving thumbnails, and exit
                m_bRetrievingSaveThumbnails = false;
            }
        }
    }

    switch(m_iState)
    {
    case e_SavesIdle:
        break;
    case e_SavesRepopulate:
        m_bIgnoreInput = false;
        m_iState=e_SavesIdle;
        m_bAllLoaded=false;
        m_bRetrievingSaveThumbnails=false;
        m_iRequestingThumbnailId = 0;
        GetSaveInfo();
        break;
	case e_SavesRepopulateAfterMashupHide:
        m_bIgnoreInput = false;
        m_iRequestingThumbnailId = 0;
        m_bAllLoaded=false;
        m_bRetrievingSaveThumbnails=false;
        m_bSavesDisplayed=false;
        m_iSaveInfoC=0;
        m_buttonListSaves.clearList();
        GetSaveInfo();
        m_iState=e_SavesIdle;
		break;
    case e_SavesRepopulateAfterDelete:
	case e_SavesRepopulateAfterTransferDownload:
        m_bIgnoreInput = false;
        m_iRequestingThumbnailId = 0;
        m_bAllLoaded=false;
        m_bRetrievingSaveThumbnails=false;
        m_bSavesDisplayed=false;
        m_iSaveInfoC=0;
        m_buttonListSaves.clearList();
        StorageManager.ClearSavesInfo();
        GetSaveInfo();
        m_iState=e_SavesIdle;
        break;
    }
#else
    if(!m_bSavesDisplayed)
    {
        AddDefaultButtons();
        m_bSavesDisplayed=true;
        m_controlSavesTimer.setVisible( false );
    }
#endif


    // SAVE TRANSFERS

}

void UIScene_LoadOrJoinMenu::GetSaveInfo()
{
    unsigned int uiSaveC=0;

    // This will return with the number retrieved in uiSaveC

    if(app.DebugSettingsOn() && app.GetLoadSavesFromFolderEnabled())
    {

        uiSaveC = 0;
        File savesDir(L"Saves");
        if( savesDir.exists() )
        {
            m_saves = savesDir.listFiles();
            uiSaveC = (unsigned int)m_saves->size();
        }
        // add the New Game and Tutorial after the saves list is retrieved, if there are any saves

        // Add two for New Game and Tutorial
        unsigned int listItems = uiSaveC;

        AddDefaultButtons();

        for(unsigned int i=0;i<listItems;i++)
        {

            wstring wName = m_saves->at(i)->getName();
            wchar_t *name = new wchar_t[wName.size()+1];
            for(unsigned int j = 0; j < wName.size(); ++j)
            {
                name[j] = wName[j];
            }
            name[wName.size()] = 0;
            m_buttonListSaves.addItem(name,L"");
        }
        m_bSavesDisplayed = true;
        m_bAllLoaded = true;
        m_bIgnoreInput = false;
    }
    else
    {
        // clear the saves list
        m_bSavesDisplayed = false; // we're blocking the exit from this scene until complete
        m_buttonListSaves.clearList();
        m_iSaveInfoC=0;
        m_controlSavesTimer.setVisible(true);

        m_pSaveDetails=StorageManager.ReturnSavesInfo();
        if(m_pSaveDetails==NULL)
        {
            C4JStorage::ESaveGameState eSGIStatus= StorageManager.GetSavesInfo(m_iPad,NULL,this,"save"); 
        }

#if TO_BE_IMPLEMENTED
        if(eSGIStatus==C4JStorage::ESGIStatus_NoSaves)
        {
            uiSaveC=0;
            m_controlSavesTimer.setVisible( false );
            m_SavesList.SetEnable(TRUE);
        }
#endif
    }

    return;
}

void UIScene_LoadOrJoinMenu::AddDefaultButtons()
{
    m_iDefaultButtonsC = 0;
	m_iMashUpButtonsC=0;
	m_generators.clear();

    m_buttonListSaves.addItem(app.GetString(IDS_CREATE_NEW_WORLD));
    m_iDefaultButtonsC++;

    int i = 0;

    for(AUTO_VAR(it, app.getLevelGenerators()->begin()); it != app.getLevelGenerators()->end(); ++it)
    {
        LevelGenerationOptions *levelGen = *it;

        // retrieve the save icon from the texture pack, if there is one
        unsigned int uiTexturePackID=levelGen->getRequiredTexturePackId();

		if(uiTexturePackID!=0)
		{
			unsigned int uiMashUpWorldsBitmask=app.GetMashupPackWorlds(m_iPad);

			if((uiMashUpWorldsBitmask & (1<<(uiTexturePackID-1024)))==0)
			{
				// this world is hidden, so skip
				continue;
			}
		}
		
		m_generators.push_back(levelGen);
		m_buttonListSaves.addItem(levelGen->getWorldName());

        if(uiTexturePackID!=0)
        {
			// increment the count of the mash-up pack worlds in the save list
			m_iMashUpButtonsC++;
            TexturePack *tp = Minecraft::GetInstance()->skins->getTexturePackById(levelGen->getRequiredTexturePackId());
            DWORD dwImageBytes;
            PBYTE pbImageData = tp->getPackIcon(dwImageBytes);

            if(dwImageBytes > 0 && pbImageData)
            {
                wchar_t imageName[64];
                swprintf(imageName,64,L"tpack%08x",tp->getId());
                registerSubstitutionTexture(imageName, pbImageData, dwImageBytes);
                m_buttonListSaves.setTextureName( m_buttonListSaves.getItemCount() - 1, imageName );
            }
        }

        ++i;
    }
    m_iDefaultButtonsC += i;
}

void UIScene_LoadOrJoinMenu::handleInput(int iPad, int key, bool repeat, bool pressed, bool released, bool &handled)
{
    if(m_bIgnoreInput) return;

    // if we're retrieving save info, ignore key presses
    if(!m_bSavesDisplayed) return;

    ui.AnimateKeyPress(m_iPad, key, repeat, pressed, released);

    switch(key)
    {
    case ACTION_MENU_CANCEL:
        if(pressed)
        {
#if defined(__PS3__)
            m_bExitScene=true;
#else
            navigateBack();
#endif
            handled = true;
        }
        break;
    case ACTION_MENU_X:
#if TO_BE_IMPLEMENTED
        // Change device
        // Fix for #12531 - TCR 001: BAS Game Stability: When a player selects to change a storage 
        // device, and repeatedly backs out of the SD screen, disconnects from LIVE, and then selects a SD, the title crashes.
        m_bIgnoreInput=true;
        StorageManager.SetSaveDevice(&CScene_MultiGameJoinLoad::DeviceSelectReturned,this,true);
        ui.PlayUISFX(eSFX_Press);
#endif
        // Save Transfer
#ifdef SONY_REMOTE_STORAGE_DOWNLOAD
		{
			bool bSignedInLive = ProfileManager.IsSignedInLive(iPad);
			if(bSignedInLive)
			{			
				LaunchSaveTransfer();
			}
		}
#endif
        break;
    case ACTION_MENU_Y:
#if defined(__PS3__)
		m_eAction = eAction_ViewInvites;
        if(pressed && iPad == ProfileManager.GetPrimaryPad())
        {

            // are we offline?
            if(!ProfileManager.IsSignedInLive(iPad))
            {
                // get them to sign in to online
                UINT uiIDA[2];
                uiIDA[0]=IDS_PRO_NOTONLINE_ACCEPT;
                uiIDA[1]=IDS_PRO_NOTONLINE_DECLINE;
                ui.RequestMessageBox(IDS_PRO_NOTONLINE_TITLE, IDS_PRO_NOTONLINE_TEXT, uiIDA, 2, ProfileManager.GetPrimaryPad(), &UIScene_LoadOrJoinMenu::MustSignInReturnedPSN, this, app.GetStringTable(),NULL,0,false);
            }
            else
            {
                int ret = sceNpBasicRecvMessageCustom(SCE_NP_BASIC_MESSAGE_MAIN_TYPE_INVITE, SCE_NP_BASIC_RECV_MESSAGE_OPTIONS_INCLUDE_BOOTABLE, SYS_MEMORY_CONTAINER_ID_INVALID);
                app.DebugPrintf("sceNpBasicRecvMessageCustom return %d ( %08x )\n", ret, ret);
            }
        }
#endif
        break;

    case ACTION_MENU_RIGHT_SCROLL:
        if(DoesSavesListHaveFocus())
        {
            // 4J-PB - check we are on a valid save
            if((m_iDefaultButtonsC != 0) && (m_iSaveListIndex >= m_iDefaultButtonsC))
            {
                m_bIgnoreInput = true;

                // Could be delete save or Save Options
                if(StorageManager.GetSaveDisabled())
                {
                    // delete the save game
                    // Have to ask the player if they are sure they want to delete this game
                    UINT uiIDA[2];
                    uiIDA[0]=IDS_CONFIRM_CANCEL;
                    uiIDA[1]=IDS_CONFIRM_OK;
                    ui.RequestMessageBox(IDS_TOOLTIPS_DELETESAVE, IDS_TEXT_DELETE_SAVE, uiIDA, 2, iPad,&UIScene_LoadOrJoinMenu::DeleteSaveDialogReturned,this, app.GetStringTable(),NULL,0,false);
                }
                else
                {
                    if(StorageManager.EnoughSpaceForAMinSaveGame())
                    {
                        UINT uiIDA[4];
                        uiIDA[0]=IDS_CONFIRM_CANCEL;
                        uiIDA[1]=IDS_TITLE_RENAMESAVE;
                        uiIDA[2]=IDS_TOOLTIPS_DELETESAVE;
                        int numOptions = 3;
#ifdef SONY_REMOTE_STORAGE_UPLOAD
                        if(ProfileManager.IsSignedInLive(ProfileManager.GetPrimaryPad()))
                        {
                            numOptions = 4;
                            uiIDA[3]=IDS_TOOLTIPS_SAVETRANSFER_UPLOAD;
                        }
#endif
                        ui.RequestMessageBox(IDS_TOOLTIPS_SAVEOPTIONS, IDS_TEXT_SAVEOPTIONS, uiIDA, numOptions, iPad,&UIScene_LoadOrJoinMenu::SaveOptionsDialogReturned,this, app.GetStringTable(),NULL,0,false);
                    }
                    else
                    {
                        // delete the save game
                        // Have to ask the player if they are sure they want to delete this game
                        UINT uiIDA[2];
                        uiIDA[0]=IDS_CONFIRM_CANCEL;
                        uiIDA[1]=IDS_CONFIRM_OK;
                        ui.RequestMessageBox(IDS_TOOLTIPS_DELETESAVE, IDS_TEXT_DELETE_SAVE, uiIDA, 2,iPad,&UIScene_LoadOrJoinMenu::DeleteSaveDialogReturned,this, app.GetStringTable(),NULL,0,false);
                    }
                }
                ui.PlayUISFX(eSFX_Press);
            }
        }
		else if(DoesMashUpWorldHaveFocus())
		{
			// hiding a mash-up world
			if((m_iSaveListIndex != JOIN_LOAD_CREATE_BUTTON_INDEX))
			{
				LevelGenerationOptions *levelGen = m_generators.at(m_iSaveListIndex - 1);

				if(!levelGen->isTutorial())
				{
					if(levelGen->requiresTexturePack())
					{
						unsigned int uiPackID=levelGen->getRequiredTexturePackId();

						m_bIgnoreInput = true;
						app.HideMashupPackWorld(m_iPad,uiPackID);

						// update the saves list
						m_iState = e_SavesRepopulateAfterMashupHide;
					}
				}
			}
			ui.PlayUISFX(eSFX_Press);

		}
        break;
    case ACTION_MENU_LEFT_SCROLL:
        break;
    case ACTION_MENU_LEFT:
    case ACTION_MENU_RIGHT:
        {
            // if we are on the saves menu, check there are games in the games list to move to
            if(DoesSavesListHaveFocus())
            {
                if( m_buttonListGames.getItemCount() > 0)
                {
                    sendInputToMovie(key, repeat, pressed, released);
                }
            }
            else
            {
                sendInputToMovie(key, repeat, pressed, released);
            }
        }
        break;

    case ACTION_MENU_OK:
    case ACTION_MENU_UP:
    case ACTION_MENU_DOWN:
    case ACTION_MENU_PAGEUP:
    case ACTION_MENU_PAGEDOWN:
        sendInputToMovie(key, repeat, pressed, released);
        handled = true;
        break;
    }
}

int UIScene_LoadOrJoinMenu::KeyboardCompleteWorldNameCallback(LPVOID lpParam,bool bRes)
{
    // 4J HEG - No reason to set value if keyboard was cancelled
    UIScene_LoadOrJoinMenu *pClass=(UIScene_LoadOrJoinMenu *)lpParam;
	pClass->m_bIgnoreInput=false;
    if (bRes)
    {	
        uint16_t ui16Text[128];
        ZeroMemory(ui16Text, 128 * sizeof(uint16_t) );
        InputManager.GetText(ui16Text);

        // check the name is valid
        if(ui16Text[0]!=0)
        {
#if defined __PS3__
            // open the save and overwrite the metadata
            StorageManager.RenameSaveData(pClass->m_iSaveListIndex - pClass->m_iDefaultButtonsC, ui16Text,&UIScene_LoadOrJoinMenu::RenameSaveDataReturned,pClass);
#endif
        }
        else 
        {
            pClass->m_bIgnoreInput=false;
            pClass->updateTooltips();
        }
    } 
    else 
    {
        pClass->m_bIgnoreInput=false;
        pClass->updateTooltips();
    }


    return 0;
}
void UIScene_LoadOrJoinMenu::handleInitFocus(F64 controlId, F64 childId)
{
    app.DebugPrintf(app.USER_SR, "UIScene_LoadOrJoinMenu::handleInitFocus - %d , %d\n", (int)controlId, (int)childId);
}

void UIScene_LoadOrJoinMenu::handleFocusChange(F64 controlId, F64 childId) 
{
    app.DebugPrintf(app.USER_SR, "UIScene_LoadOrJoinMenu::handleFocusChange - %d , %d\n", (int)controlId, (int)childId);
    
	switch((int)controlId)
	{
	case eControl_GamesList:	
		m_iGameListIndex = childId;
		m_buttonListGames.updateChildFocus( (int) childId );
		break;
	case eControl_SavesList:
		m_iSaveListIndex = childId;
        m_bUpdateSaveSize = true;
		break;
	};
    updateTooltips();
}


#ifdef SONY_REMOTE_STORAGE_DOWNLOAD
void UIScene_LoadOrJoinMenu::remoteStorageGetSaveCallback(LPVOID lpParam, SonyRemoteStorage::Status s, int error_code)
{
    app.DebugPrintf("remoteStorageGetCallback err : 0x%08x\n", error_code);
    assert(error_code == 0);
    ((UIScene_LoadOrJoinMenu*)lpParam)->LoadSaveFromCloud();
}
#endif

void UIScene_LoadOrJoinMenu::handlePress(F64 controlId, F64 childId)
{
    switch((int)controlId)
    {
    case eControl_SavesList:
        {
            m_bIgnoreInput=true;

            int lGenID = (int)childId - 1;

            //CD - Added for audio
            ui.PlayUISFX(eSFX_Press);

            if((int)childId == JOIN_LOAD_CREATE_BUTTON_INDEX)
            {		
                app.SetTutorialMode( false );

                m_controlJoinTimer.setVisible( false );

                app.SetCorruptSaveDeleted(false);

                CreateWorldMenuInitData *params = new CreateWorldMenuInitData();
                params->iPad = m_iPad;
                ui.NavigateToScene(m_iPad,eUIScene_CreateWorldMenu,(void *)params);
            }
            else if (lGenID < m_generators.size())
            {
                LevelGenerationOptions *levelGen = m_generators.at(lGenID);
                app.SetTutorialMode( levelGen->isTutorial() );
                // Reset the autosave time
                app.SetAutosaveTimerTime();

                if(levelGen->isTutorial())
                {
                    LoadLevelGen(levelGen);
                }
                else
                {
                    LoadMenuInitData *params = new LoadMenuInitData();
                    params->iPad = m_iPad;
                    // need to get the iIndex from the list item, since the position in the list doesn't correspond to the GetSaveGameInfo list because of sorting
                    params->iSaveGameInfoIndex=-1;
                    //params->pbSaveRenamed=&m_bSaveRenamed;
                    params->levelGen = levelGen;
                    params->saveDetails = NULL;

                    // navigate to the settings scene
                    ui.NavigateToScene(ProfileManager.GetPrimaryPad(),eUIScene_LoadMenu, params);
                }
            }
            else
            {
                {		
                    app.SetTutorialMode( false );

                    if(app.DebugSettingsOn() && app.GetLoadSavesFromFolderEnabled())
                    {
                        LoadSaveFromDisk(m_saves->at((int)childId-m_iDefaultButtonsC));
                    }
                    else
                    {
                        LoadMenuInitData *params = new LoadMenuInitData();
                        params->iPad = m_iPad;
                        // need to get the iIndex from the list item, since the position in the list doesn't correspond to the GetSaveGameInfo list because of sorting
                        params->iSaveGameInfoIndex=((int)childId)-m_iDefaultButtonsC;
                        //params->pbSaveRenamed=&m_bSaveRenamed;
                        params->levelGen = NULL;
                        params->saveDetails = &m_saveDetails[ ((int)childId)-m_iDefaultButtonsC ];

                        {
                            // navigate to the settings scene
                            ui.NavigateToScene(ProfileManager.GetPrimaryPad(),eUIScene_LoadMenu, params);
                        }
                    }
                }
            }
        }
        break;
    case eControl_GamesList:
        {
            m_bIgnoreInput=true;
			
			m_eAction = eAction_JoinGame;

            //CD - Added for audio
            ui.PlayUISFX(eSFX_Press);

			{
				int nIndex = (int)childId;
				m_iGameListIndex = nIndex;
				CheckAndJoinGame(nIndex);
			}

            break;
        }
    }
}

void UIScene_LoadOrJoinMenu::CheckAndJoinGame(int gameIndex)
{
	if( m_buttonListGames.getItemCount() > 0 && gameIndex < m_currentSessions->size() )
	{
#if defined(__PS3__)
		// 4J-PB - is the player allowed to join games?
		bool noUGC=false;
		bool bContentRestricted=false;

		// we're online, since we are joining a game
		ProfileManager.GetChatAndContentRestrictions(m_iPad,true,&noUGC,&bContentRestricted,NULL);


		if(noUGC)
		{
			// not allowed to join
			UINT uiIDA[1];
			uiIDA[0]=IDS_CONFIRM_OK;
			// Not allowed to play online
			ui.RequestMessageBox(IDS_ONLINE_GAME, IDS_CHAT_RESTRICTION_UGC, uiIDA, 1, m_iPad,NULL,this,app.GetStringTable(),NULL,0,false);

			m_bIgnoreInput=false;
			return;
		}
		else if(bContentRestricted)
		{
			ui.RequestContentRestrictedMessageBox();

			m_bIgnoreInput=false;
			return;
		}
#endif

		//CScene_MultiGameInfo::JoinMenuInitData *initData = new CScene_MultiGameInfo::JoinMenuInitData();
		m_initData->iPad = 0;;
		m_initData->selectedSession = m_currentSessions->at( gameIndex );

		// check that we have the texture pack available
		// If it's not the default texture pack
		if(m_initData->selectedSession->data.texturePackParentId!=0)
		{
			int texturePacksCount = Minecraft::GetInstance()->skins->getTexturePackCount();
			bool bHasTexturePackInstalled=false;

			for(int i=0;i<texturePacksCount;i++)
			{
				TexturePack *tp = Minecraft::GetInstance()->skins->getTexturePackByIndex(i);
				if(tp->getDLCParentPackId()==m_initData->selectedSession->data.texturePackParentId)
				{
					bHasTexturePackInstalled=true;
					break;
				}
			}

			if(bHasTexturePackInstalled==false)
			{
				// upsell the texture pack
				// tell sentient about the upsell of the full version of the skin pack
				UINT uiIDA[2];

				uiIDA[0]=IDS_TEXTUREPACK_FULLVERSION;
				//uiIDA[1]=IDS_TEXTURE_PACK_TRIALVERSION;
				uiIDA[1]=IDS_CONFIRM_CANCEL;


				// Give the player a warning about the texture pack missing
				ui.RequestMessageBox(IDS_DLC_TEXTUREPACK_NOT_PRESENT_TITLE, IDS_DLC_TEXTUREPACK_NOT_PRESENT, uiIDA, 2, m_iPad,&UIScene_LoadOrJoinMenu::TexturePackDialogReturned,this,app.GetStringTable(),NULL,0,false);

				return;
			}
		}
		m_controlJoinTimer.setVisible( false );


		m_bIgnoreInput=true;
		ui.NavigateToScene(ProfileManager.GetPrimaryPad(),eUIScene_JoinMenu,m_initData);
	}
}

void UIScene_LoadOrJoinMenu::LoadLevelGen(LevelGenerationOptions *levelGen)
{	
    // Load data from disc
    //File saveFile( L"Tutorial\\Tutorial" );
    //LoadSaveFromDisk(&saveFile);

    // clear out the app's terrain features list
    app.ClearTerrainFeaturePosition();

    StorageManager.ResetSaveData();
    // Make our next save default to the name of the level
    StorageManager.SetSaveTitle(levelGen->getDefaultSaveName().c_str());

    bool isClientSide = false;
    bool isPrivate = false;
    // TODO int maxPlayers = MINECRAFT_NET_MAX_PLAYERS;
    int maxPlayers = 8;

    if( app.GetTutorialMode() )
    {
        isClientSide = false;
        maxPlayers = 4;
    }

    g_NetworkManager.HostGame(0,isClientSide,isPrivate,maxPlayers,0);

    NetworkGameInitData *param = new NetworkGameInitData();
    param->seed = 0;
    param->saveData = NULL;
    param->settings = app.GetGameHostOption( eGameHostOption_Tutorial );
    param->levelGen = levelGen;

    if(levelGen->requiresTexturePack())
    {
        param->texturePackId = levelGen->getRequiredTexturePackId();

        Minecraft *pMinecraft = Minecraft::GetInstance();
        pMinecraft->skins->selectTexturePackById(param->texturePackId);
        //pMinecraft->skins->updateUI();
    }

    g_NetworkManager.FakeLocalPlayerJoined();

    LoadingInputParams *loadingParams = new LoadingInputParams();
    loadingParams->func = &CGameNetworkManager::RunNetworkGameThreadProc;
    loadingParams->lpParam = (LPVOID)param;

    UIFullscreenProgressCompletionData *completionData = new UIFullscreenProgressCompletionData();
    completionData->bShowBackground=TRUE;
    completionData->bShowLogo=TRUE;
    completionData->type = e_ProgressCompletion_CloseAllPlayersUIScenes;
    completionData->iPad = DEFAULT_XUI_MENU_USER;
    loadingParams->completionData = completionData;

    ui.NavigateToScene(ProfileManager.GetPrimaryPad(),eUIScene_FullscreenProgress, loadingParams);
}

void UIScene_LoadOrJoinMenu::UpdateGamesListCallback(LPVOID pParam)
{
    if(pParam != NULL)
    {
        UIScene_LoadOrJoinMenu *pScene = (UIScene_LoadOrJoinMenu *)pParam;
        pScene->UpdateGamesList();
    }
}

void UIScene_LoadOrJoinMenu::UpdateGamesList()
{
    // If we're ignoring input scene isn't active so do nothing
    if (m_bIgnoreInput) return;

    // If a texture pack is loading, or will be loading, then ignore this ( we are going to be destroyed anyway)
    if( Minecraft::GetInstance()->skins->getSelected()->isLoadingData() || (Minecraft::GetInstance()->skins->needsUIUpdate() || ui.IsReloadingSkin()) ) return;

    // if we're retrieving save info, don't show the list yet as we will be ignoring press events
    if(!m_bSavesDisplayed)
    {
        return;
    }


    FriendSessionInfo *pSelectedSession = NULL;
    if(DoesGamesListHaveFocus() && m_buttonListGames.getItemCount() > 0)
    {
        unsigned int nIndex = m_buttonListGames.getCurrentSelection();
        pSelectedSession = m_currentSessions->at( nIndex );
    }

    SessionID selectedSessionId;
	ZeroMemory(&selectedSessionId,sizeof(SessionID));
    if( pSelectedSession != NULL )selectedSessionId = pSelectedSession->sessionId;
    pSelectedSession = NULL;

    m_controlJoinTimer.setVisible( false );

    // if the saves list has focus, then we should show the Delete Save tooltip
    // if the games list has focus, then we should show the View Gamercard tooltip
    int iRB=-1;	
    int iY = -1;
    int iX=-1;

    delete m_currentSessions;
    m_currentSessions = g_NetworkManager.GetSessionList( m_iPad, 1, m_bShowingPartyGamesOnly );

    // Update the xui list displayed
    unsigned int xuiListSize = m_buttonListGames.getItemCount();
    unsigned int filteredListSize = (unsigned int)m_currentSessions->size();

    BOOL gamesListHasFocus = DoesGamesListHaveFocus();

    if(filteredListSize > 0)
    {
#if TO_BE_IMPLEMENTED
        if( !m_pGamesList->IsEnabled() )
        {
            m_pGamesList->SetEnable(TRUE);
            m_pGamesList->SetCurSel( 0 );
        }
#endif
        m_labelNoGames.setVisible( false );
        m_controlJoinTimer.setVisible( false );
    }
    else
    {
#if TO_BE_IMPLEMENTED
        m_pGamesList->SetEnable(FALSE);
#endif
        m_controlJoinTimer.setVisible( false );
        m_labelNoGames.setVisible( true );

#if TO_BE_IMPLEMENTED
        if( gamesListHasFocus ) m_pGamesList->InitFocus(m_iPad);
#endif
    }

    // clear out the games list and re-fill
    m_buttonListGames.clearList();

    if( filteredListSize > 0 )
    {
        // Reset the focus to the selected session if it still exists
        unsigned int sessionIndex = 0;
        m_buttonListGames.setCurrentSelection(0);

        for( AUTO_VAR(it, m_currentSessions->begin()); it < m_currentSessions->end(); ++it)
        {
            FriendSessionInfo *sessionInfo = *it;

            wchar_t textureName[64] = L"\0";

            // Is this a default game or a texture pack game?
            if(sessionInfo->data.texturePackParentId!=0)
            {
                // Do we have the texture pack
                Minecraft *pMinecraft = Minecraft::GetInstance();
                TexturePack *tp = pMinecraft->skins->getTexturePackById(sessionInfo->data.texturePackParentId);
                HRESULT hr;

                DWORD dwImageBytes=0;
                PBYTE pbImageData=NULL;

                if(tp==NULL)
                {
                    DWORD dwBytes=0;
                    PBYTE pbData=NULL;
                    app.GetTPD(sessionInfo->data.texturePackParentId,&pbData,&dwBytes);

                    // is it in the tpd data ?
                    app.GetFileFromTPD(eTPDFileType_Icon,pbData,dwBytes,&pbImageData,&dwImageBytes );
                    if(dwImageBytes > 0 && pbImageData)
                    {
                        swprintf(textureName,64,L"%ls",sessionInfo->displayLabel);
                        registerSubstitutionTexture(textureName,pbImageData,dwImageBytes);
                    }
                }
                else
                {
                    pbImageData = tp->getPackIcon(dwImageBytes);
                    if(dwImageBytes > 0 && pbImageData)
                    {
                        swprintf(textureName,64,L"%ls",sessionInfo->displayLabel);
                        registerSubstitutionTexture(textureName,pbImageData,dwImageBytes);
                    }
                }
            }
            else
            {
                // default texture pack
                Minecraft *pMinecraft = Minecraft::GetInstance();
                TexturePack *tp = pMinecraft->skins->getTexturePackByIndex(0);

                DWORD dwImageBytes;
                PBYTE pbImageData = tp->getPackIcon(dwImageBytes);

                if(dwImageBytes > 0 && pbImageData)
                {
                    swprintf(textureName,64,L"%ls",sessionInfo->displayLabel);
                    registerSubstitutionTexture(textureName,pbImageData,dwImageBytes);
                }
            }

            m_buttonListGames.addItem( sessionInfo->displayLabel, textureName );

            if(memcmp( &selectedSessionId, &sessionInfo->sessionId, sizeof(SessionID) ) == 0)
            {
                m_buttonListGames.setCurrentSelection(sessionIndex);
                break;
            }
            ++sessionIndex;
        }
    }

	updateTooltips();
}

void UIScene_LoadOrJoinMenu::HandleDLCMountingComplete()
{
    Initialise();
}

bool UIScene_LoadOrJoinMenu::DoesSavesListHaveFocus()
{
	if( m_buttonListSaves.hasFocus() )
	{
		// check it's not the first or second element (new world or tutorial)
		if(m_iSaveListIndex > (m_iDefaultButtonsC-1))
		{
			return true;
		}
	}
	return false;
}

bool UIScene_LoadOrJoinMenu::DoesMashUpWorldHaveFocus()
{
	if(m_buttonListSaves.hasFocus())
	{
		// check it's not the first or second element (new world or tutorial)
		if(m_iSaveListIndex > (m_iDefaultButtonsC - 1))
		{
			return false;
		}

		if(m_iSaveListIndex > (m_iDefaultButtonsC - 1 - m_iMashUpButtonsC))
		{
			return true;
		}
		else return false;
	}
	else return false;
}

bool UIScene_LoadOrJoinMenu::DoesGamesListHaveFocus()
{
    return m_buttonListGames.hasFocus();
}

void UIScene_LoadOrJoinMenu::handleTimerComplete(int id)
{
    switch(id)
    {
    case JOIN_LOAD_ONLINE_TIMER_ID:
        {

            bool bMultiplayerAllowed = ProfileManager.IsSignedInLive( m_iPad ) && ProfileManager.AllowedToPlayMultiplayer(m_iPad);
            if(bMultiplayerAllowed != m_bMultiplayerAllowed)
            {
                if( bMultiplayerAllowed )
                {
                    // 					m_CheckboxOnline.SetEnable(TRUE);
                    // 					m_CheckboxPrivate.SetEnable(TRUE);
                }
                else
                {
                    m_bInParty = false;
                    m_buttonListGames.clearList();
                    m_controlJoinTimer.setVisible( true );
                    m_labelNoGames.setVisible( false );
                }

                m_bMultiplayerAllowed = bMultiplayerAllowed;
            }
        }
        break;
        // 4J-PB - Only Xbox will not have trial DLC patched into the game
    }

}

void UIScene_LoadOrJoinMenu::LoadSaveFromDisk(File *saveFile, ESavePlatform savePlatform /*= SAVE_FILE_PLATFORM_LOCAL*/)
{	
    // we'll only be coming in here when the tutorial is loaded now

    StorageManager.ResetSaveData();

    // Make our next save default to the name of the level
    StorageManager.SetSaveTitle(saveFile->getName().c_str());

    __int64 fileSize = saveFile->length();
    FileInputStream fis(*saveFile);
    byteArray ba(fileSize);
    fis.read(ba);
    fis.close();



    bool isClientSide = false;
    bool isPrivate = false;
    int maxPlayers = MINECRAFT_NET_MAX_PLAYERS;

    if( app.GetTutorialMode() )
    {
        isClientSide = false;
        maxPlayers = 4;
    }

    app.SetGameHostOption(eGameHostOption_GameType,GameType::CREATIVE->getId() );

    g_NetworkManager.HostGame(0,isClientSide,isPrivate,maxPlayers,0);

    LoadSaveDataThreadParam *saveData = new LoadSaveDataThreadParam(ba.data, ba.length, saveFile->getName());

    NetworkGameInitData *param = new NetworkGameInitData();
    param->seed = 0;
    param->saveData = saveData;
    param->settings = app.GetGameHostOption( eGameHostOption_All );
    param->savePlatform = savePlatform;

    g_NetworkManager.FakeLocalPlayerJoined();

    LoadingInputParams *loadingParams = new LoadingInputParams();
    loadingParams->func = &CGameNetworkManager::RunNetworkGameThreadProc;
    loadingParams->lpParam = (LPVOID)param;

    UIFullscreenProgressCompletionData *completionData = new UIFullscreenProgressCompletionData();
    completionData->bShowBackground=TRUE;
    completionData->bShowLogo=TRUE;
    completionData->type = e_ProgressCompletion_CloseAllPlayersUIScenes;
    completionData->iPad = DEFAULT_XUI_MENU_USER;
    loadingParams->completionData = completionData;

    ui.NavigateToScene(ProfileManager.GetPrimaryPad(),eUIScene_FullscreenProgress, loadingParams);
}

#ifdef SONY_REMOTE_STORAGE_DOWNLOAD
void UIScene_LoadOrJoinMenu::LoadSaveFromCloud()
{	

    wchar_t wFileName[128];
    mbstowcs(wFileName, app.getRemoteStorage()->getLocalFilename(), strlen(app.getRemoteStorage()->getLocalFilename())+1); // plus null
    File cloudFile(wFileName);


    StorageManager.ResetSaveData();

    // Make our next save default to the name of the level
    wchar_t wSaveName[128];
    mbstowcs(wSaveName, app.getRemoteStorage()->getSaveNameUTF8(), strlen(app.getRemoteStorage()->getSaveNameUTF8())+1); // plus null
    StorageManager.SetSaveTitle(wSaveName);

    __int64 fileSize = cloudFile.length();
    FileInputStream fis(cloudFile);
    byteArray ba(fileSize);
    fis.read(ba);
    fis.close();



    bool isClientSide = false;
    bool isPrivate = false;
    int maxPlayers = MINECRAFT_NET_MAX_PLAYERS;

    if( app.GetTutorialMode() )
    {
        isClientSide = false;
        maxPlayers = 4;
    }

	app.SetGameHostOption(eGameHostOption_All, app.getRemoteStorage()->getSaveHostOptions() );

    g_NetworkManager.HostGame(0,isClientSide,isPrivate,maxPlayers,0);

    LoadSaveDataThreadParam *saveData = new LoadSaveDataThreadParam(ba.data, ba.length, cloudFile.getName());

    NetworkGameInitData *param = new NetworkGameInitData();
    param->seed = app.getRemoteStorage()->getSaveSeed();
    param->saveData = saveData;
    param->settings = app.GetGameHostOption( eGameHostOption_All );
    param->savePlatform = app.getRemoteStorage()->getSavePlatform();
	param->texturePackId = app.getRemoteStorage()->getSaveTexturePack();

    g_NetworkManager.FakeLocalPlayerJoined();

    LoadingInputParams *loadingParams = new LoadingInputParams();
    loadingParams->func = &CGameNetworkManager::RunNetworkGameThreadProc;
    loadingParams->lpParam = (LPVOID)param;

    UIFullscreenProgressCompletionData *completionData = new UIFullscreenProgressCompletionData();
    completionData->bShowBackground=TRUE;
    completionData->bShowLogo=TRUE;
    completionData->type = e_ProgressCompletion_CloseAllPlayersUIScenes;
    completionData->iPad = DEFAULT_XUI_MENU_USER;
    loadingParams->completionData = completionData;

    ui.NavigateToScene(ProfileManager.GetPrimaryPad(),eUIScene_FullscreenProgress, loadingParams);
}

#endif //SONY_REMOTE_STORAGE_DOWNLOAD

int UIScene_LoadOrJoinMenu::DeleteSaveDialogReturned(void *pParam,int iPad,C4JStorage::EMessageResult result)
{
    UIScene_LoadOrJoinMenu* pClass = (UIScene_LoadOrJoinMenu*)pParam;
    // results switched for this dialog

	// Check that we have a valid save selected (can get a bad index if the save list has been refreshed)
	bool validSelection= pClass->m_iDefaultButtonsC != 0 && pClass->m_iSaveListIndex >= pClass->m_iDefaultButtonsC;

    if(result==C4JStorage::EMessage_ResultDecline && validSelection) 
    {
        if(app.DebugSettingsOn() && app.GetLoadSavesFromFolderEnabled())
        {
            pClass->m_bIgnoreInput=false;
        }
        else
        {
			StorageManager.DeleteSaveData(&pClass->m_pSaveDetails->SaveInfoA[pClass->m_iSaveListIndex - pClass->m_iDefaultButtonsC], UIScene_LoadOrJoinMenu::DeleteSaveDataReturned, (LPVOID)pClass->GetCallbackUniqueId());
            pClass->m_controlSavesTimer.setVisible( true );
        }
    }
    else
    {
        pClass->m_bIgnoreInput=false;
    }

    return 0;
}

int UIScene_LoadOrJoinMenu::DeleteSaveDataReturned(LPVOID lpParam,bool bRes)
{
	ui.EnterCallbackIdCriticalSection();
    UIScene_LoadOrJoinMenu* pClass = (UIScene_LoadOrJoinMenu*)ui.GetSceneFromCallbackId((size_t)lpParam);

	if(pClass)
	{
		if(bRes)
		{
			// wipe the list and repopulate it
			pClass->m_iState=e_SavesRepopulateAfterDelete;		
		}
		else pClass->m_bIgnoreInput=false;

		pClass->updateTooltips();
	}
	ui.LeaveCallbackIdCriticalSection();
    return 0;
}


int UIScene_LoadOrJoinMenu::RenameSaveDataReturned(LPVOID lpParam,bool bRes)
{
    UIScene_LoadOrJoinMenu* pClass = (UIScene_LoadOrJoinMenu*)lpParam;

    if(bRes)
    {
        pClass->m_iState=e_SavesRepopulate;
    }
    else pClass->m_bIgnoreInput=false;

    pClass->updateTooltips();

    return 0;
}



int UIScene_LoadOrJoinMenu::SaveOptionsDialogReturned(void *pParam,int iPad,C4JStorage::EMessageResult result)
{
    UIScene_LoadOrJoinMenu* pClass = (UIScene_LoadOrJoinMenu*)pParam;

    // results switched for this dialog
    // EMessage_ResultAccept means cancel
    switch(result)
    {
    case C4JStorage::EMessage_ResultDecline:  // rename
        {
			pClass->m_bIgnoreInput=true;
            // bring up a keyboard
            wchar_t wSaveName[128];
            //CD - Fix - We must memset the SaveName
            ZeroMemory(wSaveName, 128 * sizeof(wchar_t) );
            mbstowcs(wSaveName, pClass->m_saveDetails[pClass->m_iSaveListIndex - pClass->m_iDefaultButtonsC].UTF8SaveName, strlen(pClass->m_saveDetails->UTF8SaveName)+1); // plus null
            LPWSTR ptr = wSaveName;
            InputManager.RequestKeyboard(app.GetString(IDS_RENAME_WORLD_TITLE),wSaveName,(DWORD)0,25,&UIScene_LoadOrJoinMenu::KeyboardCompleteWorldNameCallback,pClass,C_4JInput::EKeyboardMode_Default);
        }
        break;

    case C4JStorage::EMessage_ResultThirdOption:  // delete -
        {
            // delete the save game
            // Have to ask the player if they are sure they want to delete this game
            UINT uiIDA[2];
            uiIDA[0]=IDS_CONFIRM_CANCEL;
            uiIDA[1]=IDS_CONFIRM_OK;
            ui.RequestMessageBox(IDS_TOOLTIPS_DELETESAVE, IDS_TEXT_DELETE_SAVE, uiIDA, 2, iPad,&UIScene_LoadOrJoinMenu::DeleteSaveDialogReturned,pClass, app.GetStringTable(),NULL,0,false);
        }
        break;

#ifdef SONY_REMOTE_STORAGE_UPLOAD
    case C4JStorage::EMessage_ResultFourthOption: // upload to cloud
        {
			UINT uiIDA[2];
			uiIDA[0]=IDS_CONFIRM_OK;
			uiIDA[1]=IDS_CONFIRM_CANCEL;

			ui.RequestMessageBox(IDS_TOOLTIPS_SAVETRANSFER_UPLOAD, IDS_SAVE_TRANSFER_TEXT, uiIDA, 2, iPad,&UIScene_LoadOrJoinMenu::SaveTransferDialogReturned,pClass, app.GetStringTable(),NULL,0,false);
        }
        break;
#endif // SONY_REMOTE_STORAGE_UPLOAD

    case C4JStorage::EMessage_Cancelled:
    default:
        {
            // reset the tooltips
            pClass->updateTooltips();
            pClass->m_bIgnoreInput=false;
        }
        break;
    }
    return 0;
}

int UIScene_LoadOrJoinMenu::TexturePackDialogReturned(void *pParam,int iPad,C4JStorage::EMessageResult result)
{
    UIScene_LoadOrJoinMenu *pClass = (UIScene_LoadOrJoinMenu *)pParam;

    // Exit with or without saving
    if(result==C4JStorage::EMessage_ResultAccept) 
    {
        // we need to enable background downloading for the DLC
        XBackgroundDownloadSetMode(XBACKGROUND_DOWNLOAD_MODE_ALWAYS_ALLOW);
#if TO_BE_IMPLEMENTED
        ULONGLONG ullOfferID_Full;
        ULONGLONG ullIndexA[1];
        app.GetDLCFullOfferIDForPackID(pClass->m_initData->selectedSession->data.texturePackParentId,&ullOfferID_Full);


        if( result==C4JStorage::EMessage_ResultAccept ) // Full version
        {
            ullIndexA[0]=ullOfferID_Full;
            StorageManager.InstallOffer(1,ullIndexA,NULL,NULL);

        }
        else // trial version
        {
            DLC_INFO *pDLCInfo=app.GetDLCInfoForFullOfferID(ullOfferID_Full);
            ullIndexA[0]=pDLCInfo->ullOfferID_Trial;
            StorageManager.InstallOffer(1,ullIndexA,NULL,NULL);
        }
#endif



    }
    pClass->m_bIgnoreInput=false;
    return 0;
}

#if defined __PS3__
int UIScene_LoadOrJoinMenu::MustSignInReturnedPSN(void *pParam,int iPad,C4JStorage::EMessageResult result)
{
    UIScene_LoadOrJoinMenu* pClass = (UIScene_LoadOrJoinMenu*)pParam;

	if(result==C4JStorage::EMessage_ResultAccept) 
	{
#if defined(__PS3__)
		SQRNetworkManager_PS3::AttemptPSNSignIn(&UIScene_LoadOrJoinMenu::PSN_SignInReturned, pClass);
#else
		SQRNetworkManager_Orbis::AttemptPSNSignIn(&UIScene_LoadOrJoinMenu::PSN_SignInReturned, pClass, false, iPad);
#endif
	}
	else
	{
		pClass->m_bIgnoreInput = false;
	}

    return 0;
}

int UIScene_LoadOrJoinMenu::PSN_SignInReturned(void *pParam,bool bContinue, int iPad)
{
    UIScene_LoadOrJoinMenu* pClass = (UIScene_LoadOrJoinMenu*)pParam;
	if(bContinue==true)
	{
		switch(pClass->m_eAction)
		{
		case eAction_ViewInvites:
			// Check if we're signed in to LIVE
			if(ProfileManager.IsSignedInLive(iPad))
			{
#if defined(__PS3__)
				int ret = sceNpBasicRecvMessageCustom(SCE_NP_BASIC_MESSAGE_MAIN_TYPE_INVITE, SCE_NP_BASIC_RECV_MESSAGE_OPTIONS_INCLUDE_BOOTABLE, SYS_MEMORY_CONTAINER_ID_INVALID);
				app.DebugPrintf("sceNpBasicRecvMessageCustom return %d ( %08x )\n", ret, ret);
#else
				SQRNetworkManager_Orbis::RecvInviteGUI();
#endif
			}
			break;
		case eAction_JoinGame:
			pClass->CheckAndJoinGame(pClass->m_iGameListIndex);
			break;
		}
	}
	else
	{
		pClass->m_bIgnoreInput = false;
	}
    return 0;
}
#endif

#ifdef SONY_REMOTE_STORAGE_DOWNLOAD

void UIScene_LoadOrJoinMenu::LaunchSaveTransfer()
{
    LoadingInputParams *loadingParams = new LoadingInputParams();
    loadingParams->func = &UIScene_LoadOrJoinMenu::DownloadSonyCrossSaveThreadProc;
    loadingParams->lpParam = (LPVOID)this;

    UIFullscreenProgressCompletionData *completionData = new UIFullscreenProgressCompletionData();
    completionData->bShowBackground=TRUE;
    completionData->bShowLogo=TRUE;
    completionData->type = e_ProgressCompletion_NavigateBackToScene;
    completionData->iPad = DEFAULT_XUI_MENU_USER;
    loadingParams->completionData = completionData;

    loadingParams->cancelFunc=&UIScene_LoadOrJoinMenu::CancelSaveTransferCallback;
	loadingParams->m_cancelFuncParam=this;
    loadingParams->cancelText=IDS_TOOLTIPS_CANCEL;

    ui.NavigateToScene(m_iPad,eUIScene_FullscreenProgress, loadingParams);
}




int UIScene_LoadOrJoinMenu::CreateDummySaveDataCallback(LPVOID lpParam,bool bRes)
{
	UIScene_LoadOrJoinMenu* pClass = (UIScene_LoadOrJoinMenu *) lpParam;
	if(bRes)
	{
		pClass->m_eSaveTransferState = eSaveTransfer_GetSavesInfo;
	}
	else
	{
		pClass->m_eSaveTransferState = eSaveTransfer_Error;
		app.DebugPrintf("CreateDummySaveDataCallback failed\n");

	}
	return 0;
}

int UIScene_LoadOrJoinMenu::CrossSaveGetSavesInfoCallback(LPVOID lpParam, SAVE_DETAILS *pSaveDetails, bool bRes)
{
	UIScene_LoadOrJoinMenu* pClass = (UIScene_LoadOrJoinMenu *) lpParam;
	if(bRes)
	{
		pClass->m_eSaveTransferState = eSaveTransfer_GetFileData;
	}
	else
	{
		pClass->m_eSaveTransferState = eSaveTransfer_Error;
		app.DebugPrintf("CrossSaveGetSavesInfoCallback failed\n");
	}
	return 0;
}

int UIScene_LoadOrJoinMenu::LoadCrossSaveDataCallback( void *pParam,bool bIsCorrupt, bool bIsOwner )
{
	UIScene_LoadOrJoinMenu* pClass = (UIScene_LoadOrJoinMenu *) pParam;
	if(bIsCorrupt == false && bIsOwner)
	{
		pClass->m_eSaveTransferState = eSaveTransfer_CreatingNewSave;
	}
	else
	{
		pClass->m_eSaveTransferState = eSaveTransfer_Error;
		app.DebugPrintf("LoadCrossSaveDataCallback failed \n");

	}
	return 0;
}

int UIScene_LoadOrJoinMenu::CrossSaveFinishedCallback(void *pParam,int iPad,C4JStorage::EMessageResult result)
{
	UIScene_LoadOrJoinMenu* pClass = (UIScene_LoadOrJoinMenu *) pParam;
	pClass->m_eSaveTransferState = eSaveTransfer_Idle;
	return 0;
}


int UIScene_LoadOrJoinMenu::CrossSaveDeleteOnErrorReturned(LPVOID lpParam,bool bRes)
{
	UIScene_LoadOrJoinMenu* pClass = (UIScene_LoadOrJoinMenu *) lpParam;
	pClass->m_eSaveTransferState = eSaveTransfer_ErrorMesssage;
	return 0;
}

int UIScene_LoadOrJoinMenu::RemoteSaveNotFoundCallback(void *pParam,int iPad,C4JStorage::EMessageResult result)
{
	UIScene_LoadOrJoinMenu* pClass = (UIScene_LoadOrJoinMenu *) pParam;
	pClass->m_eSaveTransferState = eSaveTransfer_Idle;
	return 0;
}

// MGH -  added this global to force the delete of the previous data, for the remote storage saves
//	need to speak to Chris why this is necessary
bool g_bForceVitaSaveWipe = false;


int UIScene_LoadOrJoinMenu::DownloadSonyCrossSaveThreadProc( LPVOID lpParameter )
{
    Compression::UseDefaultThreadStorage();
    UIScene_LoadOrJoinMenu* pClass = (UIScene_LoadOrJoinMenu *) lpParameter;
 	pClass->m_saveTransferDownloadCancelled = false;
	bool bAbortCalled = false;
	Minecraft *pMinecraft=Minecraft::GetInstance();
	bool bSaveFileCreated = false;
	wchar_t wSaveName[128];

    // get the save file size
	pMinecraft->progressRenderer->progressStagePercentage(0);
    pMinecraft->progressRenderer->progressStart(IDS_TOOLTIPS_SAVETRANSFER_DOWNLOAD);
    pMinecraft->progressRenderer->progressStage( IDS_TOOLTIPS_SAVETRANSFER_DOWNLOAD );

	ConsoleSaveFile* pSave = NULL;

	pClass->m_eSaveTransferState = eSaveTransfer_GetRemoteSaveInfo;


    while(pClass->m_eSaveTransferState!=eSaveTransfer_Idle)
    {
        switch(pClass->m_eSaveTransferState)
        {
        case eSaveTransfer_Idle:
            break;
		case eSaveTransfer_GetRemoteSaveInfo:
			app.DebugPrintf("UIScene_LoadOrJoinMenu getSaveInfo\n");
			app.getRemoteStorage()->getSaveInfo();
			pClass->m_eSaveTransferState = eSaveTransfer_GettingRemoteSaveInfo;
		break;
		case eSaveTransfer_GettingRemoteSaveInfo:
			if(pClass->m_saveTransferDownloadCancelled)
			{
				pClass->m_eSaveTransferState = eSaveTransfer_Error;
				break;
			}
			if(app.getRemoteStorage()->waitingForSaveInfo() == false)
			{
				if(app.getRemoteStorage()->saveIsAvailable())
				{
					pClass->m_eSaveTransferState = eSaveTransfer_CreateDummyFile;
				}
				else
				{
					// no save available, inform the user about the functionality
					UINT uiIDA[1];
					uiIDA[0]=IDS_CONFIRM_OK;
					ui.RequestMessageBox(IDS_TOOLTIPS_SAVETRANSFER_DOWNLOAD, IDS_SAVE_TRANSFER_NOT_AVAILABLE_TEXT, uiIDA, 1, ProfileManager.GetPrimaryPad(),RemoteSaveNotFoundCallback,pClass, app.GetStringTable(),NULL,0,false);
				}
			}
			break;
		case eSaveTransfer_CreateDummyFile:
			{
				StorageManager.ResetSaveData();
				byte *compData = (byte *)StorageManager.AllocateSaveData( app.getRemoteStorage()->getSaveFilesize() );
				// Make our next save default to the name of the level
				const char* pNameUTF8 = app.getRemoteStorage()->getSaveNameUTF8();
				mbstowcs(wSaveName, pNameUTF8, strlen(pNameUTF8)+1); // plus null
				StorageManager.SetSaveTitle(wSaveName);
				PBYTE pbThumbnailData=NULL;
				DWORD dwThumbnailDataSize=0;

				PBYTE pbDataSaveImage=NULL;
				DWORD dwDataSizeSaveImage=0;

				StorageManager.GetDefaultSaveImage(&pbDataSaveImage, &dwDataSizeSaveImage);			// Get the default save thumbnail (as set by SetDefaultImages) for use on saving games t
				StorageManager.GetDefaultSaveThumbnail(&pbThumbnailData,&dwThumbnailDataSize);		// Get the default save image (as set by SetDefaultImages) for use on saving games that 

				BYTE bTextMetadata[88];
				ZeroMemory(bTextMetadata,88);
				int iTextMetadataBytes = app.CreateImageTextData(bTextMetadata, app.getRemoteStorage()->getSaveSeed(), true, app.getRemoteStorage()->getSaveHostOptions(), app.getRemoteStorage()->getSaveTexturePack() );

				// set the icon and save image
				StorageManager.SetSaveImages(pbThumbnailData,dwThumbnailDataSize,pbDataSaveImage,dwDataSizeSaveImage,bTextMetadata,iTextMetadataBytes);

				app.getRemoteStorage()->waitForStorageManagerIdle();
				C4JStorage::ESaveGameState saveState = StorageManager.SaveSaveData( &UIScene_LoadOrJoinMenu::CreateDummySaveDataCallback, lpParameter );
				if(saveState == C4JStorage::ESaveGame_Save)
				{
					pClass->m_eSaveTransferState = eSaveTransfer_CreatingDummyFile;
				}
				else
				{
					app.DebugPrintf("Failed to create dummy save file\n");
					pClass->m_eSaveTransferState = eSaveTransfer_Error;
				}
			}
			break;
		case eSaveTransfer_CreatingDummyFile:
			break;
		case eSaveTransfer_GetSavesInfo:
			{
				// we can't cancel here, we need the saves info so we can delete the file
				if(pClass->m_saveTransferDownloadCancelled)
				{	
					WCHAR wcTemp[256];
					swprintf(wcTemp,256, app.GetString(IDS_CANCEL));		// MGH - should change this string to "cancelling download"
					m_wstrStageText=wcTemp;
					pMinecraft->progressRenderer->progressStage( m_wstrStageText );
				}

				app.getRemoteStorage()->waitForStorageManagerIdle();
				app.DebugPrintf("CALL GetSavesInfo B\n");
				C4JStorage::ESaveGameState eSGIStatus= StorageManager.GetSavesInfo(pClass->m_iPad,&UIScene_LoadOrJoinMenu::CrossSaveGetSavesInfoCallback,pClass,"save");
				pClass->m_eSaveTransferState = eSaveTransfer_GettingSavesInfo;
			}
			break;
		case eSaveTransfer_GettingSavesInfo:
			if(pClass->m_saveTransferDownloadCancelled)
			{	
				WCHAR wcTemp[256];
				swprintf(wcTemp,256, app.GetString(IDS_CANCEL));		// MGH - should change this string to "cancelling download"
				m_wstrStageText=wcTemp;
				pMinecraft->progressRenderer->progressStage( m_wstrStageText );
			}
			break;

		case eSaveTransfer_GetFileData:
		{
			bSaveFileCreated = true;
			StorageManager.GetSaveUniqueFileDir(pClass->m_downloadedUniqueFilename);

			if(pClass->m_saveTransferDownloadCancelled)
			{
				pClass->m_eSaveTransferState = eSaveTransfer_Error;
				break;
			}
			PSAVE_DETAILS pSaveDetails=StorageManager.ReturnSavesInfo();
			int idx = pClass->m_iSaveListIndex - pClass->m_iDefaultButtonsC;
			app.getRemoteStorage()->waitForStorageManagerIdle();
			bool bGettingOK = app.getRemoteStorage()->getSaveData(pClass->m_downloadedUniqueFilename, SaveTransferReturned, pClass);
			if(bGettingOK)
			{
				pClass->m_eSaveTransferState = eSaveTransfer_GettingFileData;
			}
			else
			{
				pClass->m_eSaveTransferState = eSaveTransfer_Error;
				app.DebugPrintf("app.getRemoteStorage()->getSaveData failed\n");

			}
		}

        case eSaveTransfer_GettingFileData:
            {
                WCHAR wcTemp[256];

                int dataProgress = app.getRemoteStorage()->getDataProgress();
                pMinecraft->progressRenderer->progressStagePercentage(dataProgress);

                //swprintf(wcTemp, 256, L"Downloading data : %d", dataProgress);//app.GetString(IDS_SAVETRANSFER_STAGE_GET_DATA),0,pClass->m_ulFileSize);
                swprintf(wcTemp,256, app.GetString(IDS_SAVETRANSFER_STAGE_GET_DATA),dataProgress);
                m_wstrStageText=wcTemp;
                pMinecraft->progressRenderer->progressStage( m_wstrStageText );
				if(pClass->m_saveTransferDownloadCancelled && bAbortCalled == false)
				{
					app.getRemoteStorage()->abort();
					bAbortCalled = true;
				}
            }
            break;
        case eSaveTransfer_FileDataRetrieved:
			pClass->m_eSaveTransferState = eSaveTransfer_LoadSaveFromDisc;
		break;
		case eSaveTransfer_LoadSaveFromDisc:
		{
			if(pClass->m_saveTransferDownloadCancelled)
			{
				pClass->m_eSaveTransferState = eSaveTransfer_Error;
				break;
			}

			PSAVE_DETAILS pSaveDetails=StorageManager.ReturnSavesInfo();
			int saveInfoIndex = -1;
			for(int i=0;i<pSaveDetails->iSaveC;i++)
			{
				if(strcmp(pSaveDetails->SaveInfoA[i].UTF8SaveFilename, pClass->m_downloadedUniqueFilename) == 0)
				{
					//found it
					saveInfoIndex = i;
				}
			}
			if(saveInfoIndex == -1)
			{
				pClass->m_eSaveTransferState = eSaveTransfer_Error;
				app.DebugPrintf("CrossSaveGetSavesInfoCallback failed - couldn't find save\n");
			}
			else
			{
#ifdef __PS3__
				// ignore the CRC on PS3
				C4JStorage::ESaveGameState eLoadStatus=StorageManager.LoadSaveData(&pSaveDetails->SaveInfoA[saveInfoIndex],&LoadCrossSaveDataCallback,pClass, true);
#else
				C4JStorage::ESaveGameState eLoadStatus=StorageManager.LoadSaveData(&pSaveDetails->SaveInfoA[saveInfoIndex],&LoadCrossSaveDataCallback,pClass);
#endif
				if(eLoadStatus == C4JStorage::ESaveGame_Load)
				{
					pClass->m_eSaveTransferState = eSaveTransfer_LoadingSaveFromDisc;
				}
				else
				{
					pClass->m_eSaveTransferState = eSaveTransfer_Error;
				}
			}
		}
		break;
		case eSaveTransfer_LoadingSaveFromDisc:

			break;
        case eSaveTransfer_CreatingNewSave:
			{
				unsigned int fileSize = StorageManager.GetSaveSize();
				byteArray ba(fileSize);
				StorageManager.GetSaveData(ba.data, &fileSize);
				assert(ba.length == fileSize);


                StorageManager.ResetSaveData();
				{
					PBYTE pbThumbnailData=NULL;
					DWORD dwThumbnailDataSize=0;

					PBYTE pbDataSaveImage=NULL;
					DWORD dwDataSizeSaveImage=0;

					StorageManager.GetDefaultSaveImage(&pbDataSaveImage, &dwDataSizeSaveImage);			// Get the default save thumbnail (as set by SetDefaultImages) for use on saving games t
					StorageManager.GetDefaultSaveThumbnail(&pbThumbnailData,&dwThumbnailDataSize);		// Get the default save image (as set by SetDefaultImages) for use on saving games that 

					BYTE bTextMetadata[88];
					ZeroMemory(bTextMetadata,88);
					int iTextMetadataBytes = app.CreateImageTextData(bTextMetadata, app.getRemoteStorage()->getSaveSeed(), true, app.getRemoteStorage()->getSaveHostOptions(), app.getRemoteStorage()->getSaveTexturePack() );

					// set the icon and save image
					StorageManager.SetSaveImages(pbThumbnailData,dwThumbnailDataSize,pbDataSaveImage,dwDataSizeSaveImage,bTextMetadata,iTextMetadataBytes);
				}


#ifdef SPLIT_SAVES		
                ConsoleSaveFileOriginal oldFormatSave( wSaveName, ba.data, ba.length, false, app.getRemoteStorage()->getSavePlatform() );
                pSave = new ConsoleSaveFileSplit( &oldFormatSave, false, pMinecraft->progressRenderer );

                pMinecraft->progressRenderer->progressStage(IDS_SAVETRANSFER_STAGE_SAVING);
                pSave->Flush(false,false);	
                pClass->m_eSaveTransferState = eSaveTransfer_Saving;
#else
                pSave = new ConsoleSaveFileOriginal( wSaveName, ba.data, ba.length, false, app.getRemoteStorage()->getSavePlatform() );
                pClass->m_eSaveTransferState = eSaveTransfer_Converting;
				pMinecraft->progressRenderer->progressStage(IDS_SAVETRANSFER_STAGE_CONVERTING);
#endif
                delete ba.data;
            }
			break;
        case eSaveTransfer_Converting:
			{
            pSave->ConvertToLocalPlatform(); // check if we need to convert this file from PS3->PS4
            pClass->m_eSaveTransferState = eSaveTransfer_Saving;
            pMinecraft->progressRenderer->progressStage(IDS_SAVETRANSFER_STAGE_SAVING);
			StorageManager.SetSaveTitle(wSaveName);
			StorageManager.SetSaveUniqueFilename(pClass->m_downloadedUniqueFilename);

			app.getRemoteStorage()->waitForStorageManagerIdle();	// we need to wait for the save system to be idle here, as Flush doesn't check for it.
            pSave->Flush(false, false);
			}
            break;
        case eSaveTransfer_Saving:
			{
				// On Durango/Orbis, we need to wait for all the asynchronous saving processes to complete before destroying the levels, as that will ultimately delete
				// the directory level storage & therefore the ConsoleSaveSplit instance, which needs to be around until all the sub files have completed saving.

				delete pSave;

            
				pMinecraft->progressRenderer->progressStage(IDS_PROGRESS_SAVING_TO_DISC);
				pClass->m_eSaveTransferState = eSaveTransfer_Succeeded;
			}
            break;

		case eSaveTransfer_Succeeded:
			{
				// if we've arrived here, the save has been created successfully
				pClass->m_iState=e_SavesRepopulate;
				pClass->updateTooltips();
				UINT uiIDA[1];
				uiIDA[0]=IDS_CONFIRM_OK;
				app.getRemoteStorage()->waitForStorageManagerIdle();	// wait for everything to complete before we hand control back to the player
				ui.RequestMessageBox( IDS_TOOLTIPS_SAVETRANSFER_DOWNLOAD, IDS_SAVE_TRANSFER_DOWNLOADCOMPLETE, uiIDA,1,ProfileManager.GetPrimaryPad(),CrossSaveFinishedCallback,pClass, app.GetStringTable());			
				pClass->m_eSaveTransferState = eSaveTransfer_Finished;
			}
			break;

		case eSaveTransfer_Cancelled: // this is no longer used
			{
				assert(0); //pClass->m_eSaveTransferState = eSaveTransfer_Idle;
			}
			break;
        case eSaveTransfer_Error:
			{
				if(bSaveFileCreated)
				{
					if(pClass->m_saveTransferDownloadCancelled)
					{	
						WCHAR wcTemp[256];
					swprintf(wcTemp,256, app.GetString(IDS_CANCEL));		// MGH - should change this string to "cancelling download"
						m_wstrStageText=wcTemp;
						pMinecraft->progressRenderer->progressStage( m_wstrStageText );
						pMinecraft->progressRenderer->progressStage( m_wstrStageText );
					}
					// if the save file has already been created we have to delete it again if there's been an error
					PSAVE_DETAILS pSaveDetails=StorageManager.ReturnSavesInfo();
					int saveInfoIndex = -1;
					for(int i=0;i<pSaveDetails->iSaveC;i++)
					{
						if(strcmp(pSaveDetails->SaveInfoA[i].UTF8SaveFilename, pClass->m_downloadedUniqueFilename) == 0)
						{
							//found it
							saveInfoIndex = i;
						}
					}
					if(saveInfoIndex == -1)
					{
						app.DebugPrintf("eSaveTransfer_Error failed - couldn't find save\n");
						assert(0);
						pClass->m_eSaveTransferState = eSaveTransfer_ErrorMesssage;
					}
					else
					{
					// delete the save file
					app.getRemoteStorage()->waitForStorageManagerIdle();
						C4JStorage::ESaveGameState eDeleteStatus = StorageManager.DeleteSaveData(&pSaveDetails->SaveInfoA[saveInfoIndex],UIScene_LoadOrJoinMenu::CrossSaveDeleteOnErrorReturned,pClass);
					if(eDeleteStatus == C4JStorage::ESaveGame_Delete)
					{
						pClass->m_eSaveTransferState = eSaveTransfer_ErrorDeletingSave;
					}
					else
					{
						app.DebugPrintf("StorageManager.DeleteSaveData failed!!\n");
						pClass->m_eSaveTransferState = eSaveTransfer_ErrorMesssage;
					}
				}
				}
				else
				{
					pClass->m_eSaveTransferState = eSaveTransfer_ErrorMesssage;
				}
			}
            break;

		case eSaveTransfer_ErrorDeletingSave:
			break;
		case eSaveTransfer_ErrorMesssage:
			{
				app.getRemoteStorage()->waitForStorageManagerIdle();	// wait for everything to complete before we hand control back to the player
				if(pClass->m_saveTransferDownloadCancelled)
				{
					pClass->m_eSaveTransferState = eSaveTransfer_Idle;
				}
				else
				{
					UINT uiIDA[1];
					uiIDA[0]=IDS_CONFIRM_OK;
					ui.RequestMessageBox( IDS_TOOLTIPS_SAVETRANSFER_DOWNLOAD, IDS_SAVE_TRANSFER_DOWNLOADFAILED, uiIDA,1,ProfileManager.GetPrimaryPad(),CrossSaveFinishedCallback,pClass, app.GetStringTable());			
					pClass->m_eSaveTransferState = eSaveTransfer_Finished;
				}
				if(bSaveFileCreated)		// save file has been created, then deleted.
					pClass->m_iState=e_SavesRepopulateAfterDelete;
				else
					pClass->m_iState=e_SavesRepopulate;
				pClass->updateTooltips();
			}
			break;
		case eSaveTransfer_Finished:
			{

			}
			// waiting to dismiss the dialog
			break;
        }
        Sleep(50);
    }

    return 0;

}

void UIScene_LoadOrJoinMenu::SaveTransferReturned(LPVOID lpParam, SonyRemoteStorage::Status s, int error_code)
{
    UIScene_LoadOrJoinMenu* pClass = (UIScene_LoadOrJoinMenu *) lpParam;

    if(s == SonyRemoteStorage::e_getDataSucceeded)
	{
        pClass->m_eSaveTransferState = eSaveTransfer_FileDataRetrieved;
	}
    else
	{
        pClass->m_eSaveTransferState = eSaveTransfer_Error;
		app.DebugPrintf("SaveTransferReturned failed with error code : 0x%08x\n", error_code);
	}

}
ConsoleSaveFile* UIScene_LoadOrJoinMenu::SonyCrossSaveConvert()
{
    return NULL;
}

void UIScene_LoadOrJoinMenu::CancelSaveTransferCallback(LPVOID lpParam)
{
    UIScene_LoadOrJoinMenu* pClass = (UIScene_LoadOrJoinMenu *) lpParam;
	pClass->m_saveTransferDownloadCancelled = true;
	ui.SetTooltips( DEFAULT_XUI_MENU_USER, -1, -1, -1, -1,-1,-1,-1,-1);		// MGH -  added - remove the "cancel" tooltip, so the player knows it's underway (really needs a "cancelling" message)
}

#endif



#ifdef SONY_REMOTE_STORAGE_UPLOAD

void UIScene_LoadOrJoinMenu::LaunchSaveUpload()
{
    LoadingInputParams *loadingParams = new LoadingInputParams();
    loadingParams->func = &UIScene_LoadOrJoinMenu::UploadSonyCrossSaveThreadProc;
    loadingParams->lpParam = (LPVOID)this;

    UIFullscreenProgressCompletionData *completionData = new UIFullscreenProgressCompletionData();
    completionData->bShowBackground=TRUE;
    completionData->bShowLogo=TRUE;
    completionData->type = e_ProgressCompletion_NavigateBackToScene;
    completionData->iPad = DEFAULT_XUI_MENU_USER;
    loadingParams->completionData = completionData;

// 4J-PB - Waiting for Sony to fix canceling a save upload
	loadingParams->cancelFunc=&UIScene_LoadOrJoinMenu::CancelSaveUploadCallback;
	loadingParams->m_cancelFuncParam = this;
 loadingParams->cancelText=IDS_TOOLTIPS_CANCEL;

    ui.NavigateToScene(m_iPad,eUIScene_FullscreenProgress, loadingParams);

}

int UIScene_LoadOrJoinMenu::CrossSaveUploadFinishedCallback(void *pParam,int iPad,C4JStorage::EMessageResult result)
{
	UIScene_LoadOrJoinMenu* pClass = (UIScene_LoadOrJoinMenu *) pParam;
	pClass->m_eSaveUploadState = eSaveUpload_Idle;

	return 0;
}


int UIScene_LoadOrJoinMenu::UploadSonyCrossSaveThreadProc( LPVOID lpParameter )
{
    UIScene_LoadOrJoinMenu* pClass = (UIScene_LoadOrJoinMenu *) lpParameter;
	pClass->m_saveTransferUploadCancelled = false;
	bool bAbortCalled = false;
    Minecraft *pMinecraft=Minecraft::GetInstance();

    // get the save file size
	pMinecraft->progressRenderer->progressStagePercentage(0);
    pMinecraft->progressRenderer->progressStart(IDS_TOOLTIPS_SAVETRANSFER_UPLOAD);
    pMinecraft->progressRenderer->progressStage( IDS_TOOLTIPS_SAVETRANSFER_UPLOAD );

    PSAVE_DETAILS pSaveDetails=StorageManager.ReturnSavesInfo();
    int idx = pClass->m_iSaveListIndex - pClass->m_iDefaultButtonsC;
    bool bSettingOK = app.getRemoteStorage()->setSaveData(&pSaveDetails->SaveInfoA[idx], SaveUploadReturned, pClass);

	if(bSettingOK)
    {
        pClass->m_eSaveUploadState = eSaveUpload_UploadingFileData;
        pMinecraft->progressRenderer->progressStagePercentage(0);
    }
    else
	{
        pClass->m_eSaveUploadState = eSaveUpload_Error;
	}

    while(pClass->m_eSaveUploadState!=eSaveUpload_Idle)
    {
        switch(pClass->m_eSaveUploadState)
        {
        case eSaveUpload_Idle:
            break;
        case eSaveUpload_UploadingFileData:
            {
                WCHAR wcTemp[256];
                int dataProgress = app.getRemoteStorage()->getDataProgress();
                pMinecraft->progressRenderer->progressStagePercentage(dataProgress);

                //swprintf(wcTemp, 256, L"Uploading data : %d", dataProgress);//app.GetString(IDS_SAVETRANSFER_STAGE_GET_DATA),0,pClass->m_ulFileSize);
                swprintf(wcTemp,256, app.GetString(IDS_SAVETRANSFER_STAGE_PUT_DATA),dataProgress);

                m_wstrStageText=wcTemp;
                pMinecraft->progressRenderer->progressStage( m_wstrStageText );
// 4J-PB - Waiting for Sony to fix canceling a save upload
				if(pClass->m_saveTransferUploadCancelled && bAbortCalled == false)
				{
					// we only really want to be able to cancel during the download of data, if it's taking a long time
					app.getRemoteStorage()->abort();
					bAbortCalled = true;
				}
            }
            break;
		case eSaveUpload_FileDataUploaded:
			{
				UINT uiIDA[1];
				uiIDA[0]=IDS_CONFIRM_OK;
				ui.RequestMessageBox( IDS_TOOLTIPS_SAVETRANSFER_UPLOAD, IDS_SAVE_TRANSFER_UPLOADCOMPLETE, uiIDA,1,ProfileManager.GetPrimaryPad(),CrossSaveUploadFinishedCallback,pClass, app.GetStringTable());			
				pClass->m_eSaveUploadState = esaveUpload_Finished;
			}
			break;
		case eSaveUpload_Cancelled: // this is no longer used
			assert(0);//			pClass->m_eSaveUploadState = eSaveUpload_Idle;
			break;
        case eSaveUpload_Error:
			{
				if(pClass->m_saveTransferUploadCancelled)
				{
					pClass->m_eSaveUploadState = eSaveUpload_Idle;
				}
				else
				{
					UINT uiIDA[1];
					uiIDA[0]=IDS_CONFIRM_OK;
					ui.RequestMessageBox( IDS_TOOLTIPS_SAVETRANSFER_UPLOAD, IDS_SAVE_TRANSFER_UPLOADFAILED, uiIDA,1,ProfileManager.GetPrimaryPad(),CrossSaveUploadFinishedCallback,pClass, app.GetStringTable());			
					pClass->m_eSaveUploadState = esaveUpload_Finished;
				}
			}
            break;
		case esaveUpload_Finished:
			// waiting for dialog to be dismissed
			break;
        }
        Sleep(50);
    }

    return 0;

}

void UIScene_LoadOrJoinMenu::SaveUploadReturned(LPVOID lpParam, SonyRemoteStorage::Status s, int error_code)
{
    UIScene_LoadOrJoinMenu* pClass = (UIScene_LoadOrJoinMenu *) lpParam;

	if(pClass->m_saveTransferUploadCancelled)
	{
		UINT uiIDA[1] = { IDS_CONFIRM_OK };
		ui.RequestMessageBox( IDS_CANCEL_UPLOAD_TITLE, IDS_CANCEL_UPLOAD_TEXT, uiIDA, 1, ProfileManager.GetPrimaryPad(), CrossSaveUploadFinishedCallback, pClass, app.GetStringTable() );
		pClass->m_eSaveUploadState=esaveUpload_Finished;
	}
	else
    {
		if(s == SonyRemoteStorage::e_setDataSucceeded)
		   pClass->m_eSaveUploadState = eSaveUpload_FileDataUploaded;
		else if ( !pClass->m_saveTransferUploadCancelled )
			pClass->m_eSaveUploadState = eSaveUpload_Error;
	}
}

void UIScene_LoadOrJoinMenu::CancelSaveUploadCallback(LPVOID lpParam)
{
    UIScene_LoadOrJoinMenu* pClass = (UIScene_LoadOrJoinMenu *) lpParam;
	pClass->m_saveTransferUploadCancelled = true;
	app.DebugPrintf("m_saveTransferUploadCancelled = true\n");
	ui.SetTooltips( DEFAULT_XUI_MENU_USER, -1, -1, -1, -1,-1,-1,-1,-1);		// MGH -  added - remove the "cancel" tooltip, so the player knows it's underway (really needs a "cancelling" message)

	pClass->m_bIgnoreInput = true;
}

int UIScene_LoadOrJoinMenu::SaveTransferDialogReturned(void *pParam,int iPad,C4JStorage::EMessageResult result)
{
	UIScene_LoadOrJoinMenu* pClass = (UIScene_LoadOrJoinMenu*)pParam;
	// results switched for this dialog
	if(result==C4JStorage::EMessage_ResultAccept) 
	{
		// upload the save
		pClass->LaunchSaveUpload();

		pClass->m_bIgnoreInput=false;
	}
	else
	{
		pClass->m_bIgnoreInput=false;
	}
	return 0;
}
#endif // SONY_REMOTE_STORAGE_UPLOAD





