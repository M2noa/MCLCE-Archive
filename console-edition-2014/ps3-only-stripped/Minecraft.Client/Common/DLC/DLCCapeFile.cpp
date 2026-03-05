#include "stdafx.h"
#include "DLCManager.h"
#include "DLCCapeFile.h"

DLCCapeFile::DLCCapeFile(const wstring &path) : DLCFile(DLCManager::e_DLCType_Cape,path)
{
}

void DLCCapeFile::addData(PBYTE pbData, DWORD dwBytes)
{
	app.AddMemoryTextureFile(m_path,pbData,dwBytes);
}