#include "stdafx.h"
#include "..\..\..\Minecraft.World\StringHelpers.h"
#include "..\..\..\Minecraft.World\AABB.h"
#include "..\..\..\Minecraft.World\Vec3.h"
#include "..\..\..\Minecraft.World\Socket.h"
#include "..\..\..\Minecraft.World\ThreadName.h"
#include "..\..\..\Minecraft.World\Entity.h"
#include "..\..\..\Minecraft.World\net.minecraft.world.level.tile.h"
#include "..\..\ClientConnection.h"
#include "..\..\Minecraft.h"
#include "..\..\User.h"
#include "..\..\MinecraftServer.h"
#include "..\..\PlayerList.h"
#include "..\..\ServerPlayer.h"
#include "..\..\PlayerConnection.h"
#include "..\..\MultiPlayerLevel.h"
#include "..\..\ProgressRenderer.h"
#include "..\..\MultiPlayerLocalPlayer.h"
#include "..\..\..\Minecraft.World\DisconnectPacket.h"
#include "..\..\..\Minecraft.World\compression.h"
#include "..\..\..\Minecraft.World\OldChunkStorage.h"
#include "..\..\TexturePackRepository.h"
#include "..\..\TexturePack.h"

#include "..\..\Gui.h"
#include "..\..\LevelRenderer.h"
#include "..\..\..\Minecraft.World\IntCache.h"
#include "..\GameRules\ConsoleGameRules.h"
#include "GameNetworkManager.h"

#include "Common\UI\UI.h"
#include "Common\UI\UIScene_PauseMenu.h"
#include "..\..\Xbox\Network\NetworkPlayerXbox.h"


// Global instance
CGameNetworkManager g_NetworkManager;
CPlatformNetworkManager *CGameNetworkManager::s_pPlatformNetworkManager;

__int64 CGameNetworkManager::messageQueue[512];
__int64 CGameNetworkManager::byteQueue[512];
int CGameNetworkManager::messageQueuePos = 0;

CGameNetworkManager::CGameNetworkManager()
{
	m_bInitialised = false;
	m_bLastDisconnectWasLostRoomOnly = false;
	m_bFullSessionMessageOnNextSessionChange = false;

}

void CGameNetworkManager::Initialise()
{
	ServerStoppedCreate( false );
	ServerReadyCreate( false );
	int flagIndexSize = LevelRenderer::getGlobalChunkCount() / (Level::maxBuildHeight / 16);		// dividing here by number of renderer chunks in one column
#if defined __PS3__
	s_pPlatformNetworkManager = new CPlatformNetworkManagerSony();
#else
	s_pPlatformNetworkManager = new CPlatformNetworkManagerStub();
#endif
	s_pPlatformNetworkManager->Initialise( this, flagIndexSize );		
	m_bNetworkThreadRunning = false;
	m_bInitialised = true;
}

void CGameNetworkManager::Terminate()
{
	if( m_bInitialised )
	{
		s_pPlatformNetworkManager->Terminate();
	}
}

void CGameNetworkManager::DoWork()
{
	s_pPlatformNetworkManager->DoWork();

}

bool CGameNetworkManager::_RunNetworkGame(LPVOID lpParameter)
{
	bool success = true;

	bool isHost = g_NetworkManager.IsHost();
	// Start the network game
	Minecraft *pMinecraft=Minecraft::GetInstance();
	success = StartNetworkGame(pMinecraft,lpParameter);

	if(!success) return false;

	if( isHost )
	{
		// We do not have a lobby, so the only players in the game at this point are local ones.

		success = s_pPlatformNetworkManager->_RunNetworkGame();
		if(!success)
		{			
			app.SetAction(ProfileManager.GetPrimaryPad(),eAppAction_ExitWorld,(void *)TRUE);
			return true;
		}
	}
	
	if( g_NetworkManager.IsLeavingGame() ) return false;

	app.SetGameStarted(true);

	// 4J-PB - if this is the trial game, start the trial timer
	if(!ProfileManager.IsFullVersion())
	{
		ui.SetTrialTimerLimitSecs(MinecraftDynamicConfigurations::GetTrialTime());
		app.SetTrialTimerStart();
	}
	//app.CloseXuiScenes(ProfileManager.GetPrimaryPad());

	return success;
}

