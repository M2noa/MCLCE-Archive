#pragma once

#include "Tile_SPU.h"

class Player;

class TreeTile_SPU : public Tile_SPU
{	
public:
	static const int DARK_TRUNK = 1;
	static const int BIRCH_TRUNK = 2;
	static const int JUNGLE_TRUNK = 3;

	static const int MASK_TYPE = 0x3;
	static const int MASK_FACING = 0xC;
	static const int FACING_Y = 0 << 2;
	static const int FACING_X = 1 << 2;
	static const int FACING_Z = 2 << 2;

	static const int TREE_NAMES_LENGTH = 4;

	TreeTile_SPU(int id) : Tile_SPU(id) {}
	int getRenderShape() {	return Tile_SPU::SHAPE_TREE; }


public:
	Icon_SPU *getTexture(int face, int data)
	{
		int dir = data & MASK_FACING;
		int type = data & MASK_TYPE;
		if (dir == FACING_Y && (face == Facing::UP || face == Facing::DOWN))
			return &ms_pTileData->treeTile_iconTop;
		else if (dir == FACING_X && (face == Facing::EAST || face == Facing::WEST))
			return &ms_pTileData->treeTile_iconTop;
		else if (dir == FACING_Z && (face == Facing::NORTH || face == Facing::SOUTH))
			return &ms_pTileData->treeTile_iconTop;

		return &ms_pTileData->treeTile_icons[data];
	}
};