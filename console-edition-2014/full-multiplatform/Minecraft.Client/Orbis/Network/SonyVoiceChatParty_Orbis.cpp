#include "stdafx.h"

#include "SonyVoiceChat_Orbis.h"



bool							m_bIsPartyAPIInitialized;
bool							m_bIsPartyBinaryMessageAPIReady;

SceNpPartyState					m_stPartyState;

 SonyVoiceChatParty_Orbis::PartyInfo SonyVoiceChatParty_Orbis::m_partyInfo;

void SonyVoiceChatParty_Orbis::init()
{
// 	m_bIsRunning			= false;
// 	m_pRenderer				= NULL;
// 	m_pPadContext			= NULL;
// 	m_pGraphicsContext		= NULL;
// 	m_iPartyMemberCount		= 0;
// 	m_iPartyLeader			= 0;
	int ret					= SCE_OK;
	SceNpPartyInitializeParam stPartyInit;
	SceUserServiceLoginUserIdList userIdList;

	
	// Run PartyInitializeParamInitialize inline 
	sceNpPartyInitializeParamInitialize( &stPartyInit );

	// Initialize the Party API
	ret = sceNpPartyInitialize( &stPartyInit );
	if( ret != SCE_OK ) 
	{
		app.DebugPrintf( "Error: sceNpPartyInitialize failed result:0x%x\n", ret );
		return;
	}

	// Register handlers for party room events
	SceNpPartyEventHandlers stPartyEventHandler;
	memset( &stPartyEventHandler, 0, sizeof( SceNpPartyEventHandlers ) );
	stPartyEventHandler.roomEventHandler			= partyRoomEventHandler;
	stPartyEventHandler.voiceEventHandler			= partyVoiceEventHandler;
	stPartyEventHandler.binaryMessageEventHandler	= partyBinaryMessageEventHandler;
	ret = sceNpPartyRegisterHandler( &stPartyEventHandler, NULL );
	if( ret != SCE_OK ) 
	{
		app.DebugPrintf( "Error: sceNpPartyRegisterHandler failed result:0x%x\n", ret );
		return;
	}


	// Get current party state
	ret = sceNpPartyGetState( &m_stPartyState );
	if( ret != SCE_OK )
	{
		app.DebugPrintf( "Error: sceNpPartyGetState failed result:0x%x\n", ret );
		return;
	} 

	m_bIsPartyAPIInitialized = true;
}

void SonyVoiceChatParty_Orbis::shutdown()
{
}

void SonyVoiceChatParty_Orbis::setEnabled( bool bEnabled )
{
}




void SonyVoiceChatParty_Orbis::tick()
{
	sceNpPartyCheckCallback();
}



bool SonyVoiceChatParty_Orbis::hasMicConnected(const PlayerUID& memberUID)
{
	MemberInfo* pInfo = m_partyInfo.getMember(memberUID);
	if(pInfo)
	{
		 // in the party, might not have a mic though, still need to check for this
		return true; 
	}
	return false;
}

void SonyVoiceChatParty_Orbis::mute( bool bMute )
{
// 	if(sm_bLoaded && !sm_bUnloading)
// 	{
// 		int err = cellSysutilAvc2SetVoiceMuting(bMute); 
// 		assert(err == CELL_OK);
// 	}
}

void SonyVoiceChatParty_Orbis::mutePlayer( const SceNpMatching2RoomMemberId member_id, bool bMute ) /*Turn chat audio from a specified player on or off */
{
// 	if(sm_bLoaded && !sm_bUnloading)
// 	{
// 		int err = cellSysutilAvc2SetPlayerVoiceMuting(member_id, bMute);
// 		assert(err == CELL_OK);
// 	}
}

void SonyVoiceChatParty_Orbis::muteLocalPlayer( bool bMute ) /*Turn microphone input on or off */
{
// 	if(sm_bLoaded && !sm_bUnloading)
// 	{
// 		int err = cellSysutilAvc2SetVoiceMuting(bMute);
// 		assert(err == CELL_OK);
// 	}
}


bool SonyVoiceChatParty_Orbis::isMuted()
{
// 	if(sm_bLoaded && !sm_bUnloading)
// 	{
// 		uint8_t bMute;
// 		int err = cellSysutilAvc2GetVoiceMuting(&bMute); 
// 		assert(err == CELL_OK);
// 		return bMute;
// 	}
// 	return false;
}

bool SonyVoiceChatParty_Orbis::isMutedPlayer( const PlayerUID& memberUID) 
{
	MemberInfo* pInfo = m_partyInfo.getMember(memberUID);
	if(pInfo)
		return pInfo->m_voiceMuted;
	assert(0 && "Didn't find playerUID"); // 
	return false;
// 	if(sm_bLoaded && !sm_bUnloading)
// 	{
// 		uint8_t bMute;
// 		int err = cellSysutilAvc2GetPlayerVoiceMuting(member_id, &bMute);
// 		assert(err == CELL_OK);
// 		return bMute;
// 	}
// 	return false;
}

