#pragma once
#ifdef _WINDOWS64

#include <winsock2.h>
#include <ws2tcpip.h>
#include "NATTraversal.h"

class CSTUNClientWin : public ISTUNClient
{
public:
	CSTUNClientWin();
	~CSTUNClientWin();

	// ISTUNClient interface
	virtual bool DiscoverEndpoint(PeerEndpoint& outEndpoint);
	virtual ENATType ClassifyNAT();
	virtual const char* GetLastError();

private:
	// Resolve STUN server hostname to IP
	bool ResolveSTUNServer(const char* hostname, unsigned short port, sockaddr_in& outAddr);

	// Send binding request and wait for response
	bool SendBindingRequest(SOCKET sock, const sockaddr_in& serverAddr, PeerEndpoint& outEndpoint);

	char m_lastError[256];
};

#endif // _WINDOWS64
