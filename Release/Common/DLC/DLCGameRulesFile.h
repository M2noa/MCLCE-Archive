#pragma once
#include "DLCGameRules.h"

class DLCGameRulesFile : public DLCGameRules
{
private:
	PBYTE m_pbData;
	DWORD m_dwBytes;

public:
	DLCGameRulesFile(const wstring &path);

	virtual void addData(PBYTE pbData, DWORD dwBytes);
	virtual PBYTE getData(DWORD &dwBytes);
};