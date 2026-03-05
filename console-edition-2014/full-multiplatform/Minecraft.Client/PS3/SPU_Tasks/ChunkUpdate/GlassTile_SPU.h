#pragma once
#include "HalfTransparentTile_SPU.h"

class GlassTile_SPU : public HalfTransparentTile_SPU
{
public:
	GlassTile_SPU(int id) : HalfTransparentTile_SPU(id) {}

	virtual bool shouldRenderFace(ChunkRebuildData *level, int x, int y, int z, int face)	// from parent class, here because it needs storgage
	{
		int id = level->getTile(x, y, z);
		if (!ms_pTileData->glassTile_allowSame && id == this->id) return false;
		return Tile_SPU::shouldRenderFace(level, x, y, z, face);

	}

    virtual int getRenderLayer() { return 0; }
};