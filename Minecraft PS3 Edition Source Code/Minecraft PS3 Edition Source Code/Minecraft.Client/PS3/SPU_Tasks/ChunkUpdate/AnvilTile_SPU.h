#pragma once

#include "Tile_SPU.h"

class FallingTile;

class AnvilTile_SPU : public Tile_SPU
{
public:
	static const int PART_BASE = 0;
	static const int PART_JOINT = 1;
	static const int PART_COLUMN = 2;
	static const int PART_TOP = 3;

	static const int ANVIL_NAMES_LENGTH = 3;


// public:
	//int part;

	AnvilTile_SPU(int id) : Tile_SPU(id) {}

public:
	bool isCubeShaped() { return false; }
 	bool isSolidRender(bool isServerLevel = false) { return false; }
	Icon_SPU *getTexture(int face, int data)
	{
		if (ms_pTileData->anvilPart == PART_TOP && face == Facing::UP)
		{
			int damage = (data >> 2) % ANVIL_NAMES_LENGTH;
			return &ms_pTileData->anvil_icons[damage];
		}
		return icon();
	}
	int getRenderShape()	{	return Tile_SPU::SHAPE_ANVIL;	}
	void updateShape(ChunkRebuildData *level, int x, int y, int z)
	{
		int dir = level->getData(x, y, z) & 3;
		if (dir == Direction::EAST || dir == Direction::WEST)
		{
			setShape(0, 0, 2 / 16.0f, 1, 1, 1 - 2 / 16.0f);
		}
		else
		{
			setShape(2 / 16.0f, 0, 0, 1 - 2 / 16.0f, 1, 1);
		}
	}

	bool shouldRenderFace(ChunkRebuildData *level, int x, int y, int z, int face) { return true; }
};