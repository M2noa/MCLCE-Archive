

#include "stdafx.h"



TLSStoragePSVita* TLSStoragePSVita::m_pInstance = NULL;

BOOL TLSStoragePSVita::m_activeList[sc_maxSlots];
__thread LPVOID TLSStoragePSVita::m_values[sc_maxSlots];



TLSStoragePSVita::TLSStoragePSVita()
{
	for(int i=0;i<sc_maxSlots; i++)
	{
		m_activeList[i] = false;
		m_values[i] = NULL;
	}
}

TLSStoragePSVita* TLSStoragePSVita::Instance()
{
	if ( m_pInstance == 0 )			// Is this the first time?
	{
		m_pInstance = new TLSStoragePSVita;		// Create the singleton instance.
	}
	return m_pInstance;
}


int TLSStoragePSVita::Alloc()
{
	for(int i=0; i<sc_maxSlots; i++) 
	{
		if(m_activeList[i] == false)
		{
			m_activeList[i] = true;
			m_values[i] = NULL;
			return i;
		}
	}
	assert(0);		// we've ran out of slots
	return -1;
}

BOOL TLSStoragePSVita::Free( DWORD _index )
{
	if(m_activeList[_index] == false)
		return false; // not been allocated

	m_activeList[_index] = false;
	m_values[_index] = NULL;
	return true;
}

BOOL TLSStoragePSVita::SetValue( DWORD _index, LPVOID _val )
{
	if(m_activeList[_index] == false)
		return false;
	m_values[_index] = _val;
	return true;
}

LPVOID TLSStoragePSVita::GetValue( DWORD _index )
{
	if(m_activeList[_index] == false)
		return NULL;
	return m_values[_index];
}

