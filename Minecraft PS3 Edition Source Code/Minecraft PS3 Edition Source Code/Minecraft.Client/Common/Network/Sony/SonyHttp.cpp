#include "stdafx.h"
#include "SonyHttp.h"


#if defined(__PS3__)
#include "PS3\Network\SonyHttp_PS3.h"
SonyHttp_PS3 g_SonyHttp;

#endif



bool SonyHttp::init()
{
	return g_SonyHttp.init();
}

void SonyHttp::shutdown()
{
	g_SonyHttp.shutdown();
}

bool SonyHttp::getDataFromURL(const char* szURL, void** ppOutData, int* pDataSize)
{
	return g_SonyHttp.getDataFromURL(szURL, ppOutData, pDataSize);
}
