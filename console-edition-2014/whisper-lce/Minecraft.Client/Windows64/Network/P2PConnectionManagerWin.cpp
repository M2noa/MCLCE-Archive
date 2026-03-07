#include "stdafx.h"
#ifdef _WINDOWS64

#include "P2PConnectionManagerWin.h"
#include "..\..\Common\Network\GameNetworkManager.h"
#include "..\..\..\Minecraft.World\System.h"
#include "..\..\..\Minecraft.World\Socket.h"

#pragma comment(lib, "ws2_32.lib")

CP2PConnectionManagerWin::CP2PConnectionManagerWin()
{
	m_udpSocket = INVALID_SOCKET;
	m_localPort = 0;
	m_endpointDiscovered = false;
	m_stunClient = NULL;
	m_initialized = false;
	m_lastKeepaliveTime = 0;
	m_lastTickTime = 0;
}

CP2PConnectionManagerWin::~CP2PConnectionManagerWin()
{
	Shutdown();
}

bool CP2PConnectionManagerWin::Initialize()
{
	if (m_initialized)
		return true;

	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	// Create UDP socket
	m_udpSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (m_udpSocket == INVALID_SOCKET)
	{
		app.DebugPrintf("P2P: Failed to create UDP socket: %d\n", WSAGetLastError());
		return false;
	}

	sockaddr_in localAddr;
	memset(&localAddr, 0, sizeof(localAddr));
	localAddr.sin_family = AF_INET;
	localAddr.sin_addr.s_addr = INADDR_ANY;
	localAddr.sin_port = 0;

	if (::bind(m_udpSocket, (sockaddr*)&localAddr, sizeof(localAddr)) == SOCKET_ERROR)
	{
		app.DebugPrintf("P2P: Failed to bind UDP socket: %d\n", WSAGetLastError());
		closesocket(m_udpSocket);
		m_udpSocket = INVALID_SOCKET;
		return false;
	}

	int addrLen = sizeof(localAddr);
	getsockname(m_udpSocket, (sockaddr*)&localAddr, &addrLen);
	m_localPort = ntohs(localAddr.sin_port);

	u_long nonBlocking = 1;
	ioctlsocket(m_udpSocket, FIONBIO, &nonBlocking);

	InitializeCriticalSection(&m_peersLock);

	m_stunClient = new CSTUNClientWin();

	m_initialized = true;
	m_lastKeepaliveTime = System::currentTimeMillis();
	m_lastTickTime = m_lastKeepaliveTime;

	app.DebugPrintf("P2P: Initialized on UDP port %d\n", m_localPort);

	return true;
}

void CP2PConnectionManagerWin::Shutdown()
{
	if (!m_initialized)
		return;

	DisconnectAll();

	if (m_udpSocket != INVALID_SOCKET)
	{
		closesocket(m_udpSocket);
		m_udpSocket = INVALID_SOCKET;
	}

	if (m_stunClient != NULL)
	{
		delete m_stunClient;
		m_stunClient = NULL;
	}

	for (auto it = m_recvQueues.begin(); it != m_recvQueues.end(); ++it)
	{
		DeleteCriticalSection(&it->second->lock);
		delete it->second;
	}
	m_recvQueues.clear();

	DeleteCriticalSection(&m_peersLock);

	m_initialized = false;
	m_endpointDiscovered = false;

	app.DebugPrintf("P2P: Shutdown complete\n");
}

void CP2PConnectionManagerWin::Tick()
{
	if (!m_initialized || m_udpSocket == INVALID_SOCKET)
		return;

	__int64 now = System::currentTimeMillis();
	m_lastTickTime = now;

	ProcessIncomingPackets();
	ProcessHolePunching();
	ProcessKeepalives();
	ProcessTimeouts();
}

bool CP2PConnectionManagerWin::EstablishDirectConnection(INetworkPlayer* local, INetworkPlayer* remote)
{
	if (!m_initialized || remote == NULL)
		return false;

	PlayerUID uid = remote->GetUID();

	EnterCriticalSection(&m_peersLock);

	PeerConnection* peer = GetOrCreatePeer(remote);
	if (peer->state == P2P_STATE_CONNECTED)
	{
		LeaveCriticalSection(&m_peersLock);
		return true;  // Already connected
	}

	peer->state = P2P_STATE_DISCOVERING;
	peer->connectionStartTime = System::currentTimeMillis();
	peer->punchAttempts = 0;

	LeaveCriticalSection(&m_peersLock);

	if (!m_endpointDiscovered)
	{
		DiscoverPublicEndpoint();
	}

	if (m_endpointDiscovered)
	{
		SendSignalingMessage(remote, P2P_SIGNAL_ENDPOINT_INFO, &m_localEndpoint, sizeof(PeerEndpoint));
	}

	app.DebugPrintf("P2P: Initiating connection to player %llu\n", uid);
	return true;
}

