#pragma once

#include "Tile_SPU.h"

class FlowerPotTile_SPU : public Tile_SPU
{
public:
	static const int TYPE_FLOWER_RED = 1;
	static const int TYPE_FLOWER_YELLOW = 2;
	static const int TYPE_SAPLING_DEFAULT = 3;
	static const int TYPE_SAPLING_EVERGREEN = 4;
	static const int TYPE_SAPLING_BIRCH = 5;
	static const int TYPE_SAPLING_JUNGLE = 6;
	static const int TYPE_MUSHROOM_RED = 7;
	static const int TYPE_MUSHROOM_BROWN = 8;
	static const int TYPE_CACTUS = 9;
	static const int TYPE_DEAD_BUSH = 10;
	static const int TYPE_FERN = 11;

	FlowerPotTile_SPU(int id) : Tile_SPU(id) {}

	void updateDefaultShape()
	{
		float size = 6.0f / 16.0f;
		float half = size / 2;
		setShape(0.5f - half, 0, 0.5f - half, 0.5f + half, size, 0.5f + half);
	}

 	bool isSolidRender(bool isServerLevel = false) { return false; }
	int getRenderShape() { return SHAPE_FLOWER_POT; }
	bool isCubeShaped() { return false; }
};