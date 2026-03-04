#pragma once
#include "Tile_SPU.h"


class LadderTile_SPU : public Tile_SPU
{
public:
	LadderTile_SPU(int id) : Tile_SPU(id) {}
    virtual bool blocksLight() { return false; }
    virtual bool isSolidRender(bool isServerLevel = false) { return false; }
    virtual bool isCubeShaped() { return false; }
    virtual int getRenderShape() { return Tile_SPU::SHAPE_LADDER; }
};