#pragma once

#if defined(__PS3__)
#include "..\..\Common\Network\Sony\SQRNetworkManager.h"
#endif


// A struct that we store in the QoS data when we are hosting the session. Max size 1020 bytes.
#if defined __PS3__
typedef struct _GameSessionData
{
	unsigned short netVersion;										//   2 bytes
	GameSessionUID hostPlayerUID;										//   8 bytes ( 8*1 ) on xbox, 24 bytes on PS3
	GameSessionUID players[MINECRAFT_NET_MAX_PLAYERS];					//  64 bytes ( 8*8 ) on xbox, 192 ( 24*8) on PS3
	unsigned int m_uiGameHostSettings;								//   4 bytes
	unsigned int texturePackParentId;										//   4 bytes
	unsigned char subTexturePackId;									//   1 byte

	bool isJoinable;												//   1 byte

	unsigned char playerCount;										//   1 byte
	bool isReadyToJoin;												//   1 byte

	_GameSessionData()
	{
		netVersion = 0;
		memset(players,0,MINECRAFT_NET_MAX_PLAYERS*sizeof(players[0]));
		isJoinable = true;
		m_uiGameHostSettings = 0;
		texturePackParentId = 0;
		subTexturePackId = 0;
		playerCount = 0;
		isReadyToJoin = false;

	}
} GameSessionData;
#else
typedef struct _GameSessionData
{
	unsigned short netVersion;										//   2 bytes
	unsigned int m_uiGameHostSettings;								//   4 bytes
	unsigned int texturePackParentId;								//   4 bytes
	unsigned char subTexturePackId;									//   1 byte

	bool isReadyToJoin;												//   1 byte

	_GameSessionData()
	{
		netVersion = 0;
		m_uiGameHostSettings = 0;
		texturePackParentId = 0;
		subTexturePackId = 0;
	}
} GameSessionData;
#endif

class FriendSessionInfo
{
public:
	SessionID sessionId;
#if defined(__PS3__)
	SQRNetworkManager::SessionSearchResult searchResult;
#endif
	wchar_t *displayLabel;
	unsigned char displayLabelLength;
	unsigned char displayLabelViewableStartIndex;
	GameSessionData data;
	bool hasPartyMember;

	FriendSessionInfo()
	{
		displayLabel = NULL;
		displayLabelLength = 0;
		displayLabelViewableStartIndex = 0;
		hasPartyMember = false;
	}

	~FriendSessionInfo()
	{
		if(displayLabel!=NULL)
			delete displayLabel;
	}
};
