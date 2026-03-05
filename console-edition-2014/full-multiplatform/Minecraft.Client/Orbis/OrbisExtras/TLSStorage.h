#pragma once

class TLSStorageOrbis
{
	static TLSStorageOrbis* m_pInstance;

	static const int	sc_maxSlots = 64;
	static BOOL 		m_activeList[sc_maxSlots];
	__thread static LPVOID	m_values[sc_maxSlots];

public:
	TLSStorageOrbis();

	// Retrieve singleton instance.
	static TLSStorageOrbis* Instance();
	int Alloc();
	BOOL Free(DWORD _index);
	BOOL SetValue(DWORD _index, LPVOID _val);
	LPVOID GetValue(DWORD _index);
};