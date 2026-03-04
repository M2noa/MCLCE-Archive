

#pragma once


#pragma once

class TLSStoragePSVita
{
	static TLSStoragePSVita* m_pInstance;

	static const int	sc_maxSlots = 64;
	static BOOL 		m_activeList[sc_maxSlots];
	static __thread LPVOID		m_values[sc_maxSlots];

public:
	TLSStoragePSVita();

	// Retrieve singleton instance.
	static TLSStoragePSVita* Instance();
	int Alloc();
	BOOL Free(DWORD _index);
	BOOL SetValue(DWORD _index, LPVOID _val);
	LPVOID GetValue(DWORD _index);
};