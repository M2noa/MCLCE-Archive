#pragma once

#include "Bush_SPU.h"

class TallGrass_SPU : public Bush_SPU
{
public:
	static const int DEAD_SHRUB;
	static const int TALL_GRASS;
	static const int FERN;

	static const int TALL_GRASS_TILE_NAMES_LENGTH = 3;

// 	static const unsigned int TALL_GRASS_TILE_NAMES[TALL_GRASS_TILE_NAMES_LENGTH];


	TallGrass_SPU(int id) : Bush_SPU(id) {}

	virtual Icon_SPU *getTexture(int face, int data);
	virtual int getColor(ChunkRebuildData *level, int x, int y, int z);
	virtual int getColor(ChunkRebuildData *level, int x, int y, int z, int data);	// 4J added
};