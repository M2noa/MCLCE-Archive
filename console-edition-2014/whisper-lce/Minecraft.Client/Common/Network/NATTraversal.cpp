#include "stdafx.h"
#include "NATTraversal.h"
#include <cstdlib>
#include <ctime>

#ifdef _WINDOWS64
#include <winsock2.h>
#include <ws2tcpip.h>
#endif

// Build a STUN Binding Request per RFC 5389
int NATHolePuncher::BuildBindingRequest(unsigned char* buffer, int bufferSize, STUNTransactionID& outTxnID)
{
	if (bufferSize < STUN_HEADER_SIZE)
		return 0;

	// Generate random transaction ID
	for (int i = 0; i < 12; i++)
		outTxnID.id[i] = (unsigned char)(rand() & 0xFF);

	STUNHeader* header = (STUNHeader*)buffer;
	header->type = htons(STUN_BINDING_REQUEST);
	header->length = htons(0);  // No attributes in binding request
	header->magicCookie = htonl(STUN_MAGIC_COOKIE);
	header->transactionID = outTxnID;

	return STUN_HEADER_SIZE;
}

// Parse a STUN Binding Response to extract the XOR-MAPPED-ADDRESS or MAPPED-ADDRESS
bool NATHolePuncher::ParseBindingResponse(const unsigned char* data, int dataSize,
	const STUNTransactionID& expectedTxnID, PeerEndpoint& outEndpoint)
{
	if (dataSize < STUN_HEADER_SIZE)
		return false;

	const STUNHeader* header = (const STUNHeader*)data;

	// Verify it's a binding response
	if (ntohs(header->type) != STUN_BINDING_RESPONSE)
		return false;

	// Verify magic cookie
	if (ntohl(header->magicCookie) != STUN_MAGIC_COOKIE)
		return false;

	// Verify transaction ID
	if (memcmp(header->transactionID.id, expectedTxnID.id, 12) != 0)
		return false;

	int attrLength = ntohs(header->length);
	if (STUN_HEADER_SIZE + attrLength > dataSize)
		return false;

	// Parse attributes looking for XOR-MAPPED-ADDRESS (preferred) or MAPPED-ADDRESS
	const unsigned char* ptr = data + STUN_HEADER_SIZE;
	const unsigned char* end = ptr + attrLength;

	bool foundAddress = false;

	while (ptr + 4 <= end)
	{
		unsigned short attrType = (ptr[0] << 8) | ptr[1];
		unsigned short attrLen = (ptr[2] << 8) | ptr[3];
		ptr += 4;

		if (ptr + attrLen > end)
			break;

		if (attrType == STUN_ATTR_XOR_MAPPED_ADDRESS && attrLen >= 8)
		{
			// XOR-MAPPED-ADDRESS: family(1) + port(2) + address(4), first byte reserved
			unsigned char family = ptr[1];
			if (family == 0x01)  // IPv4
			{
				unsigned short xorPort = (ptr[2] << 8) | ptr[3];
				outEndpoint.publicPort = xorPort ^ (unsigned short)(STUN_MAGIC_COOKIE >> 16);

				unsigned int xorAddr = (ptr[4] << 24) | (ptr[5] << 16) | (ptr[6] << 8) | ptr[7];
				outEndpoint.publicIP = htonl(xorAddr ^ STUN_MAGIC_COOKIE);

				outEndpoint.valid = true;
				foundAddress = true;
			}
		}
		else if (attrType == STUN_ATTR_MAPPED_ADDRESS && attrLen >= 8 && !foundAddress)
		{
			// MAPPED-ADDRESS fallback
			unsigned char family = ptr[1];
			if (family == 0x01)  // IPv4
			{
				outEndpoint.publicPort = (ptr[2] << 8) | ptr[3];
				outEndpoint.publicIP = htonl((ptr[4] << 24) | (ptr[5] << 16) | (ptr[6] << 8) | ptr[7]);
				outEndpoint.valid = true;
				foundAddress = true;
			}
		}

		// Advance to next attribute (padded to 4-byte boundary)
		ptr += (attrLen + 3) & ~3;
	}

	return foundAddress;
}

// P2P protocol packet format:
// [4 bytes magic "P2P\0"] [1 byte msg type] [payload...]

int NATHolePuncher::BuildPunchProbe(unsigned char* buffer, int bufferSize, PlayerUID senderUID)
{
	int packetSize = 4 + 1 + sizeof(PlayerUID);
	if (bufferSize < packetSize)
		return 0;

	buffer[0] = 'P';
	buffer[1] = '2';
	buffer[2] = 'P';
	buffer[3] = '\0';
	buffer[4] = P2P_MSG_PROBE;
	memcpy(buffer + 5, &senderUID, sizeof(PlayerUID));

	return packetSize;
}

bool NATHolePuncher::IsPunchProbe(const unsigned char* data, int dataSize, PlayerUID& outSenderUID)
{
	int expectedSize = 4 + 1 + (int)sizeof(PlayerUID);
	if (dataSize < expectedSize)
		return false;

	if (data[0] != 'P' || data[1] != '2' || data[2] != 'P' || data[3] != '\0')
		return false;

	if (data[4] != P2P_MSG_PROBE)
		return false;

	memcpy(&outSenderUID, data + 5, sizeof(PlayerUID));
	return true;
}

int NATHolePuncher::BuildPunchAck(unsigned char* buffer, int bufferSize, PlayerUID senderUID)
{
	int packetSize = 4 + 1 + (int)sizeof(PlayerUID);
	if (bufferSize < packetSize)
		return 0;

	buffer[0] = 'P';
	buffer[1] = '2';
	buffer[2] = 'P';
	buffer[3] = '\0';
	buffer[4] = P2P_MSG_ACK;
	memcpy(buffer + 5, &senderUID, sizeof(PlayerUID));

	return packetSize;
}

bool NATHolePuncher::IsPunchAck(const unsigned char* data, int dataSize, PlayerUID& outSenderUID)
{
	int expectedSize = 4 + 1 + (int)sizeof(PlayerUID);
	if (dataSize < expectedSize)
		return false;

	if (data[0] != 'P' || data[1] != '2' || data[2] != 'P' || data[3] != '\0')
		return false;

	if (data[4] != P2P_MSG_ACK)
		return false;

	memcpy(&outSenderUID, data + 5, sizeof(PlayerUID));
	return true;
}

int NATHolePuncher::BuildKeepalive(unsigned char* buffer, int bufferSize)
{
	int packetSize = 4 + 1;
	if (bufferSize < packetSize)
		return 0;

	buffer[0] = 'P';
	buffer[1] = '2';
	buffer[2] = 'P';
	buffer[3] = '\0';
	buffer[4] = P2P_MSG_KEEPALIVE;

	return packetSize;
}

bool NATHolePuncher::IsKeepalive(const unsigned char* data, int dataSize)
{
	if (dataSize < 5)
		return false;

	return (data[0] == 'P' && data[1] == '2' && data[2] == 'P' && data[3] == '\0' && data[4] == P2P_MSG_KEEPALIVE);
}
