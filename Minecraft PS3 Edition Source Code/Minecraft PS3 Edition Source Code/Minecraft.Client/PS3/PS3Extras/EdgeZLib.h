#pragma once
#include <cell/spurs.h>

class EdgeZLib
{
	static void InitDeflate();
	static void InitInflate();
public:	
	static void Init(CellSpurs* pSpurs);
	static void Shutdown();
	static bool Compress(void* pDestination, uint32_t* pDestSize, const void* pSource, uint32_t SrcSize);
	static bool Decompress(void* pDestination, uint32_t* pDestSize, const void* pSource, uint32_t SrcSize);
};