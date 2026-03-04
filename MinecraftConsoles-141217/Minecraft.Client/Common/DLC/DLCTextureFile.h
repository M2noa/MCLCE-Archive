#pragma once
#include "DLCFile.h"

class DLCTextureFile : public DLCFile
{

private:
	bool m_bIsAnim;
	wstring m_animString;

	PBYTE m_pbData;
	DWORD m_dwBytes;

public:
	DLCTextureFile(const wstring &path);

	virtual void addData(PBYTE pbData, DWORD dwBytes);
	virtual PBYTE getData(DWORD &dwBytes);

	virtual void addParameter(DLCManager::EDLCParameterType type, const wstring &value);

	virtual wstring getParameterAsString(DLCManager::EDLCParameterType type);
	virtual bool getParameterAsBool(DLCManager::EDLCParameterType type);
};