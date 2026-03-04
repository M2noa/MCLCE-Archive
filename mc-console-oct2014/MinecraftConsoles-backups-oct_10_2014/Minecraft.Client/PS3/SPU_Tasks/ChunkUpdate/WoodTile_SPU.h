#pragma once
#include "Tile_SPU.h"

class WoodTile_SPU : public Tile_SPU
{

public:

	static const int WOOD_NAMES_LENGTH = 4;

public:
	WoodTile_SPU(int id) : Tile_SPU(id) {}
	virtual Icon_SPU *getTexture(int face, int data)
	{
		if (data < 0 || data >= WOOD_NAMES_LENGTH)
			data = 0;
		return &ms_pTileData->woodTile_icons[data];
	}
};