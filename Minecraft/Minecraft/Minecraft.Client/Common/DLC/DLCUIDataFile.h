#pragma once
#include "DLCFile.h"

class DLCUIDataFile : public DLCFile
{
private:
	PBYTE m_pbData;
	DWORD m_dwBytes;
	bool m_canDeleteData;

public:
	DLCUIDataFile(const wstring &path);
	~DLCUIDataFile();

	using DLCFile::addData;
	using DLCFile::addParameter;

	virtual void addData(PBYTE pbData, DWORD dwBytes,bool canDeleteData = false);
	virtual PBYTE getData(DWORD &dwBytes);
};
