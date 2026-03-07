#include "stdafx.h"
#ifdef _WINDOWS64

#include "LANSessionManager.h"
#include "..\..\..\Minecraft.World\System.h"
#include "..\..\..\Minecraft.World\StringHelpers.h"
#include <cstdlib>
#include <ctime>

CLANSessionManager::CLANSessionManager()
{
	m_broadcastSocket = INVALID_SOCKET;
	m_listenSocket = INVALID_SOCKET;
	m_initialized = false;
	m_broadcasting = false;
	m_listening = false;
	m_sessionsChanged = false;
	m_lastBroadcastTime = 0;
	m_localSessionId = 0;
	memset(&m_broadcastPacket, 0, sizeof(m_broadcastPacket));
}

CLANSessionManager::~CLANSessionManager()
{
	Shutdown();
}

bool CLANSessionManager::Initialize()
{
	if (m_initialized)
		return true;

	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	InitializeCriticalSection(&m_sessionsLock);

	srand((unsigned int)time(NULL) ^ GetCurrentProcessId());

	m_initialized = true;
	app.DebugPrintf("LAN: Session manager initialized\n");
	return true;
}

void CLANSessionManager::Shutdown()
{
	if (!m_initialized)
		return;

	StopBroadcasting();
	StopListening();

	// Clean up discovered sessions
	EnterCriticalSection(&m_sessionsLock);
	for (size_t i = 0; i < m_discoveredSessions.size(); i++)
	{
		delete m_discoveredSessions[i].info;
	}
	m_discoveredSessions.clear();
	LeaveCriticalSection(&m_sessionsLock);

	DeleteCriticalSection(&m_sessionsLock);
	m_initialized = false;
}

void CLANSessionManager::StartBroadcasting(const GameSessionData& sessionData, const char* hostName, const char* worldName)
{
	if (!m_initialized)
		return;

	// Create broadcast socket
	m_broadcastSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (m_broadcastSocket == INVALID_SOCKET)
	{
		app.DebugPrintf("LAN: Failed to create broadcast socket: %d\n", WSAGetLastError());
		return;
	}

	BOOL broadcastEnable = TRUE;
	setsockopt(m_broadcastSocket, SOL_SOCKET, SO_BROADCAST, (const char*)&broadcastEnable, sizeof(broadcastEnable));

	BOOL reuseAddr = TRUE;
	setsockopt(m_broadcastSocket, SOL_SOCKET, SO_REUSEADDR, (const char*)&reuseAddr, sizeof(reuseAddr));

	m_localSessionId = GenerateSessionId();

	memset(&m_broadcastPacket, 0, sizeof(m_broadcastPacket));
	m_broadcastPacket.magic = LAN_BROADCAST_MAGIC;
	m_broadcastPacket.netVersion = sessionData.netVersion;
	m_broadcastPacket.gameHostSettings = sessionData.m_uiGameHostSettings;
	m_broadcastPacket.texturePackParentId = sessionData.texturePackParentId;
	m_broadcastPacket.subTexturePackId = sessionData.subTexturePackId;
	m_broadcastPacket.playerCount = 1;  // Just the host for now
	m_broadcastPacket.maxPlayers = MINECRAFT_NET_MAX_PLAYERS;
	m_broadcastPacket.isJoinable = 1;
	m_broadcastPacket.gamePort = 0;  // Reserved for future TCP connection
	m_broadcastPacket.hostIP = 0;    // Receiver fills this from recvfrom
	m_broadcastPacket.sessionId = m_localSessionId;

	if (hostName != NULL)
		strncpy(m_broadcastPacket.hostName, hostName, LAN_MAX_HOST_NAME - 1);
	else
		strncpy(m_broadcastPacket.hostName, "Player", LAN_MAX_HOST_NAME - 1);

	if (worldName != NULL)
		strncpy(m_broadcastPacket.worldName, worldName, LAN_MAX_WORLD_NAME - 1);
	else
		strncpy(m_broadcastPacket.worldName, "Minecraft World", LAN_MAX_WORLD_NAME - 1);

	m_broadcasting = true;
	m_lastBroadcastTime = 0;  // Broadcast immediately on first tick

	app.DebugPrintf("LAN: Started broadcasting session '%s' (host: %s, sessionId: %llu)\n",
		m_broadcastPacket.worldName, m_broadcastPacket.hostName, m_localSessionId);
}

