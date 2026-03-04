#pragma once



class Renderer_TextureUpdate_DataIn
{
public:
	int xoffset;
	int yoffset;
	int width;
	int height;
	int pitch;
	void* pSrcData;
	void* pDstData;
	int level;
};