void CP2PConnectionManagerWin::DisconnectPeer(INetworkPlayer* remote)
{
	if (remote == NULL)
		return;

	PlayerUID uid = remote->GetUID();

	EnterCriticalSection(&m_peersLock);

	auto it = m_peerConnections.find(uid);
	if (it != m_peerConnections.end())
	{
		it->second.state = P2P_STATE_DISCONNECTED;
		m_peerConnections.erase(it);
		m_peerAddresses.erase(uid);
		app.DebugPrintf("P2P: Disconnected peer %llu\n", uid);
	}

	LeaveCriticalSection(&m_peersLock);
}

void CP2PConnectionManagerWin::DisconnectAll()
{
	EnterCriticalSection(&m_peersLock);
	m_peerConnections.clear();
	m_peerAddresses.clear();
	LeaveCriticalSection(&m_peersLock);

	app.DebugPrintf("P2P: Disconnected all peers\n");
}

void CP2PConnectionManagerWin::SendDirect(INetworkPlayer* target, const void* data, int size, EP2PChannel channel)
{
	if (!m_initialized || target == NULL || m_udpSocket == INVALID_SOCKET)
		return;

	PlayerUID uid = target->GetUID();

	EnterCriticalSection(&m_peersLock);

	auto addrIt = m_peerAddresses.find(uid);
	auto connIt = m_peerConnections.find(uid);

	if (addrIt == m_peerAddresses.end() || connIt == m_peerConnections.end() ||
		connIt->second.state != P2P_STATE_CONNECTED)
	{
		LeaveCriticalSection(&m_peersLock);
		return;  // No direct connection available
	}

	sockaddr_in peerAddr = addrIt->second;
	LeaveCriticalSection(&m_peersLock);

	if (size + 5 > P2P_MAX_PACKET_SIZE)
	{
		return;
	}

	unsigned char packetBuf[P2P_MAX_PACKET_SIZE];
	packetBuf[0] = 'P';
	packetBuf[1] = '2';
	packetBuf[2] = 'P';
	packetBuf[3] = '\0';
	packetBuf[4] = (unsigned char)channel;
	memcpy(packetBuf + 5, data, size);

	sendto(m_udpSocket, (const char*)packetBuf, size + 5, 0,
		(const sockaddr*)&peerAddr, sizeof(peerAddr));
}

int CP2PConnectionManagerWin::ReceiveDirect(INetworkPlayer* source, void* buffer, int bufferSize, EP2PChannel channel)
{
	if (source == NULL)
		return 0;

	PlayerUID uid = source->GetUID();
	PeerRecvQueue* queue = GetRecvQueue(uid);
	if (queue == NULL)
		return 0;

	EnterCriticalSection(&queue->lock);

	if (queue->packets.empty())
	{
		LeaveCriticalSection(&queue->lock);
		return 0;
	}

	std::vector<unsigned char>& front = queue->packets.front();
	int copySize = (int)front.size();
	if (copySize > bufferSize)
		copySize = bufferSize;

	memcpy(buffer, front.data(), copySize);
	queue->packets.pop();

	LeaveCriticalSection(&queue->lock);
	return copySize;
}

EP2PConnectionState CP2PConnectionManagerWin::GetConnectionState(INetworkPlayer* player)
{
	if (player == NULL)
		return P2P_STATE_DISCONNECTED;

	EnterCriticalSection(&m_peersLock);

	auto it = m_peerConnections.find(player->GetUID());
	EP2PConnectionState state = (it != m_peerConnections.end()) ? it->second.state : P2P_STATE_DISCONNECTED;

	LeaveCriticalSection(&m_peersLock);
	return state;
}

P2PConnectionQuality CP2PConnectionManagerWin::GetConnectionQuality(INetworkPlayer* player)
{
	P2PConnectionQuality quality;
	if (player == NULL)
		return quality;

	EnterCriticalSection(&m_peersLock);

	auto it = m_peerConnections.find(player->GetUID());
	if (it != m_peerConnections.end())
		quality = it->second.quality;

	LeaveCriticalSection(&m_peersLock);
	return quality;
}