bool	CGameNetworkManager::StartNetworkGame(Minecraft *minecraft, LPVOID lpParameter)
{

	__int64 seed = 0;
	if(lpParameter != NULL)
	{
		NetworkGameInitData *param = (NetworkGameInitData *)lpParameter;
		seed = param->seed;
		
		app.setLevelGenerationOptions(param->levelGen);
		if(param->levelGen != NULL)
		{
			if(app.getLevelGenerationOptions() == NULL)
			{
				app.DebugPrintf("Game rule was not loaded, and seed is required. Exiting.\n");
				return false;
			}
			else
			{
				param->seed = seed = app.getLevelGenerationOptions()->getLevelSeed();					
			}
		}
	}

	static __int64 sseed = seed;	// Create static version so this will be valid until next call to this function & whilst thread is running
	ServerStoppedCreate(false);
	if( g_NetworkManager.IsHost() )
	{
		ServerStoppedCreate(true);
		ServerReadyCreate(true);
		// Ready to go - create actual networking thread & start hosting
		C4JThread* thread = new C4JThread(&CGameNetworkManager::ServerThreadProc, lpParameter, "Server", 256 * 1024);
#if defined __PS3__
		thread->SetPriority(THREAD_PRIORITY_BELOW_NORMAL);
#endif

		thread->SetProcessor(CPU_CORE_SERVER);
		thread->Run();

		ServerReadyWait();
		ServerReadyDestroy();

		if( MinecraftServer::serverHalted() ) 
			return false;

//		printf("Server ready to go!\n");
	}
	else
	{
		Socket::Initialise(NULL);
	}

	Minecraft *pMinecraft = Minecraft::GetInstance();	
	// Make sure that we have transitioned through any joining/creating stages and are actually playing the game, so that we know the players should be valid
	bool changedMessage = false;
	while(!IsReadyToPlayOrIdle())
	{
		changedMessage = true;
		pMinecraft->progressRenderer->progressStage( g_NetworkManager.CorrectErrorIDS(IDS_PROGRESS_SAVING_TO_DISC) );		// "Finalizing..." vaguest message I could find
		pMinecraft->progressRenderer->progressStagePercentage( g_NetworkManager.GetJoiningReadyPercentage() );
		Sleep(10);
	}
	if( changedMessage )
	{
		pMinecraft->progressRenderer->progressStagePercentage( 100 );
	}

	// If we aren't in session, then something bad must have happened - we aren't joining, creating or ready play
	if(!IsInSession() )
	{
		MinecraftServer::HaltServer();
		return false;
	}

	// 4J Stu - Wait a while to make sure that DLC is loaded. This is the last point before the network communication starts
	// so the latest we can check this
	while( !app.DLCInstallProcessCompleted() && app.DLCInstallPending() && !g_NetworkManager.IsLeavingGame() )
	{
		Sleep( 10 );
	}
	if( g_NetworkManager.IsLeavingGame() )
	{
		MinecraftServer::HaltServer();
		return false;
	}

	// PRIMARY PLAYER

	vector<ClientConnection *> createdConnections;
	ClientConnection *connection;

	if( g_NetworkManager.IsHost() )
	{
		connection = new ClientConnection(minecraft, NULL);
	}
	else
	{
		INetworkPlayer *pNetworkPlayer = g_NetworkManager.GetLocalPlayerByUserIndex(ProfileManager.GetLockedProfile());
		if(pNetworkPlayer == NULL)
		{
			MinecraftServer::HaltServer();
			app.DebugPrintf("%d\n",ProfileManager.GetLockedProfile());
			// If the player is NULL here then something went wrong in the session setup, and continuing will end up in a crash
			return false;
		}

		Socket *socket = pNetworkPlayer->GetSocket();

		// Fix for #13259 - CRASH: Gameplay: loading process is halted when player loads saved data
		if(socket == NULL)
		{
			assert(false);
			MinecraftServer::HaltServer();
			// If the socket is NULL here then something went wrong in the session setup, and continuing will end up in a crash
			return false;
		}

		connection = new ClientConnection(minecraft, socket);
	}

	if( !connection->createdOk )
	{
		assert(false);
		delete connection;
		connection = NULL;
		MinecraftServer::HaltServer();
		return false;
	}

	connection->send( shared_ptr<PreLoginPacket>( new PreLoginPacket(minecraft->user->name) ) );

	// Tick connection until we're ready to go. The stages involved in this are:
	// (1) Creating the ClientConnection sends a prelogin packet to the server
	// (2) the server sends a prelogin back, which is handled by the clientConnection, and returns a login packet
	// (3) the server sends a login back, which is handled by the client connection to start the game
	if( !g_NetworkManager.IsHost() )
	{
		Minecraft::GetInstance()->progressRenderer->progressStart(IDS_PROGRESS_CONNECTING);
	}
	else
	{
		// 4J Stu - Host needs to generate a unique multiplayer id for sentient telemetry reporting
		INT multiplayerInstanceId = TelemetryManager->GenerateMultiplayerInstanceId();
		TelemetryManager->SetMultiplayerInstanceId(multiplayerInstanceId);
	}
	TexturePack *tPack = Minecraft::GetInstance()->skins->getSelected();
	do
	{
		app.DebugPrintf("ticking connection A\n");
		connection->tick();

		// 4J Stu - We were ticking this way too fast which could cause the connection to time out
		// The connections should tick at 20 per second
		Sleep(50);
	} while ( (IsInSession() && !connection->isStarted() && !connection->isClosed() && !g_NetworkManager.IsLeavingGame()) || tPack->isLoadingData() || (Minecraft::GetInstance()->skins->needsUIUpdate() || ui.IsReloadingSkin()) );
	ui.CleanUpSkinReload();

	// 4J Stu - Fix for #11279 - CRASH: TCR 001: BAS Game Stability: Signing out of game will cause title to crash
	// We need to break out of the above loop if m_bLeavingGame is set, and close the connection
	if( g_NetworkManager.IsLeavingGame() || !IsInSession() )
	{
		connection->close();
	}

	if( connection->isStarted() && !connection->isClosed() )
	{
		createdConnections.push_back( connection );

		int primaryPad = ProfileManager.GetPrimaryPad();
		app.SetRichPresenceContext(primaryPad,CONTEXT_GAME_STATE_BLANK);
		if (GetPlayerCount() > 1)	// Are we offline or online, and how many players are there
		{
			if (IsLocalGame())	ProfileManager.SetCurrentGameActivity(primaryPad,CONTEXT_PRESENCE_MULTIPLAYEROFFLINE,false);
			else				ProfileManager.SetCurrentGameActivity(primaryPad,CONTEXT_PRESENCE_MULTIPLAYER,false);
		}
		else
		{
			if(IsLocalGame())	ProfileManager.SetCurrentGameActivity(primaryPad,CONTEXT_PRESENCE_MULTIPLAYER_1POFFLINE,false);
			else				ProfileManager.SetCurrentGameActivity(primaryPad,CONTEXT_PRESENCE_MULTIPLAYER_1P,false);
		}


		// ALL OTHER LOCAL PLAYERS
		for(int idx = 0; idx < XUSER_MAX_COUNT; ++idx)
		{
			// Already have setup the primary pad
			if(idx == ProfileManager.GetPrimaryPad() ) continue;

			if( GetLocalPlayerByUserIndex(idx) != NULL && !ProfileManager.IsSignedIn(idx) )
			{
				INetworkPlayer *pNetworkPlayer = g_NetworkManager.GetLocalPlayerByUserIndex(idx);
				Socket *socket = pNetworkPlayer->GetSocket();
				app.DebugPrintf("Closing socket due to player %d not being signed in any more\n");
				if( !socket->close(false) ) socket->close(true);

				continue;
			}

			// By default when we host we only have the local player, but currently allow multiple local players to join
			// when joining any other way, so just because they are signed in doesn't mean they are in the session
			// 4J Stu - If they are in the session, then we should add them to the game. Otherwise we won't be able to add them later
			INetworkPlayer *pNetworkPlayer = g_NetworkManager.GetLocalPlayerByUserIndex(idx);
			if( pNetworkPlayer == NULL )
				continue;

			ClientConnection *connection;

			Socket *socket = pNetworkPlayer->GetSocket();
			connection = new ClientConnection(minecraft, socket, idx);

			minecraft->addPendingLocalConnection(idx, connection);
			//minecraft->createExtraLocalPlayer(idx, (convStringToWstring( ProfileManager.GetGamertag(idx) )).c_str(), idx, connection);

			// Open the socket on the server end to accept incoming data
			Socket::addIncomingSocket(socket);

			connection->send( shared_ptr<PreLoginPacket>( new PreLoginPacket(convStringToWstring( ProfileManager.GetGamertag(idx) )) ) );

			createdConnections.push_back( connection );

			// Tick connection until we're ready to go. The stages involved in this are:
			// (1) Creating the ClientConnection sends a prelogin packet to the server
			// (2) the server sends a prelogin back, which is handled by the clientConnection, and returns a login packet
			// (3) the server sends a login back, which is handled by the client connection to start the game
			do
			{
				// We need to keep ticking the connections for players that already logged in
				for(AUTO_VAR(it, createdConnections.begin()); it < createdConnections.end(); ++it)
				{
					(*it)->tick();
				}

				// 4J Stu - We were ticking this way too fast which could cause the connection to time out
				// The connections should tick at 20 per second
				Sleep(50);
				app.DebugPrintf("<***> %d %d %d %d %d\n",IsInSession(), !connection->isStarted(),!connection->isClosed(),ProfileManager.IsSignedIn(idx),!g_NetworkManager.IsLeavingGame());
#if __PS3__
			} while (IsInSession() && !connection->isStarted() && !connection->isClosed() && ProfileManager.IsSignedIn(idx) && !g_NetworkManager.IsLeavingGame() );
#else
				// TODO - This SHOULD be something just like the code above but temporarily changing here so that we don't have to depend on the profilemanager behaviour
			} while (IsInSession() && !connection->isStarted() && !connection->isClosed() && !g_NetworkManager.IsLeavingGame() );
#endif

			// 4J Stu - Fix for #11279 - CRASH: TCR 001: BAS Game Stability: Signing out of game will cause title to crash
			// We need to break out of the above loop if m_bLeavingGame is set, and stop creating new connections
			// The connections in the createdConnections vector get closed at the end of the thread
			if( g_NetworkManager.IsLeavingGame() || !IsInSession() ) break;

			if( ProfileManager.IsSignedIn(idx) && !connection->isClosed() )
			{
				app.SetRichPresenceContext(idx,CONTEXT_GAME_STATE_BLANK);
				if (IsLocalGame())	ProfileManager.SetCurrentGameActivity(idx,CONTEXT_PRESENCE_MULTIPLAYEROFFLINE,false);
				else				ProfileManager.SetCurrentGameActivity(idx,CONTEXT_PRESENCE_MULTIPLAYER,false);
			}
			else
			{
				connection->close();
				AUTO_VAR(it, find( createdConnections.begin(), createdConnections.end(), connection ));
				if(it != createdConnections.end() ) createdConnections.erase( it );
			}
		}

		app.SetGameMode( eMode_Multiplayer );
	}
	else if ( connection->isClosed() || !IsInSession())
	{
//		assert(false);
		MinecraftServer::HaltServer();
		return false;
	}

	
	if(g_NetworkManager.IsLeavingGame() || !IsInSession() )
	{
		for(AUTO_VAR(it, createdConnections.begin()); it < createdConnections.end(); ++it)
		{
			(*it)->close();
		}
//		assert(false);
		MinecraftServer::HaltServer();
		return false;
	}

	// Catch in-case server has been halted (by a player signout).
	if ( MinecraftServer::serverHalted() )
		return false;

	return true;
}

int CGameNetworkManager::CorrectErrorIDS(int IDS)
{
	return s_pPlatformNetworkManager->CorrectErrorIDS(IDS);
}

int CGameNetworkManager::GetLocalPlayerMask(int playerIndex)
{
	return s_pPlatformNetworkManager->GetLocalPlayerMask( playerIndex );
}

int CGameNetworkManager::GetPlayerCount()
{
	return s_pPlatformNetworkManager->GetPlayerCount();
}

int CGameNetworkManager::GetOnlinePlayerCount()
{
	return s_pPlatformNetworkManager->GetOnlinePlayerCount();
}

bool CGameNetworkManager::AddLocalPlayerByUserIndex( int userIndex )
{
	return s_pPlatformNetworkManager->AddLocalPlayerByUserIndex( userIndex );
}

