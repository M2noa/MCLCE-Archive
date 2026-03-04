#pragma once

#include "Tile_SPU.h"
#include "Facing_SPU.h"

class SandStoneTile_SPU : public Tile_SPU
{
public:
	static const int TYPE_DEFAULT = 0;
	static const int TYPE_HEIROGLYPHS = 1;
	static const int TYPE_SMOOTHSIDE = 2;

	// Add this in when we need it
	//static final String[] SANDSTONE_NAMES = {"default", "chiseled", "smooth"};

private:
	static const int SANDSTONE_TILE_TEXTURE_COUNT = 3;
public:
	SandStoneTile_SPU(int id) : Tile_SPU(id) {}

public:
	Icon_SPU *getTexture(int face, int data)
	{
		if (face == Facing::UP || (face == Facing::DOWN && (data == TYPE_HEIROGLYPHS || data == TYPE_SMOOTHSIDE)))
			return &ms_pTileData->sandStone_iconTop;
		if (face == Facing::DOWN)
			return &ms_pTileData->sandStone_iconBottom;
		if (data < 0 || data >= SANDSTONE_TILE_TEXTURE_COUNT) 
			data = 0;
		return &ms_pTileData->sandStone_icons[data];
	}
};