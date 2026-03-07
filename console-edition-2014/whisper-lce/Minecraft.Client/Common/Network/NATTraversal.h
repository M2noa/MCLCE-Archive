#pragma once
#include "P2PConnectionManager.h"

// STUN message types (RFC 5389)
#define STUN_BINDING_REQUEST			0x0001
#define STUN_BINDING_RESPONSE			0x0101
#define STUN_BINDING_ERROR_RESPONSE		0x0111

// STUN attribute types
#define STUN_ATTR_MAPPED_ADDRESS		0x0001
#define STUN_ATTR_XOR_MAPPED_ADDRESS	0x0020
#define STUN_ATTR_SOFTWARE				0x8022

// STUN magic cookie (RFC 5389)
#define STUN_MAGIC_COOKIE				0x2112A442

// STUN header size
#define STUN_HEADER_SIZE				20

// Max STUN response size
#define STUN_MAX_RESPONSE_SIZE			512

// NAT traversal timeouts
#define NAT_STUN_TIMEOUT_MS				3000
#define NAT_PUNCH_INTERVAL_MS			500
#define NAT_PUNCH_MAX_ATTEMPTS			10
#define NAT_PUNCH_TIMEOUT_MS			5000

// P2P keepalive interval
#define P2P_KEEPALIVE_INTERVAL_MS		2000

// P2P connection timeout (no data received)
#define P2P_CONNECTION_TIMEOUT_MS		10000

// STUN server entry
struct STUNServer
{
	const char* hostname;
	unsigned short port;
};

// Well-known public STUN servers
static const STUNServer g_STUNServers[] =
{
	{ "stun.l.google.com",			19302 },
	{ "stun1.l.google.com",		19302 },
	{ "stun.stunprotocol.org",		3478  },
};
static const int g_numSTUNServers = sizeof(g_STUNServers) / sizeof(g_STUNServers[0]);

// STUN transaction ID (96 bits)
struct STUNTransactionID
{
	unsigned char id[12];
};

// STUN message header
#pragma pack(push, 1)
struct STUNHeader
{
	unsigned short type;
	unsigned short length;
	unsigned int magicCookie;
	STUNTransactionID transactionID;
};
#pragma pack(pop)

// NAT type classification
enum ENATType
{
	NAT_TYPE_UNKNOWN = 0,
	NAT_TYPE_OPEN,					// No NAT / directly reachable
	NAT_TYPE_FULL_CONE,				// Easy to traverse
	NAT_TYPE_RESTRICTED_CONE,		// Moderate - needs hole punch
	NAT_TYPE_PORT_RESTRICTED,		// Harder - needs coordinated punch
	NAT_TYPE_SYMMETRIC				// Hardest - may need relay
};

// Abstract STUN client interface
class ISTUNClient
{
public:
	virtual ~ISTUNClient() {}

	virtual bool DiscoverEndpoint(PeerEndpoint& outEndpoint) = 0;

	// Classify NAT type (optional, for diagnostics)
	virtual ENATType ClassifyNAT() = 0;

	// Get last error description
	virtual const char* GetLastError() = 0;
};

// NAT hole punch helper
class NATHolePuncher
{
public:
	// Generate a STUN binding request
	static int BuildBindingRequest(unsigned char* buffer, int bufferSize, STUNTransactionID& outTxnID);

	// Parse a STUN binding response to extract mapped address
	static bool ParseBindingResponse(const unsigned char* data, int dataSize,
		const STUNTransactionID& expectedTxnID, PeerEndpoint& outEndpoint);

	// Generate a hole punch probe packet
	static int BuildPunchProbe(unsigned char* buffer, int bufferSize, PlayerUID senderUID);

	// Check if received data is a punch probe and extract sender UID
	static bool IsPunchProbe(const unsigned char* data, int dataSize, PlayerUID& outSenderUID);

	// Generate a punch acknowledgment
	static int BuildPunchAck(unsigned char* buffer, int bufferSize, PlayerUID senderUID);

	// Check if received data is a punch acknowledgment
	static bool IsPunchAck(const unsigned char* data, int dataSize, PlayerUID& outSenderUID);

	// Generate a keepalive packet
	static int BuildKeepalive(unsigned char* buffer, int bufferSize);

	// Check if received data is a keepalive
	static bool IsKeepalive(const unsigned char* data, int dataSize);

private:
	// P2P protocol magic bytes to distinguish from game data
	static const unsigned int P2P_MAGIC = 0x4D435032;  // "MCP2"
	static const unsigned char P2P_MSG_PROBE = 0x01;
	static const unsigned char P2P_MSG_ACK   = 0x02;
	static const unsigned char P2P_MSG_KEEPALIVE = 0x03;
};
