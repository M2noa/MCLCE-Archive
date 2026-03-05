#include "stdafx.h"
#include "HttpTexture.h"

HttpTexture::HttpTexture(const wstring& _url, HttpTextureProcessor *processor)
{
	// 4J - added
    count = 1;
    id = -1;
    isLoaded = false;

	// 4J - TODO - actually implement
}