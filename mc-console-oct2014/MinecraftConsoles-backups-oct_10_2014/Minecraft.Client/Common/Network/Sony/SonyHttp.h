#pragma once



class SonyHttp
{
public:
	static bool init();
	static void shutdown();
	static bool getDataFromURL(const char* szURL, void** ppOutData, int* pDataSize);
};