#pragma once
using namespace std;
#include <map>
#include "NetworkPlayerInterface.h"

// P2P channel types for routing different kinds of data
enum EP2PChannel
{
	P2P_CHANNEL_MOVEMENT = 0,		// Position/rotation updates
	P2P_CHANNEL_ANIMATION,			// Animate, RotateHead packets
	P2P_CHANNEL_EFFECTS,			// Visual effects, sounds
	P2P_CHANNEL_COUNT
};

// Connection state for a peer-to-peer link
enum EP2PConnectionState
{
	P2P_STATE_DISCONNECTED = 0,		// No connection
	P2P_STATE_DISCOVERING,			// Querying STUN for public endpoint
	P2P_STATE_PUNCHING,				// Attempting UDP hole punch
	P2P_STATE_CONNECTED,			// Direct P2P link established
	P2P_STATE_RELAY,				// Fallback: routing through host
	P2P_STATE_FAILED				// Connection attempt failed
};

// Quality metrics for a P2P connection
struct P2PConnectionQuality
{
	int rttMs;						// Round-trip time in milliseconds
	float packetLossPercent;		// Packet loss percentage (0-100)
	__int64 lastActivityTime;		// Timestamp of last received data
	bool isDirect;					// true if direct UDP, false if relayed

	P2PConnectionQuality() : rttMs(0), packetLossPercent(0.0f), lastActivityTime(0), isDirect(false) {}
};

// Endpoint info discovered via STUN
struct PeerEndpoint
{
	unsigned int publicIP;			// Public IP (network byte order)
	unsigned short publicPort;		// Public port (network byte order)
	unsigned int localIP;			// Local/private IP
	unsigned short localPort;		// Local port
	bool valid;

	PeerEndpoint() : publicIP(0), publicPort(0), localIP(0), localPort(0), valid(false) {}
};

// Represents a single peer-to-peer connection
struct PeerConnection
{
	PlayerUID playerUID;
	PeerEndpoint endpoint;
	EP2PConnectionState state;
	P2PConnectionQuality quality;
	int punchAttempts;
	__int64 lastPunchTime;
	__int64 connectionStartTime;

	PeerConnection() : playerUID(0), state(P2P_STATE_DISCONNECTED), punchAttempts(0),
		lastPunchTime(0), connectionStartTime(0) {}
};

// P2P signaling message types (sent via host TCP)
enum EP2PSignalType
{
	P2P_SIGNAL_ENDPOINT_INFO = 0,	// Client shares its public endpoint
	P2P_SIGNAL_PUNCH_REQUEST,		// Host tells clients to punch to each other
	P2P_SIGNAL_PUNCH_ACK,			// Client confirms P2P link is up
	P2P_SIGNAL_DISCONNECT			// Client reports P2P link down
};

// Abstract interface for P2P connection management
// Platform-specific implementations inherit from this
class IP2PConnectionManager
{
public:
	virtual ~IP2PConnectionManager() {}

	// Lifecycle
	virtual bool Initialize() = 0;
	virtual void Shutdown() = 0;
	virtual void Tick() = 0;								// Called each frame to process P2P state

	// Connection management
	virtual bool EstablishDirectConnection(INetworkPlayer* local, INetworkPlayer* remote) = 0;
	virtual void DisconnectPeer(INetworkPlayer* remote) = 0;
	virtual void DisconnectAll() = 0;

	// Data transmission
	virtual void SendDirect(INetworkPlayer* target, const void* data, int size, EP2PChannel channel) = 0;
	virtual int ReceiveDirect(INetworkPlayer* source, void* buffer, int bufferSize, EP2PChannel channel) = 0;

	// State queries
	virtual EP2PConnectionState GetConnectionState(INetworkPlayer* player) = 0;
	virtual P2PConnectionQuality GetConnectionQuality(INetworkPlayer* player) = 0;
	virtual bool IsDirectConnectionAvailable(INetworkPlayer* player) = 0;
	virtual int GetDirectPeerCount() = 0;

	// NAT traversal
	virtual bool DiscoverPublicEndpoint() = 0;
	virtual PeerEndpoint GetLocalEndpoint() = 0;
	virtual bool AttemptNATPunchthrough(INetworkPlayer* player, const PeerEndpoint& remoteEndpoint) = 0;

	// Signaling (via host TCP channel)
	virtual void HandleSignalingMessage(INetworkPlayer* from, EP2PSignalType type, const void* data, int size) = 0;
	virtual void SendSignalingMessage(INetworkPlayer* to, EP2PSignalType type, const void* data, int size) = 0;

	// Debug
	virtual wstring GetDebugStats() = 0;
};
