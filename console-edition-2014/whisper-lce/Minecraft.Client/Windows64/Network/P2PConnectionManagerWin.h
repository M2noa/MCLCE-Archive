#pragma once
#ifdef _WINDOWS64

#include <winsock2.h>
#include <ws2tcpip.h>
#include <map>
#include "..\..\Common\Network\P2PConnectionManager.h"
#include "..\..\Common\Network\NATTraversal.h"
#include "..\..\Common\Network\STUNClient.h"
#include "..\..\..\Minecraft.World\C4JThread.h"

// Max peers we can handle simultaneously
#define P2P_MAX_PEERS 8

// Max P2P packet size (UDP safe MTU)
#define P2P_MAX_PACKET_SIZE 1200

// Receive buffer size
#define P2P_RECV_BUFFER_SIZE 4096

class CP2PConnectionManagerWin : public IP2PConnectionManager
{
public:
	CP2PConnectionManagerWin();
	virtual ~CP2PConnectionManagerWin();

	// IP2PConnectionManager interface
	virtual bool Initialize();
	virtual void Shutdown();
	virtual void Tick();

	virtual bool EstablishDirectConnection(INetworkPlayer* local, INetworkPlayer* remote);
	virtual void DisconnectPeer(INetworkPlayer* remote);
	virtual void DisconnectAll();

	virtual void SendDirect(INetworkPlayer* target, const void* data, int size, EP2PChannel channel);
	virtual int ReceiveDirect(INetworkPlayer* source, void* buffer, int bufferSize, EP2PChannel channel);

	virtual EP2PConnectionState GetConnectionState(INetworkPlayer* player);
	virtual P2PConnectionQuality GetConnectionQuality(INetworkPlayer* player);
	virtual bool IsDirectConnectionAvailable(INetworkPlayer* player);
	virtual int GetDirectPeerCount();

	virtual bool DiscoverPublicEndpoint();
	virtual PeerEndpoint GetLocalEndpoint();
	virtual bool AttemptNATPunchthrough(INetworkPlayer* player, const PeerEndpoint& remoteEndpoint);

	virtual void HandleSignalingMessage(INetworkPlayer* from, EP2PSignalType type, const void* data, int size);
	virtual void SendSignalingMessage(INetworkPlayer* to, EP2PSignalType type, const void* data, int size);

	virtual wstring GetDebugStats();

	bool ConnectToKnownEndpoint(INetworkPlayer* player, const char* ip, unsigned short port);

private:
	// Internal methods
	void ProcessIncomingPackets();
	void ProcessHolePunching();
	void ProcessKeepalives();
	void ProcessTimeouts();

	PeerConnection* FindPeerByUID(PlayerUID uid);
	PeerConnection* FindPeerByAddress(const sockaddr_in& addr);
	PeerConnection* GetOrCreatePeer(INetworkPlayer* player);

	// UDP socket for all P2P communication
	SOCKET m_udpSocket;
	unsigned short m_localPort;

	// Our discovered public endpoint
	PeerEndpoint m_localEndpoint;
	bool m_endpointDiscovered;

	// STUN client
	CSTUNClientWin* m_stunClient;

	// Peer connections
	std::map<PlayerUID, PeerConnection> m_peerConnections;
	std::map<PlayerUID, sockaddr_in> m_peerAddresses;
	CRITICAL_SECTION m_peersLock;

	// Receive buffer
	unsigned char m_recvBuffer[P2P_RECV_BUFFER_SIZE];

	// State
	bool m_initialized;
	__int64 m_lastKeepaliveTime;
	__int64 m_lastTickTime;

	// Per-peer receive queues for P2P data pushed to game
	struct PeerRecvQueue
	{
		std::queue<std::vector<unsigned char>> packets;
		CRITICAL_SECTION lock;
	};
	std::map<PlayerUID, PeerRecvQueue*> m_recvQueues;

	PeerRecvQueue* GetRecvQueue(PlayerUID uid);
};

#endif // _WINDOWS64
