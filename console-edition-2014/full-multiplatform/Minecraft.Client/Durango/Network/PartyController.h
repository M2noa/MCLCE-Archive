//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved
#pragma once

#include <ppltasks.h>
class DQRNetworkManager;

class PartyController
{
public:
    PartyController(DQRNetworkManager *pDQRNet);

    void SetPartyView( Windows::Xbox::Multiplayer::PartyView^ partyView );
    Windows::Xbox::Multiplayer::PartyView^ GetPartyView();

    void RefreshPartyView();
    bool AddLocalUsersToParty(int userMask, Windows::Xbox::System::User^ primaryUser);
	void RemoveLocalUsersFromParty(Windows::Xbox::System::User^ primaryUser);
	void RemoveLocalUsersFromParty(Windows::Xbox::System::User^ primaryUser, int playerMask, Microsoft::Xbox::Services::Multiplayer::MultiplayerSessionReference^ sessionReference);
	void RemoveLocalUserFromParty(Windows::Xbox::System::User^ userToRemove);
    void RegisterEventHandlers();
    void UnregisterEventHandlers();
    void UnregisterGamePlayersEventHandler();
    void RegisterGamePlayersChangedEventHandler();
    bool CanJoinParty();
    bool CanInvitePartyToMyGame( Microsoft::Xbox::Services::Multiplayer::MultiplayerSession^ multiplayerSession );
    bool IsPartyInAnotherTitle();
    bool IsGameSessionReadyEventTriggered();
    bool DoesPartySessionExist();
	Microsoft::Xbox::Services::Multiplayer::MultiplayerSessionReference ^ GetGamePartySessionReference();
    void ClearGameSessionReadyEventTriggered();
    int GetActiveAndReservedMemberPartySize();
    bool DoesPartyAndSessionPlayersMatch(
        Microsoft::Xbox::Services::Multiplayer::MultiplayerSession^ session
        );
	void CheckPartySessionFull(Windows::Xbox::System::User^ primaryUser);
	void SetJoinability(bool isJoinable);
	void DisassociateSessionFromParty( Microsoft::Xbox::Services::Multiplayer::MultiplayerSessionReference^ sessionReference);

private:
    Concurrency::critical_section m_lock;
    bool m_isGameSessionReadyEventTriggered;
    bool m_isGamePlayerEventRegistered;
	DQRNetworkManager *m_pDQRNet;

    static void DebugPrintPartyView( Windows::Xbox::Multiplayer::PartyView^ partyView );

    void OnPartyStateChanged( Windows::Xbox::Multiplayer::PartyStateChangedEventArgs^ eventArgs );
    void OnPartyRosterChanged( Windows::Xbox::Multiplayer::PartyRosterChangedEventArgs^ eventArgs );
    void OnGamePlayersChanged( Windows::Xbox::Multiplayer::GamePlayersChangedEventArgs^ eventArgs );
    void OnGameSessionReady( Windows::Xbox::Multiplayer::GameSessionReadyEventArgs^ eventArgs );
    Windows::Xbox::Multiplayer::PartyView^ m_partyView;
    Microsoft::Xbox::Services::Multiplayer::MultiplayerSessionReference^ m_partyGameReadyRef;
    void AddAvailableGamePlayers(
        Windows::Foundation::Collections::IVectorView<Windows::Xbox::Multiplayer::GamePlayer^>^ availablePlayers, 
        int& remainingSlots, 
        Microsoft::Xbox::Services::Multiplayer::MultiplayerSession^ currentSession
        );

	Windows::Foundation::DateTime GetCurrentTime(); 
	double GetTimeBetweenInSeconds(Windows::Foundation::DateTime dt1, Windows::Foundation::DateTime dt2);

    // Party/Session events.
    Windows::Foundation::EventRegistrationToken m_partyRosterChangedToken;
    Windows::Foundation::EventRegistrationToken m_partyStateChangedToken;
    Windows::Foundation::EventRegistrationToken m_partyGamePlayersChangedToken;
    Windows::Foundation::EventRegistrationToken m_partyGameSessionReadyToken;
};


