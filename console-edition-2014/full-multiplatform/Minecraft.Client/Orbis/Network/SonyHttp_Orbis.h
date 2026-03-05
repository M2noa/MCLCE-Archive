#pragma once

#include <libhttp.h>

class SonyHttp_Orbis
{
	static SceInt32 sslCallback(int libsslCtxId,unsigned int verifyErr,SceSslCert * const sslCert[],int certNum,void *userArg);
	static bool http_get(const char *targetUrl, void** ppOutData, int* pDataSize);
	static bool http_get_close(bool bOK, SceInt32 tmplId, SceInt32 connId, SceInt32 reqId);

	static void printSslError(SceInt32 sslErr, SceUInt32 sslErrDetail);
	static void printSslCertInfo(int libsslCtxId,SceSslCert *sslCert);

	static int libnetMemId;
	static int libsslCtxId;
	static int libhttpCtxId;

	static bool bInitialised;

public:
	bool init();
	void shutdown();
	bool getDataFromURL(const char* szURL, void** ppOutData, int* pDataSize);

	static int getHTTPContextID() { return libhttpCtxId; }
};