bool CP2PConnectionManagerWin::IsDirectConnectionAvailable(INetworkPlayer* player)
{
	return GetConnectionState(player) == P2P_STATE_CONNECTED;
}

int CP2PConnectionManagerWin::GetDirectPeerCount()
{
	int count = 0;
	EnterCriticalSection(&m_peersLock);

	for (auto it = m_peerConnections.begin(); it != m_peerConnections.end(); ++it)
	{
		if (it->second.state == P2P_STATE_CONNECTED)
			count++;
	}

	LeaveCriticalSection(&m_peersLock);
	return count;
}

bool CP2PConnectionManagerWin::DiscoverPublicEndpoint()
{
	if (m_stunClient == NULL)
		return false;

	if (m_stunClient->DiscoverEndpoint(m_localEndpoint))
	{
		m_localEndpoint.localPort = m_localPort;
		m_endpointDiscovered = true;

		in_addr addr;
		addr.s_addr = m_localEndpoint.publicIP;
		app.DebugPrintf("P2P: Discovered public endpoint %s:%d\n",
			inet_ntoa(addr), m_localEndpoint.publicPort);

		return true;
	}

	app.DebugPrintf("P2P: STUN discovery failed: %s\n", m_stunClient->GetLastError());
	return false;
}

PeerEndpoint CP2PConnectionManagerWin::GetLocalEndpoint()
{
	return m_localEndpoint;
}

bool CP2PConnectionManagerWin::AttemptNATPunchthrough(INetworkPlayer* player, const PeerEndpoint& remoteEndpoint)
{
	if (!m_initialized || player == NULL || m_udpSocket == INVALID_SOCKET)
		return false;

	PlayerUID uid = player->GetUID();

	EnterCriticalSection(&m_peersLock);

	PeerConnection* peer = GetOrCreatePeer(player);
	peer->endpoint = remoteEndpoint;
	peer->state = P2P_STATE_PUNCHING;
	peer->punchAttempts = 0;
	peer->lastPunchTime = 0;

	sockaddr_in remoteAddr;
	memset(&remoteAddr, 0, sizeof(remoteAddr));
	remoteAddr.sin_family = AF_INET;
	remoteAddr.sin_addr.s_addr = remoteEndpoint.publicIP;
	remoteAddr.sin_port = htons(remoteEndpoint.publicPort);

	m_peerAddresses[uid] = remoteAddr;

	LeaveCriticalSection(&m_peersLock);

	app.DebugPrintf("P2P: Starting NAT punchthrough to player %llu\n", uid);
	return true;
}

void CP2PConnectionManagerWin::HandleSignalingMessage(INetworkPlayer* from, EP2PSignalType type,
	const void* data, int size)
{
	if (from == NULL)
		return;

	switch (type)
	{
	case P2P_SIGNAL_ENDPOINT_INFO:
	{
		if (size >= (int)sizeof(PeerEndpoint))
		{
			PeerEndpoint remoteEndpoint;
			memcpy(&remoteEndpoint, data, sizeof(PeerEndpoint));
			AttemptNATPunchthrough(from, remoteEndpoint);
		}
		break;
	}

	case P2P_SIGNAL_PUNCH_REQUEST:
	{
		if (size >= (int)sizeof(PeerEndpoint))
		{
			PeerEndpoint remoteEndpoint;
			memcpy(&remoteEndpoint, data, sizeof(PeerEndpoint));
			AttemptNATPunchthrough(from, remoteEndpoint);
		}
		break;
	}

	case P2P_SIGNAL_PUNCH_ACK:
	{
		EnterCriticalSection(&m_peersLock);
		PeerConnection* peer = FindPeerByUID(from->GetUID());
		if (peer != NULL && peer->state == P2P_STATE_PUNCHING)
		{
			peer->state = P2P_STATE_CONNECTED;
			peer->quality.isDirect = true;
			peer->quality.lastActivityTime = System::currentTimeMillis();
			app.DebugPrintf("P2P: Connection established with player %llu (via signaling ack)\n", from->GetUID());
		}
		LeaveCriticalSection(&m_peersLock);
		break;
	}

	case P2P_SIGNAL_DISCONNECT:
	{
		DisconnectPeer(from);
		break;
	}
	}
}

