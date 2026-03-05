#include "stdafx.h"
#include "TexturePack.h"

wstring TexturePack::getPath(bool bTitleUpdateTexture /*= false*/)
{
	wstring wDrive;

#if defined(__PS3__)

	// 4J-PB - we need to check for a BD patch - this is going to be an issue for full DLC texture packs (Halloween)

	char *pchUsrDir=getUsrDirPath();
	
	wstring wstr (pchUsrDir, pchUsrDir+strlen(pchUsrDir));

	if(bTitleUpdateTexture)
	{
		// Make the content package point to to the UPDATE: drive is needed
		wDrive= wstr + L"\\Common\\res\\TitleUpdate\\";
	}
	else
	{
		wDrive= wstr + L"/Common/";
	}			


#else
	if(bTitleUpdateTexture)
	{
		// Make the content package point to to the UPDATE: drive is needed
		wDrive=L"Common\\res\\TitleUpdate\\";
	}
	else
	{
		wDrive=L"Common/";
	}
#endif

	return wDrive;
}
