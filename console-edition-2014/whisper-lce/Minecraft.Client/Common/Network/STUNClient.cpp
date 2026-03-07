#include "stdafx.h"
#ifdef _WINDOWS64

#include "STUNClient.h"
#include <cstdlib>
#include <ctime>

#pragma comment(lib, "ws2_32.lib")

CSTUNClientWin::CSTUNClientWin()
{
	m_lastError[0] = '\0';
	srand((unsigned int)time(NULL));
}

CSTUNClientWin::~CSTUNClientWin()
{
}

bool CSTUNClientWin::DiscoverEndpoint(PeerEndpoint& outEndpoint)
{
	SOCKET sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (sock == INVALID_SOCKET)
	{
		sprintf_s(m_lastError, "Failed to create UDP socket: %d", WSAGetLastError());
		return false;
	}

	sockaddr_in localAddr;
	memset(&localAddr, 0, sizeof(localAddr));
	localAddr.sin_family = AF_INET;
	localAddr.sin_addr.s_addr = INADDR_ANY;
	localAddr.sin_port = 0;  // OS assigns port

	if (::bind(sock, (sockaddr*)&localAddr, sizeof(localAddr)) == SOCKET_ERROR)
	{
		sprintf_s(m_lastError, "Failed to bind UDP socket: %d", WSAGetLastError());
		closesocket(sock);
		return false;
	}

	int addrLen = sizeof(localAddr);
	getsockname(sock, (sockaddr*)&localAddr, &addrLen);
	outEndpoint.localPort = ntohs(localAddr.sin_port);

	char hostname[256];
	if (gethostname(hostname, sizeof(hostname)) == 0)
	{
		struct addrinfo hints, *result;
		memset(&hints, 0, sizeof(hints));
		hints.ai_family = AF_INET;
		if (getaddrinfo(hostname, NULL, &hints, &result) == 0)
		{
			sockaddr_in* addr = (sockaddr_in*)result->ai_addr;
			outEndpoint.localIP = addr->sin_addr.s_addr;
			freeaddrinfo(result);
		}
	}

	DWORD timeout = NAT_STUN_TIMEOUT_MS;
	setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout));

	bool success = false;
	for (int i = 0; i < g_numSTUNServers && !success; i++)
	{
		sockaddr_in serverAddr;
		if (ResolveSTUNServer(g_STUNServers[i].hostname, g_STUNServers[i].port, serverAddr))
		{
			success = SendBindingRequest(sock, serverAddr, outEndpoint);
		}
	}

	closesocket(sock);

	if (!success)
	{
		sprintf_s(m_lastError, "All STUN servers failed to respond");
	}

	return success;
}

bool CSTUNClientWin::ResolveSTUNServer(const char* hostname, unsigned short port, sockaddr_in& outAddr)
{
	struct addrinfo hints, *result;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_DGRAM;

	char portStr[16];
	sprintf_s(portStr, "%d", port);

	if (getaddrinfo(hostname, portStr, &hints, &result) != 0)
	{
		sprintf_s(m_lastError, "Failed to resolve STUN server: %s", hostname);
		return false;
	}

	memcpy(&outAddr, result->ai_addr, sizeof(sockaddr_in));
	freeaddrinfo(result);
	return true;
}

bool CSTUNClientWin::SendBindingRequest(SOCKET sock, const sockaddr_in& serverAddr, PeerEndpoint& outEndpoint)
{
	unsigned char requestBuffer[STUN_HEADER_SIZE];
	STUNTransactionID txnID;

	int reqSize = NATHolePuncher::BuildBindingRequest(requestBuffer, sizeof(requestBuffer), txnID);
	if (reqSize == 0)
		return false;

	if (sendto(sock, (const char*)requestBuffer, reqSize, 0,
		(const sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
	{
		sprintf_s(m_lastError, "Failed to send STUN request: %d", WSAGetLastError());
		return false;
	}

	unsigned char responseBuffer[STUN_MAX_RESPONSE_SIZE];
	sockaddr_in fromAddr;
	int fromLen = sizeof(fromAddr);

	int recvLen = recvfrom(sock, (char*)responseBuffer, sizeof(responseBuffer), 0,
		(sockaddr*)&fromAddr, &fromLen);

	if (recvLen <= 0)
	{
		sprintf_s(m_lastError, "STUN response timeout or error: %d", WSAGetLastError());
		return false;
	}

	return NATHolePuncher::ParseBindingResponse(responseBuffer, recvLen, txnID, outEndpoint);
}

ENATType CSTUNClientWin::ClassifyNAT()
{
	PeerEndpoint endpoint1, endpoint2;

	SOCKET sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (sock == INVALID_SOCKET)
		return NAT_TYPE_UNKNOWN;

	sockaddr_in localAddr;
	memset(&localAddr, 0, sizeof(localAddr));
	localAddr.sin_family = AF_INET;
	localAddr.sin_addr.s_addr = INADDR_ANY;
	localAddr.sin_port = 0;
	::bind(sock, (sockaddr*)&localAddr, sizeof(localAddr));

	DWORD timeout = NAT_STUN_TIMEOUT_MS;
	setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout));

	sockaddr_in server1, server2;
	bool resolved1 = (g_numSTUNServers >= 1) && ResolveSTUNServer(g_STUNServers[0].hostname, g_STUNServers[0].port, server1);
	bool resolved2 = (g_numSTUNServers >= 2) && ResolveSTUNServer(g_STUNServers[1].hostname, g_STUNServers[1].port, server2);

	bool got1 = resolved1 && SendBindingRequest(sock, server1, endpoint1);
	bool got2 = resolved2 && SendBindingRequest(sock, server2, endpoint2);

	closesocket(sock);

	if (!got1)
		return NAT_TYPE_UNKNOWN;

	if (!got2)
		return NAT_TYPE_RESTRICTED_CONE;  // Can't determine precisely

	if (endpoint1.publicPort == endpoint2.publicPort)
		return NAT_TYPE_FULL_CONE;

	return NAT_TYPE_SYMMETRIC;
}

const char* CSTUNClientWin::GetLastError()
{
	return m_lastError;
}

#endif // _WINDOWS64