void CP2PConnectionManagerWin::SendSignalingMessage(INetworkPlayer* to, EP2PSignalType type,
	const void* data, int size)
{

	INetworkPlayer* hostPlayer = g_NetworkManager.GetHostPlayer();
	if (hostPlayer == NULL)
		return;

	int totalSize = 1 + (int)sizeof(PlayerUID) + size;
	unsigned char* signalBuf = new unsigned char[totalSize];

	signalBuf[0] = (unsigned char)type;
	PlayerUID targetUID = to->GetUID();
	memcpy(signalBuf + 1, &targetUID, sizeof(PlayerUID));
	if (size > 0 && data != NULL)
		memcpy(signalBuf + 1 + sizeof(PlayerUID), data, size);

	hostPlayer->SendData(to, signalBuf, totalSize, true);  // low priority

	delete[] signalBuf;
}

wstring CP2PConnectionManagerWin::GetDebugStats()
{
	wstring stats = L"=== P2P Stats ===\n";

	EnterCriticalSection(&m_peersLock);

	wchar_t buf[256];
	swprintf_s(buf, L"Endpoint discovered: %s\n", m_endpointDiscovered ? L"Yes" : L"No");
	stats += buf;

	swprintf_s(buf, L"Local port: %d\n", m_localPort);
	stats += buf;

	swprintf_s(buf, L"Connected peers: %d\n", GetDirectPeerCount());
	stats += buf;

	for (auto it = m_peerConnections.begin(); it != m_peerConnections.end(); ++it)
	{
		const wchar_t* stateStr = L"Unknown";
		switch (it->second.state)
		{
		case P2P_STATE_DISCONNECTED: stateStr = L"Disconnected"; break;
		case P2P_STATE_DISCOVERING:  stateStr = L"Discovering"; break;
		case P2P_STATE_PUNCHING:     stateStr = L"Punching"; break;
		case P2P_STATE_CONNECTED:    stateStr = L"Connected"; break;
		case P2P_STATE_RELAY:        stateStr = L"Relay"; break;
		case P2P_STATE_FAILED:       stateStr = L"Failed"; break;
		}
		swprintf_s(buf, L"  Peer %llu: %s (RTT: %dms)\n",
			it->first, stateStr, it->second.quality.rttMs);
		stats += buf;
	}

	LeaveCriticalSection(&m_peersLock);

	return stats;
}

// ---- Internal methods ----

