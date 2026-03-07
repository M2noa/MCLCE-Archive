/**
 * Windows64: fetch store files from localhost (Minecraft Store recreation server).
 * Replaces the TMSPP_ReadFile stub so the client loads DLC catalog from http://127.0.0.1:3000
 */

#include "stdafx.h"

#ifdef _WINDOWS64

#include <vector>
#include <winhttp.h>
#pragma comment(lib, "winhttp.lib")

#include "..\Common\Consoles_App.h"

#ifndef STORE_SERVER_PORT
#define STORE_SERVER_PORT 3000
#endif

#include "Windows64\4JLibs\inc\4J_Storage.h"
#pragma comment(linker, "/FORCE:MULTIPLE")


static bool HttpGetStoreFile(const char* szFilename, BYTE** ppOutData, DWORD* pdwSize)
{
	*ppOutData = NULL;
	*pdwSize = 0;

	WCHAR wzHost[] = L"127.0.0.1";
	WCHAR wzPath[512];
	_snwprintf_s(wzPath, _countof(wzPath), _TRUNCATE, L"/store/file/%S", szFilename ? szFilename : "");

	HINTERNET hSession = WinHttpOpen(L"Minecraft/1.0", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
	if (!hSession) return false;

	HINTERNET hConnect = WinHttpConnect(hSession, wzHost, STORE_SERVER_PORT, 0);
	if (!hConnect) { WinHttpCloseHandle(hSession); return false; }

	HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"GET", wzPath, NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, 0);
	if (!hRequest) { WinHttpCloseHandle(hConnect); WinHttpCloseHandle(hSession); return false; }

	BOOL bSent = WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, 0, 0);
	if (!bSent) { WinHttpCloseHandle(hRequest); WinHttpCloseHandle(hConnect); WinHttpCloseHandle(hSession); return false; }

	BOOL bReceived = WinHttpReceiveResponse(hRequest, NULL);
	if (!bReceived) { WinHttpCloseHandle(hRequest); WinHttpCloseHandle(hConnect); WinHttpCloseHandle(hSession); return false; }

	DWORD dwStatusCode = 0;
	DWORD dwSizeLen = sizeof(dwStatusCode);
	WinHttpQueryHeaders(hRequest, WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER, WINHTTP_HEADER_NAME_BY_INDEX, &dwStatusCode, &dwSizeLen, WINHTTP_NO_HEADER_INDEX);
	if (dwStatusCode != 200) { WinHttpCloseHandle(hRequest); WinHttpCloseHandle(hConnect); WinHttpCloseHandle(hSession); return false; }

	std::vector<BYTE> buf;
	DWORD dwDownloaded = 0;
	do {
		DWORD dwAvailable = 0;
		if (!WinHttpQueryDataAvailable(hRequest, &dwAvailable) || dwAvailable == 0) break;
		size_t oldSize = buf.size();
		buf.resize(oldSize + dwAvailable);
		if (!WinHttpReadData(hRequest, &buf[oldSize], dwAvailable, &dwDownloaded)) break;
		if (dwDownloaded < dwAvailable) buf.resize(oldSize + dwDownloaded);
	} while (dwDownloaded > 0);

	WinHttpCloseHandle(hRequest);
	WinHttpCloseHandle(hConnect);
	WinHttpCloseHandle(hSession);

	if (buf.empty()) return false;
	DWORD dwTotalSize = (DWORD)buf.size();
	BYTE* pData = new BYTE[dwTotalSize];
	memcpy(pData, buf.data(), dwTotalSize);
	*ppOutData = pData;
	*pdwSize = dwTotalSize;
	return true;
}

C4JStorage::ETMSStatus C4JStorage::TMSPP_ReadFile(
	int iPad,
	C4JStorage::eGlobalStorage eStorageFacility,
	C4JStorage::eTMS_FILETYPEVAL eFileTypeVal,
	LPCSTR szFilename,
	int(*Func)(LPVOID, int, int, C4JStorage::PTMSPP_FILEDATA, LPCSTR),
	LPVOID lpParam,
	int iUserData)
{
	BYTE* pData  = NULL;
	DWORD dwSize = 0;

	if (!HttpGetStoreFile(szFilename, &pData, &dwSize))
	{
		// Fire callback with failure (NULL data signals error to caller)
		if (Func)
			Func(lpParam, iPad, iUserData, NULL, szFilename);
		return C4JStorage::ETMSStatus_Fail;
	}

	// Build the TMSPP_FILEDATA the callback expects
	C4JStorage::TMSPP_FILEDATA fileData;
	fileData.pbData = pData;
	fileData.dwSize = dwSize;

	if (Func)
		Func(lpParam, iPad, iUserData, &fileData, szFilename);

	delete[] pData;
	return C4JStorage::ETMSStatus_Pending;
}

#endif // _WINDOWS64