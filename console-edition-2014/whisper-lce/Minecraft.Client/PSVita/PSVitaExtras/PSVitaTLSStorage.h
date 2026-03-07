#pragma once

class PSVitaTLSStorage
{
private:

public:
	static void Init();
	static PSVitaTLSStorage *Instance();

	static DWORD Alloc();
	static BOOL Free(DWORD dwTlsIndex);
	static void RemoveThread(int UniqueID);
	static LPVOID GetValue(DWORD dwTlsIndex);
	static BOOL SetValue(DWORD dwTlsIndex, LPVOID lpTlsValue);
};
