#pragma once
#include "DLCFile.h"

class ColourTable;

class DLCColourTableFile : public DLCFile
{
private:
	ColourTable *m_colourTable;

public:
	DLCColourTableFile(const wstring &path);
	~DLCColourTableFile();

	virtual void addData(PBYTE pbData, DWORD dwBytes);

	ColourTable *getColourTable() { return m_colourTable; }
};