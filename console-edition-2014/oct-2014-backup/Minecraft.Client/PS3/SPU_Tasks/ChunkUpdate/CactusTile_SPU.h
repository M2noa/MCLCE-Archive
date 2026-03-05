#pragma once
#include "Tile_SPU.h"

class CactusTile_SPU : public Tile_SPU
{
public:
	CactusTile_SPU(int id) : Tile_SPU(id) {}

	virtual Icon_SPU *getTexture(int face, int data)
	{
		if (face == Facing::UP) return &ms_pTileData->cactusTile_iconTop;
		if (face == Facing::DOWN) return &ms_pTileData->cactusTile_iconBottom;
		else return icon();
	}
	virtual bool isCubeShaped() { return false; }
	virtual bool isSolidRender(bool isServerLevel = false) { return false;}
	virtual int getRenderShape() { return Tile_SPU::SHAPE_CACTUS; }
};