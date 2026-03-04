#pragma once

#include "Tile_SPU.h"

class WebTile_SPU : public Tile_SPU
{

public:
	WebTile_SPU(int id) : Tile_SPU(id) {}
	bool isSolidRender(bool isServerLevel = false) { return false;}
	int getRenderShape() { return Tile_SPU::SHAPE_CROSS_TEXTURE; }
	bool blocksLight() { return false;}
	bool isCubeShaped() { return false;}

};