bool	CGameNetworkManager::RemoveLocalPlayerByUserIndex( int userIndex )
{
	return s_pPlatformNetworkManager->RemoveLocalPlayerByUserIndex( userIndex );
}

INetworkPlayer *CGameNetworkManager::GetLocalPlayerByUserIndex(int userIndex )
{
	return s_pPlatformNetworkManager->GetLocalPlayerByUserIndex( userIndex );
}

INetworkPlayer *CGameNetworkManager::GetPlayerByIndex(int playerIndex)
{
	return s_pPlatformNetworkManager->GetPlayerByIndex( playerIndex );
}

INetworkPlayer	*CGameNetworkManager::GetPlayerByXuid(PlayerUID xuid)
{
	return s_pPlatformNetworkManager->GetPlayerByXuid( xuid );
}

INetworkPlayer *CGameNetworkManager::GetPlayerBySmallId(unsigned char smallId)
{
	return s_pPlatformNetworkManager->GetPlayerBySmallId( smallId );
}


INetworkPlayer *CGameNetworkManager::GetHostPlayer()
{
	return s_pPlatformNetworkManager->GetHostPlayer();
}

void CGameNetworkManager::RegisterPlayerChangedCallback(int iPad, void (*callback)(void *callbackParam, INetworkPlayer *pPlayer, bool leaving), void *callbackParam)
{
	s_pPlatformNetworkManager->RegisterPlayerChangedCallback( iPad, callback, callbackParam );
}

void CGameNetworkManager::UnRegisterPlayerChangedCallback(int iPad, void (*callback)(void *callbackParam, INetworkPlayer *pPlayer, bool leaving), void *callbackParam)
{
	s_pPlatformNetworkManager->UnRegisterPlayerChangedCallback( iPad, callback, callbackParam );
}

void CGameNetworkManager::HandleSignInChange()
{
	s_pPlatformNetworkManager->HandleSignInChange();
}

bool CGameNetworkManager::ShouldMessageForFullSession()
{
	return s_pPlatformNetworkManager->ShouldMessageForFullSession();
}

bool CGameNetworkManager::IsInSession()
{
	return s_pPlatformNetworkManager->IsInSession();
}

bool CGameNetworkManager::IsInGameplay()
{
	return s_pPlatformNetworkManager->IsInGameplay();
}

bool CGameNetworkManager::IsReadyToPlayOrIdle()
{
	return s_pPlatformNetworkManager->IsReadyToPlayOrIdle();
}

bool CGameNetworkManager::IsLeavingGame()
{
	return s_pPlatformNetworkManager->IsLeavingGame();
}

bool CGameNetworkManager::SetLocalGame(bool isLocal)
{
	return s_pPlatformNetworkManager->SetLocalGame( isLocal );
}

bool CGameNetworkManager::IsLocalGame()
{
	return s_pPlatformNetworkManager->IsLocalGame();
}

void CGameNetworkManager::SetPrivateGame(bool isPrivate)
{
	s_pPlatformNetworkManager->SetPrivateGame( isPrivate );
}

bool CGameNetworkManager::IsPrivateGame()
{
	return s_pPlatformNetworkManager->IsPrivateGame();
}

void CGameNetworkManager::HostGame(int localUsersMask, bool bOnlineGame, bool bIsPrivate, unsigned char publicSlots, unsigned char privateSlots)
{
	// 4J Stu - clear any previous connection errors
	Minecraft::GetInstance()->clearConnectionFailed();

	s_pPlatformNetworkManager->HostGame( localUsersMask, bOnlineGame, bIsPrivate, publicSlots, privateSlots );
}

bool CGameNetworkManager::IsHost()
{
	return (s_pPlatformNetworkManager->IsHost() == TRUE);
}

bool CGameNetworkManager::IsInStatsEnabledSession()
{
	return s_pPlatformNetworkManager->IsInStatsEnabledSession();
}

bool CGameNetworkManager::SessionHasSpace(unsigned int spaceRequired)
{
	return s_pPlatformNetworkManager->SessionHasSpace( spaceRequired );
}

vector<FriendSessionInfo *>	*CGameNetworkManager::GetSessionList(int iPad, int localPlayers, bool partyOnly)
{
	return s_pPlatformNetworkManager->GetSessionList( iPad, localPlayers, partyOnly );
}

bool CGameNetworkManager::GetGameSessionInfo(int iPad, SessionID sessionId,FriendSessionInfo *foundSession)
{
	return s_pPlatformNetworkManager->GetGameSessionInfo( iPad, sessionId, foundSession );
}

void CGameNetworkManager::SetSessionsUpdatedCallback( void (*SessionsUpdatedCallback)(LPVOID pParam), LPVOID pSearchParam )
{
	s_pPlatformNetworkManager->SetSessionsUpdatedCallback( SessionsUpdatedCallback, pSearchParam );
}

void CGameNetworkManager::GetFullFriendSessionInfo( FriendSessionInfo *foundSession, void (* FriendSessionUpdatedFn)(bool success, void *pParam), void *pParam )
{
	s_pPlatformNetworkManager->GetFullFriendSessionInfo(foundSession, FriendSessionUpdatedFn, pParam);
}

void CGameNetworkManager::ForceFriendsSessionRefresh()
{
	s_pPlatformNetworkManager->ForceFriendsSessionRefresh();
}

bool CGameNetworkManager::JoinGameFromInviteInfo( int userIndex, int userMask, const INVITE_INFO *pInviteInfo)
{
	return s_pPlatformNetworkManager->JoinGameFromInviteInfo( userIndex, userMask, pInviteInfo );
}

CGameNetworkManager::eJoinGameResult CGameNetworkManager::JoinGame(FriendSessionInfo *searchResult, int localUsersMask)
{
	app.SetTutorialMode( false );
	g_NetworkManager.SetLocalGame(false);

	int primaryUserIndex = ProfileManager.GetLockedProfile();

	// 4J-PB - clear any previous connection errors
	Minecraft::GetInstance()->clearConnectionFailed();

	// Make sure that the Primary Pad is in by default
	localUsersMask |= GetLocalPlayerMask( ProfileManager.GetPrimaryPad() );

	return (eJoinGameResult)(s_pPlatformNetworkManager->JoinGame( searchResult, localUsersMask, primaryUserIndex ));
}

void CGameNetworkManager::CancelJoinGame(LPVOID lpParam)
{
}

bool CGameNetworkManager::LeaveGame(bool bMigrateHost)
{
	Minecraft::GetInstance()->gui->clearMessages();
	return s_pPlatformNetworkManager->LeaveGame( bMigrateHost );
}

int CGameNetworkManager::JoinFromInvite_SignInReturned(void *pParam,bool bContinue, int iPad)
{
	INVITE_INFO * pInviteInfo = (INVITE_INFO *)pParam;

	if(bContinue==true)
	{

		app.DebugPrintf("JoinFromInvite_SignInReturned, iPad %d\n",iPad);
		// It's possible that the player has not signed in - they can back out
		if(ProfileManager.IsSignedIn(iPad) && ProfileManager.IsSignedInLive(iPad) )
		{
			app.DebugPrintf("JoinFromInvite_SignInReturned, passed sign-in tests\n");
			int localUsersMask = 0;
			int joiningUsers = 0;

			bool noPrivileges = false;
			for(unsigned int index = 0; index < XUSER_MAX_COUNT; ++index)
			{
				if(ProfileManager.IsSignedIn(index) )
				{
					++joiningUsers;
					if( !ProfileManager.AllowedToPlayMultiplayer(index) ) noPrivileges = true;
					localUsersMask |= GetLocalPlayerMask( index );
				}
			}

			// Check if user-created content is allowed, as we cannot play multiplayer if it's not
			bool noUGC = false;
#if defined(__PS3__)
			ProfileManager.GetChatAndContentRestrictions(iPad,false,&noUGC,NULL,NULL);
#endif

			if(noUGC)
			{
				int messageText = IDS_NO_USER_CREATED_CONTENT_PRIVILEGE_SINGLE_LOCAL;
				if(joiningUsers > 1) messageText = IDS_NO_USER_CREATED_CONTENT_PRIVILEGE_ALL_LOCAL;

				ui.RequestUGCMessageBox(IDS_CONNECTION_FAILED, messageText);
			}
			else if(noPrivileges)
			{
				UINT uiIDA[1];
				uiIDA[0]=IDS_CONFIRM_OK;
				ui.RequestMessageBox( IDS_NO_MULTIPLAYER_PRIVILEGE_TITLE, IDS_NO_MULTIPLAYER_PRIVILEGE_JOIN_TEXT, uiIDA,1,ProfileManager.GetPrimaryPad(),NULL,NULL, app.GetStringTable());
			}
			else
			{
				ProfileManager.SetLockedProfile(iPad);
				ProfileManager.SetPrimaryPad(iPad);

				g_NetworkManager.SetLocalGame(false);
			
				// If the player was signed in before selecting play, we'll not have read the profile yet, so query the sign-in status to get this to happen
				ProfileManager.QuerySigninStatus();

				// 4J-PB - clear any previous connection errors
				Minecraft::GetInstance()->clearConnectionFailed();

				// change the minecraft player name
				Minecraft::GetInstance()->user->name = convStringToWstring( ProfileManager.GetGamertag(ProfileManager.GetPrimaryPad()));

				bool success = g_NetworkManager.JoinGameFromInviteInfo(
							iPad, // dwUserIndex
							localUsersMask,   // dwUserMask
							pInviteInfo );      // pInviteInfo
				if( !success )
				{
					app.DebugPrintf( "Failed joining game from invite\n" ); 
				}
			}
		}
		else
		{
			app.DebugPrintf("JoinFromInvite_SignInReturned, failed sign-in tests :%d %d\n",ProfileManager.IsSignedIn(iPad),ProfileManager.IsSignedInLive(iPad));
		}
	}
	return 0;

}

