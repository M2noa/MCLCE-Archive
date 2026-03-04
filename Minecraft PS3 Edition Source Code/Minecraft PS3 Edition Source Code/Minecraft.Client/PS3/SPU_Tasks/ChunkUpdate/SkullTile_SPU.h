#pragma once

#include "Tile_SPU.h"
#include "EntityTile_SPU.h"


class SkullTile_SPU : public EntityTile_SPU
{
public:
	static const int MAX_SKULL_TILES = 40;
public:
	static const int PLACEMENT_MASK = 0x7;
	static const int NO_DROP_BIT = 0x8;

	SkullTile_SPU(int id) : EntityTile_SPU(id) {}

public:
	int getRenderShape() { return SHAPE_INVISIBLE; }
	bool isSolidRender(bool isServerLevel = false) { return false; }
	bool isCubeShaped() { return false; }
	void updateShape(ChunkRebuildData *level, int x, int y, int z)
	{
		int data = level->getData(x, y, z) & PLACEMENT_MASK;

		switch (data)
		{
		default:
		case Facing::UP:
			setShape(4.0f / 16.0f, 0, 4.0f / 16.0f, 12.0f / 16.0f, .5f, 12.0f / 16.0f);
			break;
		case Facing::NORTH:
			setShape(4.0f / 16.0f, 4.0f / 16.0f, .5f, 12.0f / 16.0f, 12.0f / 16.0f, 1);
			break;
		case Facing::SOUTH:
			setShape(4.0f / 16.0f, 4.0f / 16.0f, 0, 12.0f / 16.0f, 12.0f / 16.0f, .5f);
			break;
		case Facing::WEST:
			setShape(.5f, 4.0f / 16.0f, 4.0f / 16.0f, 1, 12.0f / 16.0f, 12.0f / 16.0f);
			break;
		case Facing::EAST:
			setShape(0, 4.0f / 16.0f, 4.0f / 16.0f, .5f, 12.0f / 16.0f, 12.0f / 16.0f);
			break;
		}
	}

	Icon_SPU *getTexture(int face, int data) { 	return TileRef_SPU(hellSand_Id)->getTexture(face); }
};