void CLANSessionManager::StopBroadcasting()
{
	if (m_broadcastSocket != INVALID_SOCKET)
	{
		closesocket(m_broadcastSocket);
		m_broadcastSocket = INVALID_SOCKET;
	}
	m_broadcasting = false;
}

void CLANSessionManager::SetPlayerCount(unsigned char count)
{
	m_broadcastPacket.playerCount = count;
}

void CLANSessionManager::StartListening()
{
	if (!m_initialized || m_listening)
		return;

	// Create listen socket
	m_listenSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (m_listenSocket == INVALID_SOCKET)
	{
		app.DebugPrintf("LAN: Failed to create listen socket: %d\n", WSAGetLastError());
		return;
	}

	BOOL reuseAddr = TRUE;
	setsockopt(m_listenSocket, SOL_SOCKET, SO_REUSEADDR, (const char*)&reuseAddr, sizeof(reuseAddr));

	// Bind to the broadcast port
	sockaddr_in listenAddr;
	memset(&listenAddr, 0, sizeof(listenAddr));
	listenAddr.sin_family = AF_INET;
	listenAddr.sin_addr.s_addr = INADDR_ANY;
	listenAddr.sin_port = htons(LAN_BROADCAST_PORT);

	if (::bind(m_listenSocket, (sockaddr*)&listenAddr, sizeof(listenAddr)) == SOCKET_ERROR)
	{
		app.DebugPrintf("LAN: Failed to bind listen socket to port %d: %d\n", LAN_BROADCAST_PORT, WSAGetLastError());
		closesocket(m_listenSocket);
		m_listenSocket = INVALID_SOCKET;
		return;
	}

	// Set non-blocking
	u_long nonBlocking = 1;
	ioctlsocket(m_listenSocket, FIONBIO, &nonBlocking);

	m_listening = true;
	app.DebugPrintf("LAN: Started listening for sessions on port %d\n", LAN_BROADCAST_PORT);
}

void CLANSessionManager::StopListening()
{
	if (m_listenSocket != INVALID_SOCKET)
	{
		closesocket(m_listenSocket);
		m_listenSocket = INVALID_SOCKET;
	}
	m_listening = false;
}

void CLANSessionManager::Tick()
{
	if (!m_initialized)
		return;

	if (m_broadcasting)
		TickBroadcast();

	if (m_listening)
	{
		TickListen();
		CleanExpiredSessions();
	}
}

void CLANSessionManager::TickBroadcast()
{
	__int64 now = System::currentTimeMillis();

	if (now - m_lastBroadcastTime < LAN_BROADCAST_INTERVAL_MS)
		return;

	m_lastBroadcastTime = now;

	sockaddr_in broadcastAddr;
	memset(&broadcastAddr, 0, sizeof(broadcastAddr));
	broadcastAddr.sin_family = AF_INET;
	broadcastAddr.sin_port = htons(LAN_BROADCAST_PORT);

	broadcastAddr.sin_addr.s_addr = INADDR_BROADCAST;
	sendto(m_broadcastSocket, (const char*)&m_broadcastPacket, sizeof(m_broadcastPacket), 0,
		(const sockaddr*)&broadcastAddr, sizeof(broadcastAddr));

	broadcastAddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	sendto(m_broadcastSocket, (const char*)&m_broadcastPacket, sizeof(m_broadcastPacket), 0,
		(const sockaddr*)&broadcastAddr, sizeof(broadcastAddr));
}

