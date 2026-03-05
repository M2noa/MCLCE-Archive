#pragma once
#include "Tile_SPU.h"


class TorchTile_SPU : public Tile_SPU
{
public:
	TorchTile_SPU(int id) : Tile_SPU(id) {}
    virtual bool isSolidRender(bool isServerLevel = false) { return false; }
    virtual bool isCubeShaped() { return false; }
    virtual int getRenderShape() { return Tile_SPU::SHAPE_TORCH;}
};
