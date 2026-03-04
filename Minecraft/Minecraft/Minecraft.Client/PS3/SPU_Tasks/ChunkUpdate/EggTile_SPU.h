#pragma once
#include "Tile_SPU.h"

class EggTile_SPU : public Tile_SPU
{
public:
	EggTile_SPU(int id) : Tile_SPU(id) {}
    virtual bool blocksLight() { return false; }
    virtual bool isSolidRender(bool isServerLevel = false) { return false; }
	virtual int getRenderShape() { return Tile_SPU::SHAPE_EGG; }
	virtual bool shouldRenderFace(ChunkRebuildData *level, int x, int y, int z, int face) { return true; }

};