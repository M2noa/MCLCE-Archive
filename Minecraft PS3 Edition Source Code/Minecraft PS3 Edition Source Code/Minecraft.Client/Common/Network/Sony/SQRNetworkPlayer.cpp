#include "stdafx.h"
#include "SQRNetworkPlayer.h"

#if defined(__PS3__)
#include <cell/rudp.h>
#include "PS3/Network/SonyVoiceChat.h"

#else
#include <rudp.h>
#include <adhoc_matching.h>
#include "PSVita/Network/SonyVoiceChat_Vita.h"

#endif


static const bool sc_verbose = false;

int	SQRNetworkPlayer::GetSmallId()
{
	return m_ISD.m_smallId;
}

wchar_t *SQRNetworkPlayer::GetName()
{
	return m_name;
}

bool SQRNetworkPlayer::IsRemote()
{
	return !IsLocal();
}

bool SQRNetworkPlayer::IsHost()
{
	return (m_type == SNP_TYPE_HOST);
}

bool SQRNetworkPlayer::IsLocal()
{
	// m_host determines whether this *machine* is hosting the game, not this player (which is determined by m_type)
	if( m_host )
	{
		// If we are the hosting machine, then both the host & local players are local to this machine
		return (m_type == SNP_TYPE_HOST) || (m_type == SNP_TYPE_LOCAL);
	}
	else
	{
		// Not hosting, just local players are actually physically local
		return (m_type == SNP_TYPE_LOCAL) ;
	}
}

int SQRNetworkPlayer::GetLocalPlayerIndex()
{
	return m_localPlayerIdx;
}

bool SQRNetworkPlayer::IsSameSystem(SQRNetworkPlayer *other)
{
	return (m_roomMemberId == other->m_roomMemberId);
}

uintptr_t	SQRNetworkPlayer::GetCustomDataValue()
{
	return m_customData;
}

void	SQRNetworkPlayer::SetCustomDataValue(uintptr_t data)
{
	m_customData = data;
}

SQRNetworkPlayer::SQRNetworkPlayer(SQRNetworkManager *manager, eSQRNetworkPlayerType playerType, bool onHost, SceNpMatching2RoomMemberId roomMemberId, int localPlayerIdx, int rudpCtx, PlayerUID *pUID)
{
	m_roomMemberId		= roomMemberId;
	m_localPlayerIdx	= localPlayerIdx;
	m_rudpCtx			= rudpCtx;
	m_flags				= 0;
	m_type				= playerType;
	m_host				= onHost;
	m_manager			= manager;
	m_customData		= 0;
	if( pUID )
	{
		memcpy(&m_ISD.m_UID,pUID,sizeof(PlayerUID));
	}
	else
	{
		memset(&m_ISD.m_UID,0,sizeof(PlayerUID));
	}
	SetNameFromUID();
	InitializeCriticalSection(&m_csQueue);

}

SQRNetworkPlayer::~SQRNetworkPlayer()
{
	DeleteCriticalSection(&m_csQueue);
}

bool SQRNetworkPlayer::IsReady()
{
	return ( ( m_flags & SNP_FLAG_READY_MASK ) == SNP_FLAG_READY_MASK );
}
	
PlayerUID SQRNetworkPlayer::GetUID()
{
	return m_ISD.m_UID;
}

void SQRNetworkPlayer::SetUID(PlayerUID UID)
{
	m_ISD.m_UID = UID;
	SetNameFromUID();
}

bool SQRNetworkPlayer::HasConnectionAndSmallId()
{
	const int reqFlags = ( SNP_FLAG_CONNECTION_COMPLETE | SNP_FLAG_SMALLID_ALLOCATED );
	return (( m_flags & reqFlags) == reqFlags);
}

