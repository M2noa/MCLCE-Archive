
#include "upnpc.h"

#ifdef SUPPORT_UPNP
#include "RakThread.h" 
#include "GetTime.h"
#include "RakPeer.h"
#include <string.h>
#include "LinuxStrings.h"
#include "miniupnpc/miniupnpc.h"
#include "miniupnpc/upnperrors.h"
#include "miniupnpc/upnpcommands.h"
using namespace RakNet;

// Function Declarations
bool isLocalIP(const std::string&);

// State Definitions
enum class UPnPMappingState : int {
	INITIAL_REQUEST_ATTEMPT,
	FIND_EXISTING_ENTRY,
	ADD_ANY_PORT_MAPPING_NON_STANDARD,
	ADD_PORT_MAPPING_WILDCARD_PORT,
	ADD_PORT_MAPPING,
	SELECT_PORT,
	SUCCESS,
	FAILURE,
	UNEXPECTED_ERROR,
	DONE,
};

enum class UPnPPortSelectionMethod : int {
	LINEAR_SAME_EXTERNAL_INTERNAL_PORT,
	LINEAR_EXTERNAL_PORT,
};

// Async Operation Args
struct UPNPCreatePortMappingArgs {
	char descriptor[256];
	unsigned short portToOpen;
	bool bIPv6;
	unsigned int timeout;
	UPNPResultCallback userOnCompletedCallback;
};

struct UPNPGetExternalIPv4Args {
	unsigned int timeout;
	UPNPResultCallback userOnCompletedCallback;
};

