#pragma once



class RenderChunksSpuDataIn
{
public:
	CellGcmContextData m_PPUGcmContext;
	int m_numLists;
	int m_padding[3];
	int m_listArray[8000];
};

class RenderChunksSpuDataOut
{
public:
	uint32_t m_commandBuffer[16*1024];  // 16*4 Kb
	int m_commandBufferSize;
};