void CLANSessionManager::TickListen()
{
	LANBroadcastPacket packet;
	sockaddr_in fromAddr;
	int fromLen = sizeof(fromAddr);

	while (true)
	{
		int recvLen = recvfrom(m_listenSocket, (char*)&packet, sizeof(packet), 0,
			(sockaddr*)&fromAddr, &fromLen);

		if (recvLen <= 0)
			break;

		if (recvLen < (int)sizeof(LANBroadcastPacket))
			continue;

		if (packet.magic != LAN_BROADCAST_MAGIC)
			continue;

		if (packet.sessionId == m_localSessionId && m_broadcasting)
			continue;

		unsigned int senderIP = fromAddr.sin_addr.s_addr;

		__int64 now = System::currentTimeMillis();

		EnterCriticalSection(&m_sessionsLock);

		bool found = false;
		for (size_t i = 0; i < m_discoveredSessions.size(); i++)
		{
			if (m_discoveredSessions[i].info->sessionId == packet.sessionId)
			{
				m_discoveredSessions[i].lastSeenTime = now;
				m_discoveredSessions[i].hostIP = senderIP;
				m_discoveredSessions[i].hostPort = packet.gamePort;

				// Update session data
				m_discoveredSessions[i].info->data.m_uiGameHostSettings = packet.gameHostSettings;
				m_discoveredSessions[i].info->data.texturePackParentId = packet.texturePackParentId;
				m_discoveredSessions[i].info->data.subTexturePackId = packet.subTexturePackId;
				found = true;
				break;
			}
		}

		if (!found)
		{
			LANDiscoveredSession newSession;
			newSession.info = new FriendSessionInfo();
			newSession.info->sessionId = packet.sessionId;
			newSession.lastSeenTime = now;
			newSession.hostIP = senderIP;
			newSession.hostPort = packet.gamePort;

			newSession.info->data.netVersion = packet.netVersion;
			newSession.info->data.m_uiGameHostSettings = packet.gameHostSettings;
			newSession.info->data.texturePackParentId = packet.texturePackParentId;
			newSession.info->data.subTexturePackId = packet.subTexturePackId;
			newSession.info->data.isReadyToJoin = (packet.isJoinable != 0);

			wchar_t label[256];
			wchar_t wHostName[LAN_MAX_HOST_NAME];
			wchar_t wWorldName[LAN_MAX_WORLD_NAME];
			mbstowcs(wHostName, packet.hostName, LAN_MAX_HOST_NAME);
			mbstowcs(wWorldName, packet.worldName, LAN_MAX_WORLD_NAME);

			swprintf_s(label, 256, L"%s - %s (%d/%d)",
				wHostName, wWorldName, packet.playerCount, packet.maxPlayers);

			int labelLen = (int)wcslen(label);
			newSession.info->displayLabel = new wchar_t[labelLen + 1];
			wcscpy_s(newSession.info->displayLabel, labelLen + 1, label);
			newSession.info->displayLabelLength = (unsigned char)labelLen;
			newSession.info->hasPartyMember = false;

			m_discoveredSessions.push_back(newSession);
			m_sessionsChanged = true;

			in_addr addr;
			addr.s_addr = senderIP;
			app.DebugPrintf("LAN: Discovered session '%s' from %s (sessionId: %llu)\n",
				packet.hostName, inet_ntoa(addr), packet.sessionId);
		}

		LeaveCriticalSection(&m_sessionsLock);
	}
}

void CLANSessionManager::CleanExpiredSessions()
{
	__int64 now = System::currentTimeMillis();

	EnterCriticalSection(&m_sessionsLock);

	for (int i = (int)m_discoveredSessions.size() - 1; i >= 0; i--)
	{
		if (now - m_discoveredSessions[i].lastSeenTime > LAN_SESSION_EXPIRY_MS)
		{
			app.DebugPrintf("LAN: Session expired (sessionId: %llu)\n",
				m_discoveredSessions[i].info->sessionId);
			delete m_discoveredSessions[i].info;
			m_discoveredSessions.erase(m_discoveredSessions.begin() + i);
			m_sessionsChanged = true;
		}
	}

	LeaveCriticalSection(&m_sessionsLock);
}

std::vector<FriendSessionInfo*>* CLANSessionManager::GetSessionList()
{
	std::vector<FriendSessionInfo*>* list = new std::vector<FriendSessionInfo*>();

	EnterCriticalSection(&m_sessionsLock);

	for (size_t i = 0; i < m_discoveredSessions.size(); i++)
	{
		list->push_back(m_discoveredSessions[i].info);
	}

	LeaveCriticalSection(&m_sessionsLock);

	return list;
}

SessionID CLANSessionManager::GenerateSessionId()
{
	SessionID id = 0;
	id = (SessionID)GetCurrentProcessId();
	id = (id << 32) | (SessionID)(time(NULL) ^ rand());
	return id;
}

#endif // _WINDOWS64