void CGameNetworkManager::UpdateAndSetGameSessionData(INetworkPlayer *pNetworkPlayerLeaving)
{
	Minecraft *pMinecraft = Minecraft::GetInstance();
	TexturePack *tPack = pMinecraft->skins->getSelected();
	s_pPlatformNetworkManager->SetSessionTexturePackParentId( tPack->getDLCParentPackId() );
	s_pPlatformNetworkManager->SetSessionSubTexturePackId( tPack->getDLCSubPackId() );

	s_pPlatformNetworkManager->UpdateAndSetGameSessionData( pNetworkPlayerLeaving );
}

void CGameNetworkManager::SendInviteGUI(int quadrant)
{
	s_pPlatformNetworkManager->SendInviteGUI(quadrant);
}

void CGameNetworkManager::ResetLeavingGame()
{
	s_pPlatformNetworkManager->ResetLeavingGame();
}

bool CGameNetworkManager::IsNetworkThreadRunning()
{
	return m_bNetworkThreadRunning;;
}

int CGameNetworkManager::RunNetworkGameThreadProc( void* lpParameter )
{
	// Share AABB & Vec3 pools with default (main thread) - should be ok as long as we don't tick the main thread whilst this thread is running
	AABB::UseDefaultThreadStorage();
	Vec3::UseDefaultThreadStorage();
	Compression::UseDefaultThreadStorage();
	Tile::CreateNewThreadStorage();
	IntCache::CreateNewThreadStorage();
	
	g_NetworkManager.m_bNetworkThreadRunning = true;
	bool success = g_NetworkManager._RunNetworkGame(lpParameter);
	g_NetworkManager.m_bNetworkThreadRunning = false;
	if( !success)
	{
		TexturePack *tPack = Minecraft::GetInstance()->skins->getSelected();
		while ( tPack->isLoadingData() || (Minecraft::GetInstance()->skins->needsUIUpdate() || ui.IsReloadingSkin()) )
		{
			Sleep(1);
		}
		ui.CleanUpSkinReload();
		if(app.GetDisconnectReason() == DisconnectPacket::eDisconnect_None) 
		{
			app.SetDisconnectReason( DisconnectPacket::eDisconnect_ConnectionCreationFailed );
		}
		// If we failed before the server started, clear the game rules. Otherwise the server will clear it up.
		if(MinecraftServer::getInstance() == NULL) app.m_gameRules.unloadCurrentGameRules();
		Tile::ReleaseThreadStorage();
		return -1;
	}


	Tile::ReleaseThreadStorage();
	IntCache::ReleaseThreadStorage();
	return 0;
}

int CGameNetworkManager::ServerThreadProc( void* lpParameter )
{
	__int64 seed = 0;
	if(lpParameter != NULL)
	{
		NetworkGameInitData *param = (NetworkGameInitData *)lpParameter;
		seed = param->seed;
		app.SetGameHostOption(eGameHostOption_All,param->settings);
	}

	SetThreadName(-1, "Minecraft Server thread");
	AABB::CreateNewThreadStorage();
	Vec3::CreateNewThreadStorage();
	IntCache::CreateNewThreadStorage();	
	Compression::UseDefaultThreadStorage();
	OldChunkStorage::UseDefaultThreadStorage();
	Entity::useSmallIds();
	Level::enableLightingCache();
	Tile::CreateNewThreadStorage();

	MinecraftServer::main(seed, lpParameter); //saveData, app.GetGameHostOption(eGameHostOption_All));
	
	Tile::ReleaseThreadStorage();
	AABB::ReleaseThreadStorage();
	Vec3::ReleaseThreadStorage();
	IntCache::ReleaseThreadStorage();
	Level::destroyLightingCache();

	if(lpParameter != NULL) delete lpParameter;

	return S_OK;
}

int	CGameNetworkManager::ExitAndJoinFromInviteThreadProc( void* lpParam )
{
	// Share AABB & Vec3 pools with default (main thread) - should be ok as long as we don't tick the main thread whilst this thread is running
	AABB::UseDefaultThreadStorage();
	Vec3::UseDefaultThreadStorage();
	Compression::UseDefaultThreadStorage();

	//app.SetGameStarted(false);

	UIScene_PauseMenu::_ExitWorld(NULL);

	while( g_NetworkManager.IsInSession() )
	{
		Sleep(1);
	}

	// Xbox should always be online when receiving invites - on PS3 we need to check & ask the user to sign in
#ifndef __PS3__
	JoinFromInviteData *inviteData = (JoinFromInviteData *)lpParam;
	app.SetAction(inviteData->dwUserIndex, eAppAction_JoinFromInvite, lpParam);
#else
	if(ProfileManager.IsSignedInLive(ProfileManager.GetPrimaryPad()))
	{
		JoinFromInviteData *inviteData = (JoinFromInviteData *)lpParam;
		app.SetAction(inviteData->dwUserIndex, eAppAction_JoinFromInvite, lpParam);
	}
	else
	{
		UINT uiIDA[2];
		uiIDA[0]=IDS_PRO_NOTONLINE_ACCEPT;
		uiIDA[1]=IDS_PRO_NOTONLINE_DECLINE;
		ui.RequestMessageBox(IDS_PRO_NOTONLINE_TITLE, IDS_PRO_NOTONLINE_TEXT, uiIDA, 2, ProfileManager.GetPrimaryPad(),&CGameNetworkManager::MustSignInReturned_0,lpParam, app.GetStringTable());
	}
#endif

	return S_OK;
}

#if defined __PS3__
// This case happens when we have been returned from the game to the main menu after receiving an invite and are now trying to go back in to join the new game
// The pair of methods MustSignInReturned_0 & PSNSignInReturned_0 handle this
int	CGameNetworkManager::MustSignInReturned_0(void *pParam,int iPad,C4JStorage::EMessageResult result)
{
	if(result==C4JStorage::EMessage_ResultAccept) 
	{
#if defined(__PS3__)
		SQRNetworkManager_PS3::AttemptPSNSignIn(&CGameNetworkManager::PSNSignInReturned_0, pParam,true);
#endif
	}
	else
	{
		app.SetAction(0,eAppAction_Idle);
		ui.NavigateToHomeMenu();
		ui.UpdatePlayerBasePositions();
	}

	return 0;
}

int CGameNetworkManager::PSNSignInReturned_0(void* pParam, bool bContinue, int iPad)
{
	JoinFromInviteData *inviteData = (JoinFromInviteData *)pParam;

	// If the invite data isn't set up yet (indicated by it being all zeroes, easiest detected via the net version), then try and get it again... this can happen if we got
	// the invite whilst signed out

	if( bContinue )
	{
		if(inviteData->pInviteInfo->netVersion == 0)
		{
#if defined __PS3__ || defined __VITA__
			if(!SQRNetworkManager_PS3::UpdateInviteData((SQRNetworkManager::PresenceSyncInfo *)inviteData->pInviteInfo))
			{
				bContinue = false;
			}
#endif
		}
	}

	if( bContinue )
	{
		app.SetAction(inviteData->dwUserIndex, eAppAction_JoinFromInvite, pParam);
	}
	else
	{
		app.SetAction(inviteData->dwUserIndex,eAppAction_Idle);
		ui.NavigateToHomeMenu();
		ui.UpdatePlayerBasePositions();
	}

	return 0;
}

