#pragma once
#include "Tile_SPU.h"

class HalfTransparentTile_SPU : public Tile_SPU
{
public:
	HalfTransparentTile_SPU(int id) : Tile_SPU(id) {} 
    virtual bool isSolidRender(bool isServerLevel = false) { return false; }
    virtual bool shouldRenderFace(ChunkRebuildData *level, int x, int y, int z, int face) = 0;	// make sure the inherited class defines this, as it needs some storage (allowSame)
	virtual bool blocksLight() { return false; }
};
