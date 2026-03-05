/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/

#pragma once

#include "world/level/Level.h"
#include "world/level/levelgen/synth/PerlinSimplexNoise.h"

class BlockPos;
class ChunkBlockPos;
class BlockSource;
class MinecraftClient;
class TextureData;
class Dimension;

class Seasons {

public:

	explicit Seasons(Dimension& d);
	~Seasons();

	void tick();

	void postProcess(LevelChunk& lc, BlockSource& region, const ChunkPos& ctp);

private:

	Dimension& mDimension;
	PerlinSimplexNoise mSnowNoise;
};