// This case happens when we were in the main menus when we got an invite, and weren't signed in... now can proceed with the normal flow of code for this situation
// The pair of methods MustSignInReturned_1 & PSNSignInReturned_1 handle this
int	CGameNetworkManager::MustSignInReturned_1(void *pParam,int iPad,C4JStorage::EMessageResult result)
{
	if(result==C4JStorage::EMessage_ResultAccept) 
	{
#if defined(__PS3__)
		SQRNetworkManager_PS3::AttemptPSNSignIn(&CGameNetworkManager::PSNSignInReturned_1, pParam,true);
#endif
	}
	return 0;
}

int CGameNetworkManager::PSNSignInReturned_1(void* pParam, bool bContinue, int iPad)
{
	INVITE_INFO *inviteInfo = (INVITE_INFO *)pParam;

	// If the invite data isn't set up yet (indicated by it being all zeroes, easiest detected via the net version), then try and get it again... this can happen if we got
	// the invite whilst signed out

	if( bContinue )
	{
		if(inviteInfo->netVersion == 0)
		{
#if defined __PS3__ || defined __VITA__
			if(!SQRNetworkManager_PS3::UpdateInviteData((SQRNetworkManager::PresenceSyncInfo *)inviteInfo))
			{
				bContinue = false;
			}
#endif
			
		}
	}

	if( bContinue )
	{
		g_NetworkManager.HandleInviteWhenInMenus(0, inviteInfo);
	}

	return 0;
}
#endif

void CGameNetworkManager::_LeaveGame()
{
	s_pPlatformNetworkManager->_LeaveGame(false, true);
}

int CGameNetworkManager::ChangeSessionTypeThreadProc( void* lpParam )
{
	// Share AABB & Vec3 pools with default (main thread) - should be ok as long as we don't tick the main thread whilst this thread is running
	AABB::UseDefaultThreadStorage();
	Vec3::UseDefaultThreadStorage();
	Compression::UseDefaultThreadStorage();

	Minecraft *pMinecraft = Minecraft::GetInstance();	
	MinecraftServer *pServer = MinecraftServer::getInstance();

#if defined(__PS3__)
	UINT uiIDA[1];
	uiIDA[0]=IDS_CONFIRM_OK;
	if( g_NetworkManager.m_bLastDisconnectWasLostRoomOnly )
	{
		if(g_NetworkManager.m_bSignedOutofPSN)
		{
			C4JStorage::EMessageResult result = ui.RequestMessageBox( IDS_PROGRESS_CONVERTING_TO_OFFLINE_GAME, IDS_ERROR_PSN_SIGN_OUT, uiIDA,1,ProfileManager.GetPrimaryPad());
		}
		else
		{
			C4JStorage::EMessageResult result = ui.RequestMessageBox( IDS_ERROR_NETWORK_TITLE, IDS_PROGRESS_CONVERTING_TO_OFFLINE_GAME, uiIDA,1,ProfileManager.GetPrimaryPad());
		}
	}
	else
	{
		C4JStorage::EMessageResult result = ui.RequestMessageBox( IDS_CONNECTION_LOST, g_NetworkManager.CorrectErrorIDS(IDS_CONNECTION_LOST_LIVE_NO_EXIT), uiIDA,1,ProfileManager.GetPrimaryPad());
	}

	// Swap these two messages around as one is too long to display at 480
	pMinecraft->progressRenderer->progressStartNoAbort( IDS_PROGRESS_CONVERTING_TO_OFFLINE_GAME );
	pMinecraft->progressRenderer->progressStage( -1 ); //g_NetworkManager.CorrectErrorIDS(IDS_CONNECTION_LOST_LIVE_NO_EXIT) );
#else
	pMinecraft->progressRenderer->progressStartNoAbort( g_NetworkManager.CorrectErrorIDS(IDS_CONNECTION_LOST_LIVE_NO_EXIT) );
	pMinecraft->progressRenderer->progressStage( IDS_PROGRESS_CONVERTING_TO_OFFLINE_GAME );
#endif

	while( app.GetXuiServerAction(ProfileManager.GetPrimaryPad() ) != eXuiServerAction_Idle && !MinecraftServer::serverHalted() )
	{
		Sleep(10);
	}
	app.SetXuiServerAction(ProfileManager.GetPrimaryPad(),eXuiServerAction_PauseServer,(void *)TRUE);

	// wait for the server to be in a non-ticking state
	pServer->m_serverPausedEvent->WaitForSignal(INFINITE);
	
#if defined(__PS3__)
	// Swap these two messages around as one is too long to display at 480
	pMinecraft->progressRenderer->progressStartNoAbort( IDS_PROGRESS_CONVERTING_TO_OFFLINE_GAME );
	pMinecraft->progressRenderer->progressStage( -1 ); //g_NetworkManager.CorrectErrorIDS(IDS_CONNECTION_LOST_LIVE_NO_EXIT) );
#else
	pMinecraft->progressRenderer->progressStartNoAbort( g_NetworkManager.CorrectErrorIDS(IDS_CONNECTION_LOST_LIVE_NO_EXIT) );
	pMinecraft->progressRenderer->progressStage( IDS_PROGRESS_CONVERTING_TO_OFFLINE_GAME );
#endif

	pMinecraft->progressRenderer->progressStagePercentage(25);


	// Null the network player of all the server players that are local, to stop them being removed from the server when removed from the session
	if( pServer != NULL )
	{
		PlayerList *players = pServer->getPlayers();
		for(AUTO_VAR(it, players->players.begin()); it < players->players.end(); ++it)
		{
			shared_ptr<ServerPlayer> servPlayer = *it;
			if( servPlayer->connection->isLocal() && !servPlayer->connection->isGuest() )
			{
				servPlayer->connection->connection->getSocket()->setPlayer(NULL);
			}
		}
	}

	// delete the current session - if we weren't actually disconnected fully from the network but have just lost our room, then pass a bLeaveRoom flag of false
	// here as by definition we don't need to leave the room (again). This is currently only an issue for sony platforms.
	if( g_NetworkManager.m_bLastDisconnectWasLostRoomOnly  )
	{
		s_pPlatformNetworkManager->_LeaveGame(false, false);
	}
	else
	{
		s_pPlatformNetworkManager->_LeaveGame(false, true);
	}

	// wait for the current session to end
	while( g_NetworkManager.IsInSession() )
	{
		Sleep(1);
	}
	
	// Reset this flag as the we don't need to know that we only lost the room only from this point onwards, the behaviour is exactly the same
	g_NetworkManager.m_bLastDisconnectWasLostRoomOnly = false;
	g_NetworkManager.m_bFullSessionMessageOnNextSessionChange = false;

	pMinecraft->progressRenderer->progressStagePercentage(50);

	// Defaulting to making this a local game
	g_NetworkManager.SetLocalGame(true);

	// Create a new session with all the players that were in the old one
	int localUsersMask = 0;
	char numLocalPlayers = 0;
	for(unsigned int index = 0; index < XUSER_MAX_COUNT; ++index)
	{
		if(ProfileManager.IsSignedIn(index) && pMinecraft->localplayers[index] != NULL )
		{
			numLocalPlayers++;
			localUsersMask |= GetLocalPlayerMask(index);
		}
	}

	s_pPlatformNetworkManager->_HostGame( localUsersMask );

	pMinecraft->progressRenderer->progressStagePercentage(75);

	// Wait for all the local players to rejoin the session
	while( g_NetworkManager.GetPlayerCount() < numLocalPlayers )
	{
		Sleep(1);
	}
	
	// Restore the network player of all the server players that are local
	if( pServer != NULL )
	{
		for(unsigned int index = 0; index < XUSER_MAX_COUNT; ++index)
		{
			if(ProfileManager.IsSignedIn(index) && pMinecraft->localplayers[index] != NULL )
			{
				PlayerUID localPlayerXuid = pMinecraft->localplayers[index]->getXuid();

				PlayerList *players = pServer->getPlayers();
				for(AUTO_VAR(it, players->players.begin()); it < players->players.end(); ++it)
				{
					shared_ptr<ServerPlayer> servPlayer = *it;
					if( servPlayer->getXuid() == localPlayerXuid )
					{
						servPlayer->connection->connection->getSocket()->setPlayer( g_NetworkManager.GetLocalPlayerByUserIndex(index) );
					}
				}

				// Player might have a pending connection
				if (pMinecraft->m_pendingLocalConnections[index] != NULL)
				{
					// Update the network player
					pMinecraft->m_pendingLocalConnections[index]->getConnection()->getSocket()->setPlayer(g_NetworkManager.GetLocalPlayerByUserIndex(index));
				}
			}
		}
	}
	
	pMinecraft->progressRenderer->progressStagePercentage(100);

	// Make sure that we have transitioned through any joining/creating stages so we're actually ready to set to play
	while(!s_pPlatformNetworkManager->IsReadyToPlayOrIdle())
	{
		Sleep(10);
	}

	s_pPlatformNetworkManager->_StartGame();

	// Wait until the message box has been closed
	while(ui.IsSceneInStack(XUSER_INDEX_ANY, eUIScene_MessageBox))
	{
		Sleep(10);
	}

	// Start the game again
	app.SetGameStarted(true);				
	app.SetXuiServerAction(ProfileManager.GetPrimaryPad(),eXuiServerAction_PauseServer,(void *)FALSE);
	app.SetChangingSessionType(false);
	app.SetReallyChangingSessionType(false);

	return S_OK;

}

