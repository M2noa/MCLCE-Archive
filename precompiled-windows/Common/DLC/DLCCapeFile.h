#pragma once
#include "DLCFile.h"

class DLCCapeFile : public DLCFile
{
public:
	DLCCapeFile(const wstring &path);

	virtual void addData(PBYTE pbData, DWORD dwBytes);
};