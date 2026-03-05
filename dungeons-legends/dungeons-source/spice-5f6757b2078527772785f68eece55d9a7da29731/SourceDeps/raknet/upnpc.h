#pragma once

#include "RakNetDefines.h"
#include <memory.h>
#include <functional>

struct UPNPPortMappingResult {
	bool bUPNPSupported;
	char hostIPAddress[MAXIMUM_HOSTNAME_LENGTH];
	short externalPort;
	short internalPort;
};

struct UPNPExternalIPResult {
	bool bUPNPSupported;
	char hostIPAddress[MAXIMUM_HOSTNAME_LENGTH];
};

union UPNPResult {	
	UPNPExternalIPResult externalIP;
	UPNPPortMappingResult portMapping;

	UPNPResult() {
		memset(this, 0, sizeof(UPNPResult));
	}
};

typedef std::function<void(UPNPResult&)> UPNPResultCallback;

void UPNPCreatePortMappingAsync(unsigned short portToOpen,
	unsigned int timeout,
	const char* descriptor,
	bool isIPv6,
	UPNPResultCallback onCompleted);

void UPNPGetExternalIPAsync(unsigned int timeout, UPNPResultCallback onCompleted);