bool SonyVoiceChatParty_Orbis::isMutedLocalPlayer() 
{
// 	if(sm_bLoaded && !sm_bUnloading)
// 	{
// 		uint8_t bMute;
// 		int err = cellSysutilAvc2GetVoiceMuting(&bMute);
// 		assert(err == CELL_OK);
// 		return bMute;
// 	}
// 	return false;
}


bool SonyVoiceChatParty_Orbis::isTalking( const PlayerUID& memberUID)
{
	MemberInfo* pInfo = m_partyInfo.getMember(memberUID);
	if(pInfo)
	{
		DWORD currTime = GetTickCount();
		DWORD timeElapsed = currTime - pInfo->m_lastTimeTalking;
		return (timeElapsed < 1000);
	}
	assert(0 && "Didn't find playerUID"); // 
	return false;
}




void SonyVoiceChatParty_Orbis::partyRoomEventHandler(SceNpPartyRoomEventType eventType, const void* data, void* userdata)
{

	switch(eventType)
	{
	case SCE_NP_PARTY_ROOM_EVENT_JOINED:
	{
		// local player joined party
		app.DebugPrintf("SCE_NP_PARTY_ROOM_EVENT_JOINED\n");
		SceNpPartyJoinedInfo* pPartyJoinedInfo = (SceNpPartyJoinedInfo*) data;

		m_partyInfo.clear();
		app.DebugPrintf("Local user joined party.\n");
		for(int i=0; i<pPartyJoinedInfo->memberNum;i++)
		{
			m_partyInfo.addMember(pPartyJoinedInfo->memberIds[i], pPartyJoinedInfo->members[i], false, false);
			app.DebugPrintf("Member room ID : %d - Member PSN ID : %s\n", pPartyJoinedInfo->memberIds[i], pPartyJoinedInfo->members[i].handle.data);
		}
	}
	break;

	case SCE_NP_PARTY_ROOM_EVENT_MEMBER_JOINED:
	{
		// remote player joined party
		app.DebugPrintf("SCE_NP_PARTY_ROOM_EVENT_MEMBER_JOINED\n");
		SceNpPartyMemberInfo *pNewPartyMemberInfo = (SceNpPartyMemberInfo*) data;
		app.DebugPrintf("A remote player[%s] has a joined the party.\n",pNewPartyMemberInfo->npId.handle.data);

		m_partyInfo.addMember(pNewPartyMemberInfo->memberId, pNewPartyMemberInfo->npId, pNewPartyMemberInfo->memberFlags & SCE_NP_PARTY_MEMBER_FLAG_IS_LOCAL, pNewPartyMemberInfo->memberFlags & SCE_NP_PARTY_MEMBER_FLAG_IS_PARTY_LEADER);
	}
	break;

	case SCE_NP_PARTY_ROOM_EVENT_MEMBER_LEFT:
	{
		// remote player left party
		app.DebugPrintf("SCE_NP_PARTY_ROOM_EVENT_MEMBER_LEFT\n");
		SceNpPartyMemberInfo *pLeavingPartyMemberInfo = (SceNpPartyMemberInfo*) data;
		app.DebugPrintf("A remote player[%s] has left the party.\n", pLeavingPartyMemberInfo->npId.handle.data);
		m_partyInfo.removeMember(pLeavingPartyMemberInfo->memberId);
	}
	break;

	case SCE_NP_PARTY_ROOM_EVENT_LEFT:
	{
		// local player left party
		app.DebugPrintf("SCE_NP_PARTY_ROOM_EVENT_LEFT\n");
		SceNpPartyRoomLeftInfo *pLeftInfo = (SceNpPartyRoomLeftInfo*)data;
		printf("Local party member[%s] id:%d has left the party. Reason[%d]\n", pLeftInfo->npId.handle.data, pLeftInfo->memberId, pLeftInfo->reason);
		m_partyInfo.removeMember(pLeftInfo->memberId);
	}
	break;

	case SCE_NP_PARTY_ROOM_EVENT_CREATE_RESPONSE:
	{
		app.DebugPrintf("SCE_NP_PARTY_ROOM_EVENT_CREATE_RESPONSE\n");
		SceNpPartyCreateResponseInfo *pCreateResponseInfo = (SceNpPartyCreateResponseInfo*) data;

		switch( pCreateResponseInfo->status )
		{
		case SCE_NP_PARTY_ROOM_CREATE_RESPONSE_STATUS_OK: 
			app.DebugPrintf(	"Local party member userId[0x%x] response for creating a party. Status = SCE_NP_PARTY_ROOM_CREATE_RESPONSE_STATUS_OK\n" , pCreateResponseInfo->userId );
			break;
		case SCE_NP_PARTY_ROOM_CREATE_RESPONSE_STATUS_CANCELLED: 
			app.DebugPrintf(	"Local party member userId[0x%x] response for creating a party. Status = SCE_NP_PARTY_ROOM_CREATE_RESPONSE_STATUS_CANCELLED\n" , pCreateResponseInfo->userId );
			break;
		default:
			app.DebugPrintf( "Warning: Unknown SceNpPartyCreateResponseStatus [%d]. Ignore.\n", pCreateResponseInfo->status );
			break;
		}
	}
	break;

	case SCE_NP_PARTY_ROOM_EVENT_SHOW_INVITATION_RESPONSE:
	{
		app.DebugPrintf("SCE_NP_PARTY_ROOM_EVENT_SHOW_INVITATION_RESPONSE\n");
		SceNpPartyShowInvitationResponseInfo *pShowInvitationResponseInfo = (SceNpPartyShowInvitationResponseInfo*) data;

		switch( pShowInvitationResponseInfo->status )
		{
		case SCE_NP_PARTY_ROOM_SHOW_INVITATION_RESPONSE_STATUS_OK: 
			app.DebugPrintf(	"Local party member userId[0x%x] response for sending an invitation/suggestion. Status = SCE_NP_PARTY_ROOM_SHOW_INVITATION_RESPONSE_STATUS_OK\n" , pShowInvitationResponseInfo->userId );
			break;
		case SCE_NP_PARTY_ROOM_SHOW_INVITATION_RESPONSE_STATUS_CANCELLED: 
			app.DebugPrintf(	"Local party member userId[0x%x] response for sending an invitation/suggestion. Status = SCE_NP_PARTY_ROOM_SHOW_INVITATION_RESPONSE_STATUS_CANCELLED\n" , pShowInvitationResponseInfo->userId );
			break;
		default:
			app.DebugPrintf( "Warning: Unknown SceNpPartyShowInvitationResponseStatus [%d]. Ignore.\n", pShowInvitationResponseInfo->status );
			break;
		}
	}
	break;

	default:
	{
		app.DebugPrintf( "Warning: Un-handled party event[%d]. Ignore.\n", eventType );
	}
	break;

	}

}



