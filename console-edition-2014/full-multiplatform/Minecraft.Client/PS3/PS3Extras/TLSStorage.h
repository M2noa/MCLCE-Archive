

#pragma once


#pragma once

class TLSStoragePS3
{
	static TLSStoragePS3* m_pInstance;

	static const int	sc_maxSlots = 64;
	static BOOL 		m_activeList[sc_maxSlots];
	static __thread LPVOID		m_values[sc_maxSlots];

public:
	TLSStoragePS3();

	// Retrieve singleton instance.
	static TLSStoragePS3* Instance();
	int Alloc();
	BOOL Free(DWORD _index);
	BOOL SetValue(DWORD _index, LPVOID _val);
	LPVOID GetValue(DWORD _index);
};