void CGameNetworkManager::SystemFlagSet(INetworkPlayer *pNetworkPlayer, int index)
{
	s_pPlatformNetworkManager->SystemFlagSet( pNetworkPlayer, index );
}

bool CGameNetworkManager::SystemFlagGet(INetworkPlayer *pNetworkPlayer, int index)
{
	return s_pPlatformNetworkManager->SystemFlagGet( pNetworkPlayer, index );
}

wstring CGameNetworkManager::GatherStats()
{
	return s_pPlatformNetworkManager->GatherStats();
}

void CGameNetworkManager::renderQueueMeter()
{
}

wstring CGameNetworkManager::GatherRTTStats()
{
	return s_pPlatformNetworkManager->GatherRTTStats();
}

void CGameNetworkManager::StateChange_AnyToHosting()
{
	app.DebugPrintf("Disabling Guest Signin\n");
	XEnableGuestSignin(FALSE);
	Minecraft::GetInstance()->clearPendingClientTextureRequests();
}

void CGameNetworkManager::StateChange_AnyToJoining()
{
	app.DebugPrintf("Disabling Guest Signin\n");
	XEnableGuestSignin(FALSE);
	Minecraft::GetInstance()->clearPendingClientTextureRequests();
		
	ConnectionProgressParams *param = new ConnectionProgressParams();
	param->iPad = ProfileManager.GetPrimaryPad();
	param->stringId = -1;
	param->showTooltips = false;
	param->setFailTimer = true;
	param->timerTime = CONNECTING_PROGRESS_CHECK_TIME;

	ui.NavigateToScene(ProfileManager.GetPrimaryPad(), eUIScene_ConnectingProgress, param);
}

void CGameNetworkManager::StateChange_JoiningToIdle(CPlatformNetworkManager::eJoinFailedReason reason)
{
	DisconnectPacket::eDisconnectReason disconnectReason;
	switch(reason)
	{
		case CPlatformNetworkManager::JOIN_FAILED_SERVER_FULL:
			disconnectReason = DisconnectPacket::eDisconnect_ServerFull;
			break;
		case CPlatformNetworkManager::JOIN_FAILED_INSUFFICIENT_PRIVILEGES:
			disconnectReason = DisconnectPacket::eDisconnect_NoMultiplayerPrivilegesJoin;
			app.SetAction(ProfileManager.GetPrimaryPad(),eAppAction_FailedToJoinNoPrivileges);
			break;
		default:
			disconnectReason = DisconnectPacket::eDisconnect_ConnectionCreationFailed;
			break;
	};
	Minecraft::GetInstance()->connectionDisconnected(ProfileManager.GetPrimaryPad(), disconnectReason);
}

void CGameNetworkManager::StateChange_AnyToStarting()
{
#if defined __PS3__
 	app.getRemoteStorage()->shutdown();			// shut the remote storage lib down and hopefully get our 7mb back
#endif

	if(!g_NetworkManager.IsHost())
	{
		LoadingInputParams *loadingParams = new LoadingInputParams();
		loadingParams->func = &CGameNetworkManager::RunNetworkGameThreadProc;
		loadingParams->lpParam = NULL;

		UIFullscreenProgressCompletionData *completionData = new UIFullscreenProgressCompletionData();
		completionData->bShowBackground=TRUE;
		completionData->bShowLogo=TRUE;
		completionData->type = e_ProgressCompletion_CloseAllPlayersUIScenes;
		completionData->iPad = ProfileManager.GetPrimaryPad();
		loadingParams->completionData = completionData;
			
		ui.NavigateToScene(ProfileManager.GetPrimaryPad(),eUIScene_FullscreenProgress, loadingParams);
	}
}

void CGameNetworkManager::StateChange_AnyToEnding(bool bStateWasPlaying)
{
	// Kick off a stats write for players that are signed into LIVE, if this is a local game
	if( bStateWasPlaying && g_NetworkManager.IsLocalGame() )
	{
		for(unsigned int i = 0; i < XUSER_MAX_COUNT; ++i)
		{
			INetworkPlayer *pNetworkPlayer = g_NetworkManager.GetLocalPlayerByUserIndex(i);
			if(pNetworkPlayer != NULL && ProfileManager.IsSignedIn( i ) )
			{
				app.DebugPrintf("Stats save for an offline game for the player at index %d\n", i );
				Minecraft::GetInstance()->forceStatsSave(pNetworkPlayer->GetUserIndex());
			}
		}
	}

	Minecraft::GetInstance()->gui->clearMessages();

	if(!g_NetworkManager.IsHost() && !g_NetworkManager.IsLeavingGame() )
	{
		// 4J Stu - If the host is saving then it might take a while to quite the session, so do it ourself
		//m_bLeavingGame = true;

		// The host has notified that the game is about to end
		if(app.GetDisconnectReason() == DisconnectPacket::eDisconnect_None) app.SetDisconnectReason( DisconnectPacket::eDisconnect_Quitting );
		app.SetAction(ProfileManager.GetPrimaryPad(),eAppAction_ExitWorld,(void *)TRUE);
	}
}

void CGameNetworkManager::StateChange_AnyToIdle()
{
	app.DebugPrintf("Enabling Guest Signin\n");
	XEnableGuestSignin(TRUE);
	// Reset this here so that we can search for games again
	// 4J Stu - If we are changing session type there is a race between that thread setting the game to local, and this setting it to not local
	if(!app.GetChangingSessionType()) g_NetworkManager.SetLocalGame( false );

}

void CGameNetworkManager::CreateSocket( INetworkPlayer *pNetworkPlayer, bool localPlayer )
{
	Minecraft *pMinecraft = Minecraft::GetInstance();

	Socket *socket = NULL;
	shared_ptr<MultiplayerLocalPlayer> mpPlayer = pMinecraft->localplayers[pNetworkPlayer->GetUserIndex()];
	if( localPlayer && mpPlayer != NULL && mpPlayer->connection != NULL)
	{
		// If we already have a MultiplayerLocalPlayer here then we are doing a session type change
		socket = mpPlayer->connection->getSocket();

		// Pair this socket and network player
		pNetworkPlayer->SetSocket( socket);
		if( socket )
		{
			socket->setPlayer( pNetworkPlayer );
		}
	}
	else
	{
		socket = new Socket( pNetworkPlayer, g_NetworkManager.IsHost(), g_NetworkManager.IsHost() && localPlayer );
		pNetworkPlayer->SetSocket( socket );

		// 4J Stu - May be other states we want to accept aswell
		// Add this user to the game server if the game is started already
		if( g_NetworkManager.IsHost() && g_NetworkManager.IsInGameplay() )
		{
			Socket::addIncomingSocket(socket);
		}

		// If this is a local player and we are already in the game, we need to setup a local connection and log
		// the player in to the game server
		if( localPlayer && g_NetworkManager.IsInGameplay() )
		{
			int idx = pNetworkPlayer->GetUserIndex();
			app.DebugPrintf("Creating new client connection for idx: %d\n", idx);

			ClientConnection *connection;
			connection = new ClientConnection(pMinecraft, socket, idx);

			if( connection->createdOk )
			{
				connection->send( shared_ptr<PreLoginPacket>( new PreLoginPacket( pNetworkPlayer->GetOnlineName() ) ) );
				pMinecraft->addPendingLocalConnection(idx, connection);
			}
			else
			{
				pMinecraft->connectionDisconnected( idx , DisconnectPacket::eDisconnect_ConnectionCreationFailed );
				delete connection;
				connection = NULL;
			}
		}
	}

}

