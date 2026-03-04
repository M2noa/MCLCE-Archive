#pragma once

#include "Tile_SPU.h"


class ReedTile_SPU : public Tile_SPU
{
public:
	ReedTile_SPU(int id) : Tile_SPU(id) {}

	bool blocksLight() { return false; }
	bool isSolidRender(bool isServerLevel = false) { return false; }
	bool isCubeShaped() { return false; }
	int getRenderShape() { return Tile_SPU::SHAPE_CROSS_TEXTURE; }
};