void SQRNetworkPlayer::ConnectionComplete()
{
	m_host ? app.DebugPrintf(sc_verbose, "host : ") : app.DebugPrintf(sc_verbose, "client:");
	app.DebugPrintf(sc_verbose, ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>    ConnectionComplete\n");
	m_flags |= SNP_FLAG_CONNECTION_COMPLETE;
}

void SQRNetworkPlayer::SmallIdAllocated(unsigned char smallId)
{
	m_ISD.m_smallId = smallId;
	m_flags |= SNP_FLAG_SMALLID_ALLOCATED;
	m_host ? app.DebugPrintf(sc_verbose, "host : ") : app.DebugPrintf(sc_verbose, "client:");
	app.DebugPrintf(sc_verbose, ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>    Small ID allocated\n");


	// If this is a non-network sort of player then flag now as having its small id confirmed
	if( ( m_type == SNP_TYPE_HOST ) ||
		( m_host &&  ( m_type == SNP_TYPE_LOCAL ) ) ||
		( !m_host && ( m_type == SNP_TYPE_REMOTE ) ) )
	{
		m_host ? app.DebugPrintf(sc_verbose, "host : ") : app.DebugPrintf(sc_verbose, "client:");
		app.DebugPrintf(sc_verbose, ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>    Small ID confirmed\n");

		m_flags |= SNP_FLAG_SMALLID_CONFIRMED;
	}
}

void SQRNetworkPlayer::InitialDataReceived(SQRNetworkPlayer::InitSendData *ISD)
{
	assert(m_ISD.m_smallId == ISD->m_smallId);
	memcpy(&m_ISD, ISD, sizeof(InitSendData) );
	m_host ? app.DebugPrintf(sc_verbose, "host : ") : app.DebugPrintf(sc_verbose, "client:");
	app.DebugPrintf(sc_verbose, ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>    Small ID confirmed\n");
	m_flags |= SNP_FLAG_SMALLID_CONFIRMED;
}

bool SQRNetworkPlayer::HasSmallIdConfirmed()
{
	return ( m_flags & SNP_FLAG_SMALLID_CONFIRMED );
}

// To confirm to the host that we are ready, send a single byte with our small id.
void SQRNetworkPlayer::ConfirmReady()
{
#ifdef __PS3__
	int ret = cellRudpWrite( m_rudpCtx, &m_ISD, sizeof(InitSendData), CELL_RUDP_MSG_LATENCY_CRITICAL );
#else //__ORBIS__
	int ret = sceRudpWrite( m_rudpCtx, &m_ISD, sizeof(InitSendData), SCE_RUDP_MSG_LATENCY_CRITICAL );
#endif
	// TODO - error handling here?
	assert ( ret == sizeof(InitSendData) );
	// Final flag for a local player on the client, as we are now safe to send data on to the host
	m_host ? app.DebugPrintf(sc_verbose, "host : ") : app.DebugPrintf(sc_verbose, "client:");
	app.DebugPrintf(sc_verbose, ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>    Small ID confirmed\n");
	m_flags |= SNP_FLAG_SMALLID_CONFIRMED;
}

// Attempt to send data, of any size, from this player to that specified by pPlayerTarget. This may not be possible depending on the two players, due to
// our star shaped network connectivity. Data may be any size, and is copied so on returning from this method it does not need to be preserved.
void SQRNetworkPlayer::SendData( SQRNetworkPlayer *pPlayerTarget, const void *data, unsigned int dataSize )
{
	// Our network is connected as a star. If we are the host, then we can send to any remote player. If we're a client, we can send only to the host.
	// The host can also send to other local players, but this doesn't need to go through Rudp. 
	if( m_host )
	{
		if( ( m_type == SNP_TYPE_HOST ) && ( pPlayerTarget->m_type == SNP_TYPE_LOCAL ) )
		{
			// Special internal communication from host to local player
			m_manager->LocalDataSend( this, pPlayerTarget, data, dataSize );
		}
		else if( ( m_type == SNP_TYPE_LOCAL ) && ( pPlayerTarget->m_type == SNP_TYPE_HOST ) )
		{
			// Special internal communication from local player to host
			m_manager->LocalDataSend( this, pPlayerTarget, data, dataSize );
		}
		else if( ( m_type == SNP_TYPE_HOST ) && ( pPlayerTarget->m_type == SNP_TYPE_REMOTE ) )
		{
			// Rudp communication from host to remote player - handled by remote player instance
			pPlayerTarget->SendInternal(data,dataSize);
		}
		else
		{
			// Can't do any other types of communications
			assert(false);
		}
	}
	else
	{
		if( ( m_type == SNP_TYPE_LOCAL ) && ( pPlayerTarget->m_type == SNP_TYPE_HOST ) )
		{
			// Rudp communication from client to host - handled by this player instace
			SendInternal(data, dataSize);
		}
		else
		{
			// Can't do any other types of communications
			assert(false);
		}
	}
}

// Internal send function - to simplify the number of mechanisms we have for sending data, this method just adds the data to be send to the player's internal queue,
// and then calls SendMoreInternal. This method can take any size of data, which it will split up into payload size chunks before sending. All input data is copied
// into internal buffers.
void SQRNetworkPlayer::SendInternal(const void *data, unsigned int dataSize)
{
	EnterCriticalSection(&m_csQueue);

	QueuedSendBlock sendBlock;

	unsigned char *dataCurrent = (unsigned char *)data;
	unsigned int dataRemaining = dataSize;

	while( dataRemaining )
	{
		int dataSize = dataRemaining;
		if( dataSize > SNP_MAX_PAYLOAD ) dataSize = SNP_MAX_PAYLOAD;
		sendBlock.start = new unsigned char [dataSize];
		sendBlock.end = sendBlock.start + dataSize;
		sendBlock.current = sendBlock.start;
		memcpy( sendBlock.start, dataCurrent, dataSize);
		m_sendQueue.push(sendBlock);		
		dataRemaining -= dataSize;
		dataCurrent += dataSize;
	}

	// Now try and send as much as we can
	SendMoreInternal();

	LeaveCriticalSection(&m_csQueue);
}




// Internal send function. This attempts to send as many elements in the queue as possible until the write function tells us that we can't send any more. This way,
// we are guaranteed that if there *is* anything more in the queue left to send, we'll get a CELL_RUDP_CONTEXT_EVENT_WRITABLE event when whatever we've managed to
// send here is complete, and can continue on.
void SQRNetworkPlayer::SendMoreInternal()
{
	EnterCriticalSection(&m_csQueue);
	bool keepSending;
	do
	{
		keepSending = false;
		if( m_sendQueue.size() > 0)
		{
			// Attempt to send the full data in the first element in our queue
			unsigned char *data= m_sendQueue.front().current;
			int dataSize = m_sendQueue.front().end - m_sendQueue.front().current;
#ifdef __PS3__
			int ret = cellRudpWrite( m_rudpCtx, data, dataSize, 0);//CELL_RUDP_MSG_LATENCY_CRITICAL  );
			int wouldBlockFlag =  CELL_RUDP_ERROR_WOULDBLOCK;

#else // __ORBIS__
			int ret = sceRudpWrite( m_rudpCtx, data, dataSize, 0);//CELL_RUDP_MSG_LATENCY_CRITICAL  );
			int wouldBlockFlag =  SCE_RUDP_ERROR_WOULDBLOCK;
#endif
			if( ret == dataSize )
			{
				// Fully sent, remove from queue - will loop in the while loop to see if there's anything else in the queue we could send
				delete [] m_sendQueue.front().start;
				m_sendQueue.pop();
				if( m_sendQueue.size() )
				{
					keepSending = true;
				}
			} 
			else if( ( ret >= 0 ) || ( ret == wouldBlockFlag ) )
			{
				

				// Things left to send - adjust this element in the queue
				int	remainingBytes;
				if( ret >= 0 )
				{
					// Only ret bytes sent so far
					remainingBytes = dataSize - ret;
					assert(remainingBytes > 0 );
				}
				else
				{
					// Is CELL_RUDP_ERROR_WOULDBLOCK, nothing has yet been sent
					remainingBytes = dataSize;
				}
				m_sendQueue.front().current = m_sendQueue.front().end - remainingBytes;	
			}
		}
	} while (keepSending);
	LeaveCriticalSection(&m_csQueue);
}

void SQRNetworkPlayer::SetNameFromUID()
{
	mbstowcs(m_name, m_ISD.m_UID.getOnlineID(), 16);
	m_name[16] = 0;
#ifdef __PS3__ // only 1 player on vita, and they have to be online (or adhoc), and with PS4 all local players need to be signed in
	// Not an online player? Add a suffix with the controller ID on
	if( m_ISD.m_UID.isSignedIntoPSN() == 0)
	{
		int pos = wcslen(m_name);
		swprintf(&m_name[pos], 5, L" (%d)", m_ISD.m_UID.getQuadrant() + 1 );
	}
#endif
}

void SQRNetworkPlayer::SetName(char *name)
{
	mbstowcs(m_name, name, 20);
	m_name[20] = 0;
}

int SQRNetworkPlayer::GetSessionIndex()
{
	return m_manager->GetSessionIndex(this);
}

bool SQRNetworkPlayer::HasVoice()
{
	return SonyVoiceChat::hasMicConnected(&m_roomMemberId);
}

bool SQRNetworkPlayer::IsTalking()
{
	return SonyVoiceChat::isTalking(&m_roomMemberId);
}

bool SQRNetworkPlayer::IsMutedByLocalUser(int userIndex)
{
	SQRNetworkManager_PS3* pMan = (SQRNetworkManager_PS3*)m_manager;
	return SonyVoiceChat::isMutedPlayer(pMan->m_roomSyncData.players[userIndex].m_roomMemberId);
}