// Asynchronous UPnP Operations
RAK_THREAD_DECLARATION(UPNPCreatePortMapping) {
	UPNPCreatePortMappingArgs *args = (UPNPCreatePortMappingArgs *)arguments;
	
	char externalIPAddress[MAXIMUM_HOSTNAME_LENGTH];
	memset(externalIPAddress, 0, sizeof(externalIPAddress));
	
	UPNPResult result;
	UPNPPortMappingResult& portMapping = result.portMapping;
	portMapping.bUPNPSupported = false;
	portMapping.internalPort = args->portToOpen;

	// If IPv6, the upnpDiscover command should ping the SSDP service over IPv6 and for the proper services.
	struct UPNPDev * devlist = 0;
	devlist = upnpDiscover(args->timeout, 0, 0, 0, args->bIPv6 ? 1 : 0, 0);
	
	if (devlist) {
		RAKNET_DEBUG_PRINTF("List of UPNP devices found on the network :\n");

		struct UPNPDev * device;
		for (device = devlist; device; device = device->pNext) {
			RAKNET_DEBUG_PRINTF(" desc: %s\n st: %s\n", device->descURL, device->st);
		}

		char lanaddr[64];	/* my ip address on the LAN */
		struct UPNPUrls urls;
		struct IGDdatas data;

		if (UPNP_GetValidIGD(devlist, &urls, &data, lanaddr, sizeof(lanaddr)) == 1) {
			// The maximum number of port selection attempts allowed.
			const int maxPortSelectionAttempts = 20;

			// Constant values to use when attempting to add a port mapping in UPNP.
			const char protocolToMap[4] = "UDP";
			const char infiniteLeaseDuration[2] = "0";
			const char *anyRemoteAddress = nullptr;

			// The internal port requesting to be mapped in UPnP table.
			char internalPortStr[MAXIMUM_PORTNAME_LENGTH];
			sprintf(internalPortStr, "%d", args->portToOpen);

			// The external port requesting to be mapped in UPnP table.
			char externalPortStr[MAXIMUM_PORTNAME_LENGTH];
			strcpy(externalPortStr, internalPortStr);

			// Used in linear port selection when the requested port is already mapped and can't be used.
			int portOffset = 0;

			// The return value from UPNP command calls, saved in this scope to be used in error reporting.
			int commandReturn = 0;

			// Initialize state machine states.
			UPnPMappingState upnpState = UPnPMappingState::INITIAL_REQUEST_ATTEMPT;
			UPnPPortSelectionMethod selectionMethod = UPnPPortSelectionMethod::LINEAR_EXTERNAL_PORT;
				
			// @note: Not all NAT implementation will support...
			//			- dynamic port mappings. So the following defaults to static port mappings. A static port mapping has a lease duration of 0 (infinite).
			//			- internal port values that are different than external port values. (Defined here as a moderate NAT).
			//			- wildcard value for external port (0).
			// @ref:  http://www.upnp.org/specs/gw/UPnP-gw-WANIPConnection-v1-Service.pdf #2.4.16

			while (upnpState != UPnPMappingState::DONE) {
				switch (upnpState) {
				case UPnPMappingState::INITIAL_REQUEST_ATTEMPT: {
					// Initial attempt to add the port mapping requested.
					char reservedPort[MAXIMUM_PORTNAME_LENGTH];
					commandReturn = UPNP_AddPortMapping(urls.controlURL, data.first.servicetype, externalPortStr, internalPortStr, lanaddr, args->descriptor, protocolToMap, anyRemoteAddress, infiniteLeaseDuration, reservedPort);
					if (commandReturn == UPNPCOMMAND_SUCCESS) {
						strcpy(externalPortStr, reservedPort);
						upnpState = UPnPMappingState::SUCCESS;
					}
					else {
						// Fallback to finding the best method for adding a UPnP port mapping.
						upnpState = UPnPMappingState::FIND_EXISTING_ENTRY;
					}
					break;
				}
				case UPnPMappingState::FIND_EXISTING_ENTRY: {
					RAKNET_DEBUG_PRINTF("Searching UPnP table for existing entry.\n");

					upnpState = UPnPMappingState::ADD_ANY_PORT_MAPPING_NON_STANDARD;

					char mappedInternalPort[6];
					char mappedExternalPort[6];
					char mappedInternalLanAddress[16];
					char mappedProtocol[4];
					char mappedLeaseDuration[16];
					char mappedDescription[80];
					char mappedEnabled[4];
					char mappedRemoteHost[64];

					bool found = false;
					char entryCountStr[4];
					int entryCount = 0;
					sprintf(entryCountStr, "%d", entryCount);
					memset(mappedProtocol, 0, sizeof(mappedProtocol));
					// Iterate through the upnp port mapping entries
					while (!found && UPNP_GetGenericPortMappingEntry(urls.controlURL, data.first.servicetype, entryCountStr, mappedExternalPort, mappedInternalLanAddress, mappedInternalPort, mappedProtocol, mappedDescription, mappedEnabled, mappedRemoteHost, mappedLeaseDuration) == UPNPCOMMAND_SUCCESS) {

						// Find any mapping that maps to this host and for the specified transmission protocol (UDP).
						if (strcmp(mappedInternalLanAddress, lanaddr) == 0 && _stricmp(mappedProtocol, protocolToMap) == 0) {
							// If the entry was not created by this app, then we shouldn't use it because we wouldn't be exclusive to it.
							if (_stricmp(mappedDescription, args->descriptor) == 0) {
								// Use the mapped internal and external ports of this entry.
								strcpy(externalPortStr, mappedExternalPort);
								strcpy(internalPortStr, mappedInternalPort);
								upnpState = UPnPMappingState::SUCCESS;
								found = true;
							}
							else if (strcmp(mappedInternalPort, internalPortStr) == 0) {
								// An entry exists that this app didn't create and maps to the internal port we want to use.
								// Need to choose a different internal port.
								upnpState = UPnPMappingState::SELECT_PORT;
								selectionMethod = UPnPPortSelectionMethod::LINEAR_SAME_EXTERNAL_INTERNAL_PORT;
								break;
							}
						}
						sprintf(entryCountStr, "%d", ++entryCount);
					}
					break;
				} // UPnPMappingState::FIND_EXISTING_ENTRY
				case UPnPMappingState::SELECT_PORT: {

					// Increment the port count offset
					portOffset++;

					// Exceeded the number of allowed attempts...fail UPnP.
					if (portOffset > maxPortSelectionAttempts) {
						upnpState = UPnPMappingState::FAILURE;

						// no need to proceed, break out of the case
						break;
					}
					else {
						upnpState = UPnPMappingState::ADD_PORT_MAPPING;
					}

					// If the selection methods enum were to expand beyond the linear methods, then use a switch on selectionMethod instead.

					// No matter the selection method (Linear same external internal port | linear same external port)
					// the external port should be changed.

					// The default port for IPv4 is less than the default port for IPv6, to prevent conflicts in bindings on non dual-stack ip environments, decrement the port offset on IPv4 and increment on IPv6.
					int newPort = args->bIPv6 ? args->portToOpen + portOffset : args->portToOpen - portOffset;
					sprintf(externalPortStr, "%d", newPort);

					// If the internal port needs to change too, then do it in accordance with the external port.
					if (selectionMethod == UPnPPortSelectionMethod::LINEAR_SAME_EXTERNAL_INTERNAL_PORT) {
						sprintf(internalPortStr, "%d", newPort);
					}
					break;
				} // UPnPMappingState::SELECT_PORT
				case UPnPMappingState::ADD_ANY_PORT_MAPPING_NON_STANDARD: {
					RAKNET_DEBUG_PRINTF("Attempt non-standard mapping for any external port.\n");

					char reservedPort[MAXIMUM_PORTNAME_LENGTH];
					// Attempt to use a non-standard IGD action for adding the specified external port. If it's not free use any free port.
					commandReturn = UPNP_AddAnyPortMapping(urls.controlURL, data.first.servicetype, externalPortStr, internalPortStr, lanaddr, args->descriptor, protocolToMap, anyRemoteAddress, infiniteLeaseDuration, reservedPort);
					if (commandReturn == UPNPCOMMAND_SUCCESS) {
						strcpy(externalPortStr, reservedPort);
						upnpState = UPnPMappingState::SUCCESS;
					}
					else {
						// An error code of 401 means "Invalid Action": Indicated that AddAnyPortMapping not supported by the router firmware (vendor-dependent).
						// But regardless of the error, the official standard process should be followed. Any other specific errors will trickle down.
						upnpState = UPnPMappingState::ADD_PORT_MAPPING_WILDCARD_PORT;
					}
					break;
				} // UPnPMappingState::ADD_ANY_PORT_MAPPING_NON_STANDARD
				case UPnPMappingState::ADD_PORT_MAPPING_WILDCARD_PORT: {
					sprintf(externalPortStr, "%d", 0);
					upnpState = UPnPMappingState::ADD_PORT_MAPPING;
					break;
				} // UPnPMappingState::ADD_PORT_MAPPING_WILDCARD_PORT
				case UPnPMappingState::ADD_PORT_MAPPING: {
					RAKNET_DEBUG_PRINTF("Attempt to add port mapping for eport(%s) & iport(%s).\n", externalPortStr, internalPortStr);

					char reservedPort[MAXIMUM_PORTNAME_LENGTH];
					commandReturn = UPNP_AddPortMapping(urls.controlURL, data.first.servicetype, externalPortStr, internalPortStr, lanaddr, args->descriptor, protocolToMap, anyRemoteAddress, infiniteLeaseDuration, reservedPort);
					switch (commandReturn) {
					case UPNPCOMMAND_SUCCESS: {
						strcpy(externalPortStr, reservedPort);
						upnpState = UPnPMappingState::SUCCESS;
						break;
					} // UPNPCOMMAND_SUCCESS
					// Error: The source IP address cannot be wild-carded.
					case 715: {
						// This should never be the case...
						upnpState = UPnPMappingState::UNEXPECTED_ERROR;
						break;
					} // error: 715
					// Error: The external port cannot be wild-carded 
					case 716: {
						// Fallback to our own method of port selection.
						upnpState = UPnPMappingState::SELECT_PORT;
						selectionMethod = UPnPPortSelectionMethod::LINEAR_EXTERNAL_PORT;
						break;
					} // error: 716
					// Error: The port mapping entry specified conflicts with a mapping assigned previously to another client.
					case 718: {
						// This would never hit as a result of a conflict of internal port used while attempting a wild-carded external port.
						// So this is the result of an add processed through custom port selection. Continue attempting port selection...
						upnpState = UPnPMappingState::SELECT_PORT;
						break;
					}
					// Error: Different Internal and External port settings are not supported.
					case 724: {
						// We have to fallback to port selection traversal where the internal and external are the same.
						// We know by now that at least the requested internal/external pairing (which would've been the same) didn't work.
						upnpState = UPnPMappingState::SELECT_PORT;
						selectionMethod = UPnPPortSelectionMethod::LINEAR_SAME_EXTERNAL_INTERNAL_PORT;
						break;
					} // error: 724
					// Error: The NAT implementation only supports permanent lease times on port mappings 
					case 725: {
						// This should never be the case...we only perform static mappings. i.e. lease duration of 0.
						upnpState = UPnPMappingState::UNEXPECTED_ERROR;
						break;
					} // error: 725
					// Error: RemoteHost must be a wildcard and cannot be a specific IP address or DNS name 
					case 726: {
						// This should never be the case...the request is always a wild-carded remote host.
						upnpState = UPnPMappingState::UNEXPECTED_ERROR;
						break;
					} // error: 726
					// Error: ExternalPort must be a wildcard and cannot be a specific port value 
					case 727: {
						// This should never be the case...we try wild-carded external port before attempting a specific one.
						upnpState = UPnPMappingState::UNEXPECTED_ERROR;
						break;
					} // error: 727
					// Error: Invalid Arguments
					case 402: {
						upnpState = UPnPMappingState::UNEXPECTED_ERROR;
						break;
					} // error: 402
					// Error: Action Failed
					case 501: {
						upnpState = UPnPMappingState::UNEXPECTED_ERROR;
						break;
					} // error: 501
					// Any other error that's not documented by the spec.
					default: {
						upnpState = UPnPMappingState::UNEXPECTED_ERROR;
						break;
					} // default
					} // end of switch

					break;
				} // UPnPMappingState::ADD_PORT_MAPPING
				case UPnPMappingState::SUCCESS: {
					// Only get the external IP for IPv4. NAT isn't used in IPv6 so there's no need to retrieve an external IP.
					if (args->bIPv6) {
						// Instead for IPv6, we use the "LAN" address.
						strncpy(portMapping.hostIPAddress, lanaddr, 64);
					}
					else {
						UPNP_GetExternalIPAddress(urls.controlURL, data.first.servicetype, externalIPAddress);
						strncpy(portMapping.hostIPAddress, externalIPAddress, MAXIMUM_HOSTNAME_LENGTH);
					}
						
					RAKNET_DEBUG_PRINTF("Success, mapping added for eport(%s) & iport(%s).\n", externalPortStr, internalPortStr);

					portMapping.bUPNPSupported = true;
					portMapping.internalPort = (unsigned short)atoi(internalPortStr);
					portMapping.externalPort = (unsigned short)atoi(externalPortStr);

					upnpState = UPnPMappingState::DONE;
					break;
				} // UPnPMappingState::SUCCESS
				case UPnPMappingState::FAILURE: {
					RAKNET_DEBUG_PRINTF("UPNP Port Mapping Failure.\n");

					portMapping.bUPNPSupported = false;
					portMapping.internalPort = 0;
					portMapping.externalPort = 0;

					upnpState = UPnPMappingState::DONE;
					break;
				} // UPnPMappingState::FAILURE
				case UPnPMappingState::UNEXPECTED_ERROR: {
					RAKNET_DEBUG_PRINTF("Unexpected error occurred, %s.\n", strupnperror(commandReturn));

					portMapping.bUPNPSupported = false;
					portMapping.internalPort = 0;
					portMapping.externalPort = 0;

					upnpState = UPnPMappingState::DONE;
					break;
				} // UPnPMappingState::UNEXPECTED_ERROR
				default: break;
				} // End of switch
			} // End of while loop
		}
	}

	if (!args->bIPv6) {
		// If the mapping was successful, check that we can reach the internet by determining if we are behind 2 routers.
		if (portMapping.bUPNPSupported) {
			portMapping.bUPNPSupported = !isLocalIP(std::string(portMapping.hostIPAddress));
		}
	}

	// Calls a user callback to handle the result.
	if (args->userOnCompletedCallback) {
		args->userOnCompletedCallback(result);
	}

	RakNet::OP_DELETE(args, _FILE_AND_LINE_);

	return 0;
 }

 RAK_THREAD_DECLARATION(UPNPGetExternalIPv4) {
	 UPNPGetExternalIPv4Args *args = (UPNPGetExternalIPv4Args *)arguments;

	 UPNPResult result;
	 UPNPExternalIPResult& externalIP = result.externalIP;
	 externalIP.bUPNPSupported = false;

	 struct UPNPDev * devlist = 0;
	 devlist = upnpDiscover(args->timeout, 0, 0, 0, 0, 0);

	 if (devlist) {
		 RAKNET_DEBUG_PRINTF("List of UPNP devices found on the network :\n");

		 struct UPNPDev * device;
		 for (device = devlist; device; device = device->pNext) {
			 RAKNET_DEBUG_PRINTF(" desc: %s\n st: %s\n", device->descURL, device->st);
		 }

		 char lanaddr[64];	/* my ip address on the LAN */
		 struct UPNPUrls urls;
		 struct IGDdatas data;

		 if (UPNP_GetValidIGD(devlist, &urls, &data, lanaddr, sizeof(lanaddr)) == 1) {
			 int retValue = UPNP_GetExternalIPAddress(urls.controlURL, data.first.servicetype, externalIP.hostIPAddress);
			 externalIP.bUPNPSupported = (retValue == UPNPCOMMAND_SUCCESS);
		 }
	 }

	 // If the retreival was successful, check that we can reach the internet by determining if we are behind 2 routers.
	 if (externalIP.bUPNPSupported) {
		 externalIP.bUPNPSupported = !isLocalIP(std::string(externalIP.hostIPAddress));
	 }
	 else {
		 // If the command wasn't successful, zero out the external IP sent to the result callback to indicate failure.
		 memset(externalIP.hostIPAddress, 0, sizeof(externalIP.hostIPAddress));
	 }

	 // Calls a user callback to handle the result.
	 if (args->userOnCompletedCallback) {
		 args->userOnCompletedCallback(result);
	 }

	 RakNet::OP_DELETE(args, _FILE_AND_LINE_);

	 return 0;
 }

 bool isLocalIP(const std::string& ip) {
	 // we need to check and see if this IP address is from one of the private ranges 
	 // if it's private, that means there are two or more IGDs (NAT devices / routers)  
	 // between this PC and the internet.   
	 // 
	 // in that case, we can't treat this like a successful UPNP, since only one of the 
	 // routers has been configured, and we still might not have a route configured 
	 // from the internet 

	 const std::string prefix1 = "10.";			// 10.0.0.0		- 10.255.255.255 
	 const std::string prefix2 = "192.168.";	// 192.168.0.0	- 192.168.255.255 
	 const std::string prefix3 = "172.";		// 172.16.0.0	- 172.31.255.255 

	 if (ip.substr(0, prefix1.size()) == prefix1 ||
		 ip.substr(0, prefix2.size()) == prefix2) {
		 // this is definitely a private address 
		 return true;
	 }
	 // if it's a 172.* address, we have to check the value of the class B network 
	 else if (ip.substr(0, prefix3.size()) == prefix3
		 && ip[6] == '.') {
		 // might be private, have to check second set of digits 
		 std::string mid = ip.substr(4, 2);
		 int classB = atoi(mid.c_str());

		 if (classB >= 16 && classB <= 31) {
			 // this is a private address 
			 return true;
		 }
	 }
	 return false;
 }

