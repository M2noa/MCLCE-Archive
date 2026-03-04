#pragma once
#include "Tile_SPU.h"

class SmoothStoneBrickTile_SPU : public Tile_SPU
{
public:
	static const int TYPE_DEFAULT = 0;
    static const int TYPE_MOSSY = 1;
    static const int TYPE_CRACKED = 2;
	static const int TYPE_DETAIL = 3;

	static const int SMOOTH_STONE_BRICK_NAMES_LENGTH = 4;


public:

    SmoothStoneBrickTile_SPU(int id) : Tile_SPU(id) {}

	virtual Icon_SPU *getTexture(int face, int data)
	{
		if (data < 0 || data >= SMOOTH_STONE_BRICK_NAMES_LENGTH) data = 0;
		return &ms_pTileData->smoothStoneBrick_icons[data];
	}

};