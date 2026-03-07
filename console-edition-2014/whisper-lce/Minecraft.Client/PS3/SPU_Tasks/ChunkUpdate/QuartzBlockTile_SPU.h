#pragma once

#include "Tile_SPU.h"

class QuartzBlockTile_SPU : public Tile_SPU
{
public:
	static const int TYPE_DEFAULT = 0;
	static const int TYPE_CHISELED = 1;
	static const int TYPE_LINES_Y = 2;
	static const int TYPE_LINES_X = 3;
	static const int TYPE_LINES_Z = 4;

	static const int QUARTZ_BLOCK_NAMES = 3;

private:
	static const int QUARTZ_BLOCK_TEXTURES = 5;

// 	Icon *icons[QUARTZ_BLOCK_TEXTURES];
// 	Icon *iconChiseledTop;
// 	Icon *iconLinesTop;
// 	Icon *iconTop;
// 	Icon *iconBottom;

public:
	QuartzBlockTile_SPU(int id) : Tile_SPU(id) {}

	Icon_SPU *getTexture(int face, int data)
	{
		if (data == TYPE_LINES_Y || data == TYPE_LINES_X || data == TYPE_LINES_Z)
		{
			if (data == TYPE_LINES_Y && (face == Facing::UP || face == Facing::DOWN))
			{
				return &ms_pTileData->quartzBlock_iconLinesTop;
			}
			else if (data == TYPE_LINES_X && (face == Facing::EAST || face == Facing::WEST))
			{
				return &ms_pTileData->quartzBlock_iconLinesTop;
			}
			else if (data == TYPE_LINES_Z && (face == Facing::NORTH || face == Facing::SOUTH))
			{
				return &ms_pTileData->quartzBlock_iconLinesTop;
			}

			return &ms_pTileData->quartzBlock_icons[data];
		}

		if (face == Facing::UP || (face == Facing::DOWN && data == TYPE_CHISELED))
		{
			if (data == TYPE_CHISELED)
			{
				return &ms_pTileData->quartzBlock_iconChiseledTop;
			}
			return &ms_pTileData->quartzBlock_iconTop;
		}
		if (face == Facing::DOWN)
		{
			return &ms_pTileData->quartzBlock_iconBottom;
		}
		if (data < 0 || data >= QUARTZ_BLOCK_TEXTURES) data = 0;
		return &ms_pTileData->quartzBlock_icons[data];
	}

	int getRenderShape() { return Tile_SPU::SHAPE_QUARTZ; }
};