void UPNPCreatePortMappingAsync(unsigned short portToOpen,
	unsigned int timeout,
	const char* descriptor,
	bool isIPv6,
	UPNPResultCallback onCompleted) {

	UPNPCreatePortMappingArgs *args = RakNet::OP_NEW<UPNPCreatePortMappingArgs>(_FILE_AND_LINE_);
	args->portToOpen = portToOpen;
	args->timeout = timeout;
	args->bIPv6 = isIPv6;
	args->userOnCompletedCallback = onCompleted;
	strcpy(args->descriptor, descriptor);

	RakThread::Create(UPNPCreatePortMapping, args);
}

void UPNPGetExternalIPAsync(unsigned int timeout,
	UPNPResultCallback onCompleted) {

	UPNPGetExternalIPv4Args *args = RakNet::OP_NEW<UPNPGetExternalIPv4Args>(_FILE_AND_LINE_);
	args->timeout = timeout;
	args->userOnCompletedCallback = onCompleted;
	
	RakThread::Create(UPNPGetExternalIPv4, args);
}
#else
void UPNPCreatePortMappingAsync(unsigned short portToOpen,
	unsigned int timeout,
	const char* descriptor,
	bool isIPv6,
	UPNPResultCallback onCompleted) {

	UPNPResult result;
	result.portMapping.externalPort = portToOpen;
	result.portMapping.bUPNPSupported = false;

	if (onCompleted) {
		onCompleted(result);
	}
}

void UPNPGetExternalIPAsync(unsigned int timeout,
	UPNPResultCallback onCompleted) {

	UPNPResult result;
	result.externalIP.bUPNPSupported = false;

	if (onCompleted) {
		onCompleted(result);
	}
}
#endif