void CP2PConnectionManagerWin::ProcessIncomingPackets()
{
	if (m_udpSocket == INVALID_SOCKET)
		return;

	sockaddr_in fromAddr;
	int fromLen = sizeof(fromAddr);

	while (true)
	{
		int recvLen = recvfrom(m_udpSocket, (char*)m_recvBuffer, P2P_RECV_BUFFER_SIZE, 0,
			(sockaddr*)&fromAddr, &fromLen);

		if (recvLen <= 0)
			break;  // No more data or error

		// Check if it's a P2P protocol message (punch probe/ack/keepalive)
		PlayerUID senderUID;

		if (NATHolePuncher::IsPunchProbe(m_recvBuffer, recvLen, senderUID))
		{
			EnterCriticalSection(&m_peersLock);
			PeerConnection* peer = FindPeerByUID(senderUID);
			if (peer != NULL)
			{
				m_peerAddresses[senderUID] = fromAddr;

				INetworkPlayer* localPlayer = g_NetworkManager.GetLocalPlayerByUserIndex(0);
				if (localPlayer != NULL)
				{
					unsigned char ackBuf[64];
					int ackSize = NATHolePuncher::BuildPunchAck(ackBuf, sizeof(ackBuf), localPlayer->GetUID());
					sendto(m_udpSocket, (const char*)ackBuf, ackSize, 0,
						(const sockaddr*)&fromAddr, sizeof(fromAddr));
				}

				if (peer->state == P2P_STATE_PUNCHING)
				{
					peer->state = P2P_STATE_CONNECTED;
					peer->quality.isDirect = true;
					peer->quality.lastActivityTime = System::currentTimeMillis();
					app.DebugPrintf("P2P: Connection established with player %llu (probe received)\n", senderUID);
				}
			}
			LeaveCriticalSection(&m_peersLock);
			continue;
		}

		if (NATHolePuncher::IsPunchAck(m_recvBuffer, recvLen, senderUID))
		{
			EnterCriticalSection(&m_peersLock);
			PeerConnection* peer = FindPeerByUID(senderUID);
			if (peer != NULL)
			{
				m_peerAddresses[senderUID] = fromAddr;
				if (peer->state == P2P_STATE_PUNCHING)
				{
					peer->state = P2P_STATE_CONNECTED;
					peer->quality.isDirect = true;
					peer->quality.lastActivityTime = System::currentTimeMillis();
					app.DebugPrintf("P2P: Connection established with player %llu (ack received)\n", senderUID);
				}
			}
			LeaveCriticalSection(&m_peersLock);
			continue;
		}

		if (NATHolePuncher::IsKeepalive(m_recvBuffer, recvLen))
		{
			EnterCriticalSection(&m_peersLock);
			PeerConnection* peer = FindPeerByAddress(fromAddr);
			if (peer != NULL)
			{
				peer->quality.lastActivityTime = System::currentTimeMillis();
			}
			LeaveCriticalSection(&m_peersLock);
			continue;
		}

		if (recvLen >= 5 && m_recvBuffer[0] == 'P' && m_recvBuffer[1] == '2' &&
			m_recvBuffer[2] == 'P' && m_recvBuffer[3] == '\0')
		{
			EnterCriticalSection(&m_peersLock);
			PeerConnection* peer = FindPeerByAddress(fromAddr);
			if (peer != NULL && peer->state == P2P_STATE_CONNECTED)
			{
				peer->quality.lastActivityTime = System::currentTimeMillis();

				int dataSize = recvLen - 5;
				if (dataSize > 0)
				{
					PeerRecvQueue* queue = GetRecvQueue(peer->playerUID);
					if (queue != NULL)
					{
						EnterCriticalSection(&queue->lock);
						std::vector<unsigned char> packet(m_recvBuffer + 5, m_recvBuffer + 5 + dataSize);
						queue->packets.push(packet);
						LeaveCriticalSection(&queue->lock);
					}

					INetworkPlayer* peerPlayer = g_NetworkManager.GetPlayerByXuid(peer->playerUID);
					if (peerPlayer != NULL)
					{
						Socket* pSocket = peerPlayer->GetSocket();
						if (pSocket != NULL)
						{
							pSocket->pushDataToQueue(m_recvBuffer + 5, dataSize, true);
						}
					}
				}
			}
			LeaveCriticalSection(&m_peersLock);
		}
	}
}

void CP2PConnectionManagerWin::ProcessHolePunching()
{
	__int64 now = System::currentTimeMillis();

	EnterCriticalSection(&m_peersLock);

	for (auto it = m_peerConnections.begin(); it != m_peerConnections.end(); ++it)
	{
		PeerConnection& peer = it->second;
		if (peer.state != P2P_STATE_PUNCHING)
			continue;

		if (now - peer.connectionStartTime > NAT_PUNCH_TIMEOUT_MS)
		{
			peer.state = P2P_STATE_RELAY;  // Fall back to host routing
			app.DebugPrintf("P2P: Hole punch timed out for player %llu, falling back to relay\n", peer.playerUID);
			continue;
		}

		if (now - peer.lastPunchTime >= NAT_PUNCH_INTERVAL_MS && peer.punchAttempts < NAT_PUNCH_MAX_ATTEMPTS)
		{
			auto addrIt = m_peerAddresses.find(peer.playerUID);
			if (addrIt != m_peerAddresses.end())
			{
				INetworkPlayer* localPlayer = g_NetworkManager.GetLocalPlayerByUserIndex(0);
				if (localPlayer != NULL)
				{
					unsigned char probeBuf[64];
					int probeSize = NATHolePuncher::BuildPunchProbe(probeBuf, sizeof(probeBuf), localPlayer->GetUID());

					sendto(m_udpSocket, (const char*)probeBuf, probeSize, 0,
						(const sockaddr*)&addrIt->second, sizeof(sockaddr_in));

					peer.punchAttempts++;
					peer.lastPunchTime = now;
				}
			}
		}
	}

	LeaveCriticalSection(&m_peersLock);
}

