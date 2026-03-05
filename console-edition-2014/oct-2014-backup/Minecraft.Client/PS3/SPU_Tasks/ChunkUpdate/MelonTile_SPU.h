#pragma once
#include "Tile_SPU.h"


class MelonTile_SPU : public Tile_SPU
{
public:
	MelonTile_SPU(int id) : Tile_SPU(id) {}
	virtual Icon_SPU *getTexture(int face, int data)
	{
		if (face == Facing::UP || face == Facing::DOWN) return &ms_pTileData->melonTile_iconTop;
		return icon();
	}
};