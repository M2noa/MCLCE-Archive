#pragma once
#include "Tile_SPU.h"

class Bush_SPU : public Tile_SPU
{
public:
	Bush_SPU(int id) : Tile_SPU(id) {}

	virtual bool blocksLight() { return false; }
	virtual bool isSolidRender(bool isServerLevel = false) { return false; }
	virtual int getRenderShape() { 	return Tile_SPU::SHAPE_CROSS_TEXTURE; }
};
