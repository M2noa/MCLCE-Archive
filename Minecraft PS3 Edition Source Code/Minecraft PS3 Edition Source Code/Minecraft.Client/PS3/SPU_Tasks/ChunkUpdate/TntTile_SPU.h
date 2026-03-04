#pragma once

#include "Tile_SPU.h"

class TntTile_SPU : public Tile_SPU
{
public:
	static const int EXPLODE_BIT = 1;
	TntTile_SPU(int id) : Tile_SPU(id) {}

	Icon_SPU *getTexture(int face, int data)
	{
		if (face == Facing::DOWN) return &ms_pTileData->tntTile_iconBottom;
		if (face == Facing::UP) return &ms_pTileData->tntTile_iconTop;
		return icon();

	}
};