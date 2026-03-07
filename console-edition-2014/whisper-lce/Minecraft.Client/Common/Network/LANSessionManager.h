#pragma once
#ifdef _WINDOWS64

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <vector>
#include <string>
#include "SessionInfo.h"

#pragma comment(lib, "ws2_32.lib")

// LAN broadcast port for session discovery
#define LAN_BROADCAST_PORT		4445

// Broadcast interval in milliseconds
#define LAN_BROADCAST_INTERVAL_MS	2000

// Session expiry time (if no broadcast received in this time, remove session)
#define LAN_SESSION_EXPIRY_MS	8000

// Broadcast packet magic
#define LAN_BROADCAST_MAGIC		0x4A344D43  // "MC4J"

// Max world name length in broadcast
#define LAN_MAX_WORLD_NAME		64
#define LAN_MAX_HOST_NAME		32

#pragma pack(push, 1)
struct LANBroadcastPacket
{
	unsigned int magic;
	unsigned short netVersion;
	unsigned int gameHostSettings;
	unsigned int texturePackParentId;
	unsigned char subTexturePackId;
	unsigned char playerCount;
	unsigned char maxPlayers;
	unsigned char isJoinable;
	unsigned short gamePort;				// Reserved for future TCP game connection
	unsigned int hostIP;					// Host's LAN IP (filled by receiver from recvfrom)
	SessionID sessionId;					// Unique session ID
	char hostName[LAN_MAX_HOST_NAME];		// UTF-8 host display name
	char worldName[LAN_MAX_WORLD_NAME];		// UTF-8 world name
};
#pragma pack(pop)

struct LANDiscoveredSession
{
	FriendSessionInfo* info;
	__int64 lastSeenTime;
	unsigned int hostIP;
	unsigned short hostPort;
};

class CLANSessionManager
{
public:
	CLANSessionManager();
	~CLANSessionManager();

	bool Initialize();
	void Shutdown();

	// Broadcasting (host side)
	void StartBroadcasting(const GameSessionData& sessionData, const char* hostName, const char* worldName);
	void StopBroadcasting();
	void SetPlayerCount(unsigned char count);
	bool IsBroadcasting() { return m_broadcasting; }

	// Discovery (client side)
	void StartListening();
	void StopListening();
	bool IsListening() { return m_listening; }

	// Tick - call from DoWork
	void Tick();

	// Get discovered sessions
	std::vector<FriendSessionInfo*>* GetSessionList();
	bool HasNewSessions() { return m_sessionsChanged; }
	void ClearNewSessionsFlag() { m_sessionsChanged = false; }

private:
	void TickBroadcast();
	void TickListen();
	void CleanExpiredSessions();
	SessionID GenerateSessionId();

	SOCKET m_broadcastSocket;
	SOCKET m_listenSocket;
	bool m_initialized;
	bool m_broadcasting;
	bool m_listening;
	bool m_sessionsChanged;

	// Broadcast state
	LANBroadcastPacket m_broadcastPacket;
	__int64 m_lastBroadcastTime;
	SessionID m_localSessionId;

	// Discovery state
	std::vector<LANDiscoveredSession> m_discoveredSessions;
	CRITICAL_SECTION m_sessionsLock;
};

#endif // _WINDOWS64