void SonyVoiceChatParty_Orbis::partyVoiceEventHandler( const SceNpPartyMemberVoiceInfo* memberVoiceInfo, void* userdata )
{

	switch(memberVoiceInfo->memberVoiceState)
	{
	case SCE_NP_PARTY_MEMBER_VOICE_STATE_DISCONNECTED:
	{
		app.DebugPrintf("SCE_NP_PARTY_MEMBER_VOICE_STATE_DISCONNECTED\n");
		MemberInfo* pInfo = m_partyInfo.getMember(memberVoiceInfo->memberId);
		assert(pInfo);
		if(pInfo)
			pInfo->m_voiceConnected = false;
	}
	break;
	case SCE_NP_PARTY_MEMBER_VOICE_STATE_CONNECTED:
	{
		app.DebugPrintf("SCE_NP_PARTY_MEMBER_VOICE_STATE_CONNECTED\n");
		MemberInfo* pInfo = m_partyInfo.getMember(memberVoiceInfo->memberId);
		assert(pInfo);
		if(pInfo)
			pInfo->m_voiceConnected = true;
	}
	break;
	case SCE_NP_PARTY_MEMBER_VOICE_STATE_TALKING:
	{
		app.DebugPrintf("SCE_NP_PARTY_MEMBER_VOICE_STATE_TALKING\n");
		MemberInfo* pInfo = m_partyInfo.getMember(memberVoiceInfo->memberId);
		assert(pInfo);
		if(pInfo)
		{
			pInfo->m_voiceMuted = false;
			pInfo->m_lastTimeTalking = GetTickCount();
		}
	}
	break;
	case SCE_NP_PARTY_MEMBER_VOICE_STATE_MUTED:
	{
		app.DebugPrintf("SCE_NP_PARTY_MEMBER_VOICE_STATE_MUTED\n");
		MemberInfo* pInfo = m_partyInfo.getMember(memberVoiceInfo->memberId);
		assert(pInfo);
		if(pInfo)
			pInfo->m_voiceMuted = true;
	}
	break;
	case SCE_NP_PARTY_MEMBER_VOICE_STATE_UNKNOWN:
	{
		app.DebugPrintf("SCE_NP_PARTY_MEMBER_VOICE_STATE_UNKNOWN\n");
	}
		break;
	default:
	{
		app.DebugPrintf("Warning: Un-handled voice event. Ignore.\n");
	}
	break;
	}
}


void SonyVoiceChatParty_Orbis::partyBinaryMessageEventHandler(	SceNpPartyBinaryMessageEvent   event,
											   const void *data,
											   void * userdata
											   )
{
	switch(event)
	{
	case SCE_NP_PARTY_BINARY_MESSAGE_EVENT_READY:
	case SCE_NP_PARTY_BINARY_MESSAGE_EVENT_DATA:
	case SCE_NP_PARTY_BINARY_MESSAGE_EVENT_COMPATIBILITY:
	default:
// 		app.DebugPrintf("partyBinaryMessageEventHandler not supported");
// 		assert(0);
		break;
	}
}
