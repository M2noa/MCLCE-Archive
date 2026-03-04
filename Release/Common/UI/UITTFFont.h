#pragma once

class UITTFFont
{
private:
	PBYTE pbData;
	//DWORD dwDataSize;

public:
	UITTFFont(const string &path, S32 fallbackCharacter);
	~UITTFFont();
};
