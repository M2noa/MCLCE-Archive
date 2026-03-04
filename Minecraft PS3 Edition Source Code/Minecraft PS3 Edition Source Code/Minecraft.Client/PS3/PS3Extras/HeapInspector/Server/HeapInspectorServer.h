#ifndef _HEAPINSPECTORSERVER_PLATFORMINTERFACES_HEAPINSPECTORSERVER_H_
#define _HEAPINSPECTORSERVER_PLATFORMINTERFACES_HEAPINSPECTORSERVER_H_

#include "HeapInfo.h"
#include <vector>

BEGIN_NAMESPACE(HeapInspectorServer)

enum EWaitForConnection
{
	WaitForConnection_Disabled,
	WaitForConnection_Enabled
};

std::vector<HeapInfo> GetDefaultHeapInfo();
bool Initialise(const std::vector<HeapInfo>& a_HeapInfo, int a_Port, EWaitForConnection a_WaitForConnection);
bool IsInitialised();
void Shutdown();

typedef int Mutation;

Mutation BeginAlloc();
void EndAlloc(Mutation a_Mutation, uint16 a_HeapID, void* a_Address, uint32 a_SizeRequested, uint32 a_SizeReceived);

Mutation BeginReAlloc();
void EndReAlloc(Mutation a_Mutation, uint16 a_HeapID, void* a_OldAddress, void* a_NewAddress, uint32 a_SizeRequested, uint32 a_SizeReceived);

Mutation BeginFree();
void EndFree(Mutation a_Mutation, uint16 a_HeapID, void* a_Address);

END_NAMESPACE(HeapInspectorServer)

#endif // _HEAPINSPECTORSERVER_PLATFORMINTERFACES_HEAPINSPECTORSERVER_H_