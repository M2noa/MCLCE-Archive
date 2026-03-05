#pragma once
#include "HalfTransparentTile_SPU.h"

class Random;

class IceTile_SPU : public HalfTransparentTile_SPU
{
public:
	IceTile_SPU(int id) : HalfTransparentTile_SPU(id) {}
	virtual int getRenderLayer() { return 1; }
	virtual bool shouldRenderFace(ChunkRebuildData *level, int x, int y, int z, int face) 
	{ 	
		int id = level->getTile(x, y, z);
		if (!ms_pTileData->iceTile_allowSame && id == this->id) return false;
		return Tile_SPU::shouldRenderFace(level, x, y, z, 1 - face);
	}
};
