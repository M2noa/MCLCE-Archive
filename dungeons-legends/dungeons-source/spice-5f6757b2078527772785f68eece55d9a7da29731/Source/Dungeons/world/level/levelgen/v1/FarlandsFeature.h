#pragma once

#include "common_header.h"

#include "world/level/levelgen/v1/LargeFeature.h"
#include "world/level/levelgen/synth/PerlinNoise.h"
#include "Core/Utility/buffer_span.h"

class LevelChunk;

class FarlandsFeature {
public:

	const int limit, endWidth;
	const Height seaLevel;

	FarlandsFeature(RandomSeed seed, int limit, int endWidth, Height seaLevel);

	bool isInsideLimit(const LevelChunk& lc, float margin = 0) const;

	void apply(buffer_span_mut<BlockID> blocks, LevelChunk& lc, Dimension& biomeSource, Random& random);

protected:

	void addFeature( LevelChunk& lc, Dimension& generator, Random& random, int x, int z );

private:

	float max = 17, min = -17;

	PerlinNoise noise, zoneNoise;
};
