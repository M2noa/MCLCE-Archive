#pragma once

#include "util/Random.h"

class Random;
class Level;
class ChunkSource;
class BiomeSource;
class LevelChunk;
class Dimension;

class LargeFeature {
public:

	LargeFeature();
	virtual ~LargeFeature();

	virtual void apply(LevelChunk& lc, Dimension& dimension, Random& random);

protected:

	virtual void addFeature(LevelChunk& lc, Dimension& generator, Random& random, int x, int z ) = 0;

	int radius;
	//Level* level;
};