void CP2PConnectionManagerWin::ProcessKeepalives()
{
	__int64 now = System::currentTimeMillis();

	if (now - m_lastKeepaliveTime < P2P_KEEPALIVE_INTERVAL_MS)
		return;

	m_lastKeepaliveTime = now;

	unsigned char keepaliveBuf[8];
	int keepaliveSize = NATHolePuncher::BuildKeepalive(keepaliveBuf, sizeof(keepaliveBuf));

	EnterCriticalSection(&m_peersLock);

	for (auto it = m_peerConnections.begin(); it != m_peerConnections.end(); ++it)
	{
		if (it->second.state != P2P_STATE_CONNECTED)
			continue;

		auto addrIt = m_peerAddresses.find(it->first);
		if (addrIt != m_peerAddresses.end())
		{
			sendto(m_udpSocket, (const char*)keepaliveBuf, keepaliveSize, 0,
				(const sockaddr*)&addrIt->second, sizeof(sockaddr_in));
		}
	}

	LeaveCriticalSection(&m_peersLock);
}

void CP2PConnectionManagerWin::ProcessTimeouts()
{
	__int64 now = System::currentTimeMillis();

	EnterCriticalSection(&m_peersLock);

	for (auto it = m_peerConnections.begin(); it != m_peerConnections.end(); ++it)
	{
		if (it->second.state != P2P_STATE_CONNECTED)
			continue;

		if (now - it->second.quality.lastActivityTime > P2P_CONNECTION_TIMEOUT_MS)
		{
			it->second.state = P2P_STATE_RELAY;
			it->second.quality.isDirect = false;
			app.DebugPrintf("P2P: Connection to player %llu timed out, falling back to relay\n", it->first);
		}
	}

	LeaveCriticalSection(&m_peersLock);
}

PeerConnection* CP2PConnectionManagerWin::FindPeerByUID(PlayerUID uid)
{
	auto it = m_peerConnections.find(uid);
	if (it != m_peerConnections.end())
		return &it->second;
	return NULL;
}

PeerConnection* CP2PConnectionManagerWin::FindPeerByAddress(const sockaddr_in& addr)
{
	for (auto it = m_peerAddresses.begin(); it != m_peerAddresses.end(); ++it)
	{
		if (it->second.sin_addr.s_addr == addr.sin_addr.s_addr &&
			it->second.sin_port == addr.sin_port)
		{
			return FindPeerByUID(it->first);
		}
	}
	return NULL;
}

PeerConnection* CP2PConnectionManagerWin::GetOrCreatePeer(INetworkPlayer* player)
{
	PlayerUID uid = player->GetUID();
	auto it = m_peerConnections.find(uid);
	if (it != m_peerConnections.end())
		return &it->second;

	PeerConnection newPeer;
	newPeer.playerUID = uid;
	newPeer.state = P2P_STATE_DISCONNECTED;
	m_peerConnections[uid] = newPeer;

	return &m_peerConnections[uid];
}

CP2PConnectionManagerWin::PeerRecvQueue* CP2PConnectionManagerWin::GetRecvQueue(PlayerUID uid)
{
	auto it = m_recvQueues.find(uid);
	if (it != m_recvQueues.end())
		return it->second;

	PeerRecvQueue* queue = new PeerRecvQueue();
	InitializeCriticalSection(&queue->lock);
	m_recvQueues[uid] = queue;
	return queue;
}

bool CP2PConnectionManagerWin::ConnectToKnownEndpoint(INetworkPlayer* player, const char* ip, unsigned short port)
{
    if (!m_initialized || player == NULL || m_udpSocket == INVALID_SOCKET)
        return false;

    // Resolve the IP string to a sockaddr_in
    sockaddr_in remoteAddr;
    memset(&remoteAddr, 0, sizeof(remoteAddr));
    remoteAddr.sin_family = AF_INET;
    remoteAddr.sin_port   = htons(port);

    if (inet_pton(AF_INET, ip, &remoteAddr.sin_addr) != 1)
    {
        app.DebugPrintf("P2P: ConnectToKnownEndpoint - invalid IP: %s\n", ip);
        return false;
    }

    PlayerUID uid = player->GetUID();

    EnterCriticalSection(&m_peersLock);

    // Bypass all discovery/punching — store address and mark as connected directly
    m_peerAddresses[uid] = remoteAddr;

    PeerConnection* peer = GetOrCreatePeer(player);
    peer->state                      = P2P_STATE_CONNECTED;
    peer->quality.isDirect           = true;
    peer->quality.lastActivityTime   = System::currentTimeMillis();
    peer->connectionStartTime        = System::currentTimeMillis();
    peer->punchAttempts              = 0;

    LeaveCriticalSection(&m_peersLock);

    app.DebugPrintf("P2P: Direct connect to %s:%d for player %llu\n", ip, port, uid);
    return true;
}


#endif // _WINDOWS64
