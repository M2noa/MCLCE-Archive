#include "stdafx.h"
#include "DLCFile.h"

DLCFile::DLCFile(DLCManager::EDLCType type, const wstring &path)
{
	m_type = type;
	m_path = path;

	// store the id
	bool dlcSkin = path.substr(0,3).compare(L"dlc") == 0;

	if(dlcSkin)
	{
		wstring skinValue = path.substr(7,path.size());
		skinValue = skinValue.substr(0,skinValue.find_first_of(L'.'));
		std::wstringstream ss;
		ss << std::dec << skinValue.c_str();
		ss >> m_dwSkinId;
		m_dwSkinId = MAKE_SKIN_BITMASK(true, m_dwSkinId);

	}
	else
	{
		m_dwSkinId=0;
	}
}