void CGameNetworkManager::CloseConnection( INetworkPlayer *pNetworkPlayer )
{
	MinecraftServer *server = MinecraftServer::getInstance();
	if( server != NULL )
	{
		PlayerList *players = server->getPlayers();
		if( players != NULL )
		{
			players->closePlayerConnectionBySmallId(pNetworkPlayer->GetSmallId());
		}
	}
}

void CGameNetworkManager::PlayerJoining( INetworkPlayer *pNetworkPlayer )
{
	if (g_NetworkManager.IsInGameplay()) // 4J-JEV: Wait to do this at StartNetworkGame if not in-game yet.
	{
		// 4J-JEV: Update RichPresence when a player joins the game.
		bool multiplayer = g_NetworkManager.GetPlayerCount() > 1, localgame = g_NetworkManager.IsLocalGame();
		for (int iPad=0; iPad<XUSER_MAX_COUNT; ++iPad)
		{
			INetworkPlayer *pNetworkPlayer = g_NetworkManager.GetLocalPlayerByUserIndex(iPad);	
			if (pNetworkPlayer == NULL)	continue;

			app.SetRichPresenceContext(iPad,CONTEXT_GAME_STATE_BLANK);
			if (multiplayer)
			{
				if (localgame)	ProfileManager.SetCurrentGameActivity(iPad,	CONTEXT_PRESENCE_MULTIPLAYEROFFLINE,	false);
				else			ProfileManager.SetCurrentGameActivity(iPad,	CONTEXT_PRESENCE_MULTIPLAYER,			false);
			}
			else
			{
				if (localgame)	ProfileManager.SetCurrentGameActivity(iPad,	CONTEXT_PRESENCE_MULTIPLAYER_1POFFLINE,	false);
				else			ProfileManager.SetCurrentGameActivity(iPad,	CONTEXT_PRESENCE_MULTIPLAYER_1P,		false);
			}
		}
	}

    if( pNetworkPlayer->IsLocal() )
    {
		TelemetryManager->RecordPlayerSessionStart(pNetworkPlayer->GetUserIndex());
    }
}

void CGameNetworkManager::PlayerLeaving( INetworkPlayer *pNetworkPlayer )
{
	if( pNetworkPlayer->IsLocal() )
	{
		ProfileManager.SetCurrentGameActivity(pNetworkPlayer->GetUserIndex(),CONTEXT_PRESENCE_IDLE,false);

		TelemetryManager->RecordPlayerSessionExit(pNetworkPlayer->GetUserIndex(), app.GetDisconnectReason());
	}
}

void CGameNetworkManager::HostChanged()
{
	// Disable host migration
	app.SetAction(ProfileManager.GetPrimaryPad(),eAppAction_ExitWorld,(void *)TRUE);
}

void CGameNetworkManager::WriteStats( INetworkPlayer *pNetworkPlayer )
{
	Minecraft::GetInstance()->forceStatsSave( pNetworkPlayer->GetUserIndex() );
}

void CGameNetworkManager::GameInviteReceived( int userIndex, const INVITE_INFO *pInviteInfo)
{



	int localUsersMask = 0;
	Minecraft *pMinecraft = Minecraft::GetInstance();
	int joiningUsers = 0;

	bool noPrivileges = false;
	for(unsigned int index = 0; index < XUSER_MAX_COUNT; ++index)
	{
		if(ProfileManager.IsSignedIn(index) )
		{
			// 4J-PB we shouldn't bring any inactive players into the game, except for the invited player (who may be an inactive player)
			// 4J Stu - If we are not in a game, then bring in all players signed in
			if(index==userIndex || pMinecraft->localplayers[index]!=NULL )
			{	
				++joiningUsers;
				if( !ProfileManager.AllowedToPlayMultiplayer(index) ) noPrivileges = true;	
				localUsersMask |= GetLocalPlayerMask( index );
			}
		}
	}

	// Check if user-created content is allowed, as we cannot play multiplayer if it's not
	bool noUGC = false;
	bool bContentRestricted=false;
	BOOL pccAllowed = TRUE;
	BOOL pccFriendsAllowed = TRUE;
#if defined(__PS3__)
	ProfileManager.GetChatAndContentRestrictions(userIndex,false,&noUGC,&bContentRestricted,NULL);
#else
	ProfileManager.AllowedPlayerCreatedContent(ProfileManager.GetPrimaryPad(),false,&pccAllowed,&pccFriendsAllowed);
	if(!pccAllowed && !pccFriendsAllowed) noUGC = true;
#endif
	
#if defined(__PS3__)
	if(joiningUsers > 1 && !RenderManager.IsHiDef() && userIndex != ProfileManager.GetPrimaryPad())
	{
		UINT uiIDA[1];
		uiIDA[0]=IDS_CONFIRM_OK;

		// 4J-PB - it's possible there is no primary pad here, when accepting an invite from the dashboard
		ui.RequestMessageBox( IDS_CONNECTION_FAILED, IDS_CONNECTION_FAILED_NO_SD_SPLITSCREEN, uiIDA,1,XUSER_INDEX_ANY,NULL,NULL, app.GetStringTable());
	}
	else
#endif

	if( noUGC )
	{
		int messageText = IDS_NO_USER_CREATED_CONTENT_PRIVILEGE_SINGLE_LOCAL;
		if(joiningUsers > 1) messageText = IDS_NO_USER_CREATED_CONTENT_PRIVILEGE_ALL_LOCAL;

		ui.RequestUGCMessageBox(IDS_CONNECTION_FAILED, messageText, XUSER_INDEX_ANY);
	}
#if defined(__PS3__)
	else if(bContentRestricted)
	{
		int messageText = IDS_CONTENT_RESTRICTION;
		if(joiningUsers > 1) messageText = IDS_CONTENT_RESTRICTION_MULTIPLAYER;

		ui.RequestContentRestrictedMessageBox(IDS_CONNECTION_FAILED, messageText, XUSER_INDEX_ANY);
	}
#endif
	else if(noPrivileges)
	{
		UINT uiIDA[1];
		uiIDA[0]=IDS_CONFIRM_OK;

		// 4J-PB - it's possible there is no primary pad here, when accepting an invite from the dashboard
		//StorageManager.RequestMessageBox( IDS_NO_MULTIPLAYER_PRIVILEGE_TITLE, IDS_NO_MULTIPLAYER_PRIVILEGE_JOIN_TEXT, uiIDA,1,ProfileManager.GetPrimaryPad(),NULL,NULL, app.GetStringTable());
		ui.RequestMessageBox( IDS_NO_MULTIPLAYER_PRIVILEGE_TITLE, IDS_NO_MULTIPLAYER_PRIVILEGE_JOIN_TEXT, uiIDA,1,XUSER_INDEX_ANY,NULL,NULL, app.GetStringTable());
	}
	else
	{
		if( !g_NetworkManager.IsInSession() )
		{	
#ifndef __PS3__
			HandleInviteWhenInMenus(userIndex, pInviteInfo);			
#else
			// PS3 is more complicated here - we need to make sure that the player is online. If they are then we can do the same as the xbox, if not we need to try and get them online and then, if they do sign in, go down the same path
			if(ProfileManager.IsSignedInLive(ProfileManager.GetPrimaryPad()))
			{
				HandleInviteWhenInMenus(userIndex, pInviteInfo);
			}
			else
			{
				UINT uiIDA[2];
				uiIDA[0]=IDS_PRO_NOTONLINE_ACCEPT;
				uiIDA[1]=IDS_PRO_NOTONLINE_DECLINE;
				ui.RequestMessageBox(IDS_PRO_NOTONLINE_TITLE, IDS_PRO_NOTONLINE_TEXT, uiIDA, 2, ProfileManager.GetPrimaryPad(),&CGameNetworkManager::MustSignInReturned_1,(void *)pInviteInfo, app.GetStringTable());
			}
#endif
		}
		else
		{
			app.DebugPrintf("We are already in a multiplayer game...need to leave it\n");

// 			JoinFromInviteData *joinData = new JoinFromInviteData();
// 			joinData->dwUserIndex = dwUserIndex;
// 			joinData->dwLocalUsersMask = dwLocalUsersMask;
// 			joinData->pInviteInfo = pInviteInfo;

			// tell the app to process this
			{
				app.ProcessInvite(userIndex,localUsersMask,pInviteInfo);
			}
		}
	}
}

