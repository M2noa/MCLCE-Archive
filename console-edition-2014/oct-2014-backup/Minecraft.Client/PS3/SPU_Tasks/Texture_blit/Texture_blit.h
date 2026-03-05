#pragma once



class Texture_blit_DataIn
{
public:
	uint8_t* pSrcData;
	uint8_t* pDstData;
	int yy;
	int xx;
	int hh;
	int ww;
	int shh;
	int sww;
	bool rotated;
	int pad[3];
};
