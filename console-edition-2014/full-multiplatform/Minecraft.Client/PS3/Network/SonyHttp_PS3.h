#pragma once

#include <cell/http.h>


class SonyHttp_PS3
{
	static bool loadCerts(size_t *numBufPtr, CellHttpsData **caListPtr);
	static void* getData(const char* url, int* pDataSize);
	static bool parseUri(const char* szUri, CellHttpUri& parsedUri);

	static void *uriPool;
	static void *httpPool;
	static void* sslPool;
	static void* cookiePool;
	static CellHttpClientId client;
	static CellHttpTransId trans;
	static bool bInitialised;

public:
	bool init();
	void shutdown();
	bool getDataFromURL(const char* szURL, void** ppOutData, int* pDataSize);

};