volatile bool waitHere = true;

void CGameNetworkManager::HandleInviteWhenInMenus( int userIndex, const INVITE_INFO *pInviteInfo)
{
	// We are in the root menus somewhere

#if 0
	while( waitHere )
	{
		Sleep(1);
	}
#endif

	// if this is the trial game, then we need the user to unlock the full game
	if(!ProfileManager.IsFullVersion())
	{
		// The marketplace will fail with the primary player set to -1
		ProfileManager.SetPrimaryPad(userIndex); 

		app.SetAction(userIndex,eAppAction_DashboardTrialJoinFromInvite);
	}
	else
	{
		ProfileManager.SetPrimaryPad(userIndex);

		// 4J Stu - If we accept an invite from the main menu before going to play game we need to load the DLC
		// These checks are done within the StartInstallDLCProcess - (!app.DLCInstallProcessCompleted() && !app.DLCInstallPending()) app.StartInstallDLCProcess(dwUserIndex);
		app.StartInstallDLCProcess(userIndex);

		// 4J Stu - Fix for #10936 - MP Lab: TCR 001: Matchmaking: Player is stuck in a soft-locked state after selecting the guest account when prompted
		// The locked profile should not be changed if we are in menus as the main player might sign out in the sign-in ui
		//ProfileManager.SetLockedProfile(-1);

		if(!app.IsLocalMultiplayerAvailable())
		{
			bool noPrivileges=!ProfileManager.AllowedToPlayMultiplayer(userIndex);

			if(noPrivileges)
			{
				UINT uiIDA[1];
				uiIDA[0]=IDS_CONFIRM_OK;
				ui.RequestMessageBox( IDS_NO_MULTIPLAYER_PRIVILEGE_TITLE, IDS_NO_MULTIPLAYER_PRIVILEGE_JOIN_TEXT, uiIDA,1,ProfileManager.GetPrimaryPad(),NULL,NULL, app.GetStringTable());
			}
			else
			{
				ProfileManager.SetLockedProfile(userIndex);
				ProfileManager.SetPrimaryPad(userIndex);

				int localUsersMask=0;
				localUsersMask |= GetLocalPlayerMask( userIndex );

				// If the player was signed in before selecting play, we'll not have read the profile yet, so query the sign-in status to get this to happen
				ProfileManager.QuerySigninStatus();

				// 4J-PB - clear any previous connection errors
				Minecraft::GetInstance()->clearConnectionFailed();						

				g_NetworkManager.SetLocalGame(false);

				// change the minecraft player name
				Minecraft::GetInstance()->user->name = convStringToWstring( ProfileManager.GetGamertag(ProfileManager.GetPrimaryPad()));

				bool success = g_NetworkManager.JoinGameFromInviteInfo( userIndex, localUsersMask, pInviteInfo );
				if( !success )
				{
					app.DebugPrintf( "Failed joining game from invite\n" ); 
				}
			}
		}
		else
		{
			// the FromInvite will make the lib decide how many panes to display based on connected pads/signed in players
			SignInInfo info;
			info.Func = &CGameNetworkManager::JoinFromInvite_SignInReturned;
			info.lpParam = (LPVOID)pInviteInfo;
			info.requireOnline = true;
			app.DebugPrintf("Using fullscreen layer\n");
			ui.NavigateToScene(ProfileManager.GetPrimaryPad(),eUIScene_QuadrantSignin,&info,eUILayer_Alert,eUIGroup_Fullscreen);
		}
	}
}

void CGameNetworkManager::AddLocalPlayerFailed(int idx, bool serverFull/* = false*/)
{
	Minecraft::GetInstance()->connectionDisconnected(idx,  serverFull ? DisconnectPacket::eDisconnect_ServerFull : DisconnectPacket::eDisconnect_ConnectionCreationFailed);
}

#if defined __PS3__
void CGameNetworkManager::HandleDisconnect(bool bLostRoomOnly,bool bPSNSignout)
#else
void CGameNetworkManager::HandleDisconnect(bool bLostRoomOnly)
#endif
{
	int iPrimaryPlayer = g_NetworkManager.GetPrimaryPad();

	if((g_NetworkManager.GetLockedProfile()!=-1) && iPrimaryPlayer!=-1 && g_NetworkManager.IsInSession() )
	{
		m_bLastDisconnectWasLostRoomOnly = bLostRoomOnly;
#if defined __PS3__
		m_bSignedOutofPSN=bPSNSignout;
#endif
		app.SetAction(iPrimaryPlayer,eAppAction_EthernetDisconnected);
	}
	else
	{
		m_bLastDisconnectWasLostRoomOnly = false;
	}
}

int  CGameNetworkManager::GetPrimaryPad()
{
	return ProfileManager.GetPrimaryPad();
}

int  CGameNetworkManager::GetLockedProfile()
{
	return ProfileManager.GetLockedProfile();
}

bool CGameNetworkManager::IsSignedInLive(int playerIdx)
{
	return ProfileManager.IsSignedInLive(playerIdx);
}

bool CGameNetworkManager::AllowedToPlayMultiplayer(int playerIdx)
{
	return ProfileManager.AllowedToPlayMultiplayer(playerIdx);
}

char *CGameNetworkManager::GetOnlineName(int playerIdx)
{
	return ProfileManager.GetGamertag(playerIdx);
}

void CGameNetworkManager::ServerReadyCreate(bool create)
{
	m_hServerReadyEvent = ( create ? ( new C4JThread::Event ) : NULL );
}

void CGameNetworkManager::ServerReady()
{
	m_hServerReadyEvent->Set();
}

void CGameNetworkManager::ServerReadyWait()
{
	m_hServerReadyEvent->WaitForSignal(INFINITE);
}

void CGameNetworkManager::ServerReadyDestroy()
{
	delete m_hServerReadyEvent;
	m_hServerReadyEvent = NULL;
}

bool CGameNetworkManager::ServerReadyValid()
{
	return ( m_hServerReadyEvent != NULL );
}

void CGameNetworkManager::ServerStoppedCreate(bool create)
{
	m_hServerStoppedEvent = ( create ? ( new C4JThread::Event ) : NULL );
}

void CGameNetworkManager::ServerStopped()
{
	m_hServerStoppedEvent->Set();
}

void CGameNetworkManager::ServerStoppedWait()
{
	// If this is called from the main thread, then this won't be ticking anything which can mean that the storage manager state can't progress.
	// This means that the server thread we are waiting on won't ever finish, as it might be locked waiting for this to complete itself.
	// Do some ticking here then if this is the case.
	if( C4JThread::isMainThread() )
	{
		int result = WAIT_TIMEOUT;
		do
		{
			RenderManager.StartFrame();
			result = m_hServerStoppedEvent->WaitForSignal(20);
			// Tick some simple things
			ProfileManager.Tick();
			StorageManager.Tick();
			InputManager.Tick();
			RenderManager.Tick();
			ui.tick();
			ui.render();
			RenderManager.Present();
		} while( result == WAIT_TIMEOUT );
	}
	else
	{
		m_hServerStoppedEvent->WaitForSignal(INFINITE);
	}
}

void CGameNetworkManager::ServerStoppedDestroy()
{
	delete m_hServerStoppedEvent;
	m_hServerStoppedEvent = NULL;
}

bool CGameNetworkManager::ServerStoppedValid()
{
	return ( m_hServerStoppedEvent != NULL );
}

int CGameNetworkManager::GetJoiningReadyPercentage()
{
	return s_pPlatformNetworkManager->GetJoiningReadyPercentage();
}

void CGameNetworkManager::FakeLocalPlayerJoined()
{
	s_pPlatformNetworkManager->FakeLocalPlayerJoined();
}

