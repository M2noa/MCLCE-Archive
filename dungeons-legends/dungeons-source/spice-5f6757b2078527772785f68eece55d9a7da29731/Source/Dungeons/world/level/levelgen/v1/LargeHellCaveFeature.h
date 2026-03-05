#pragma once

#include "world/level/levelgen/v1/LargeFeature.h"
#include "Core/Utility/buffer_span.h"

class BiomeSource;
class LevelChunk;
class Random;
class Vec3;
struct BlockID;
class BlockVolume;

class LargeHellCaveFeature {
public:
	void apply(BlockVolume& blocks, LevelChunk& lc, Dimension& biomeSource, Random& random);

protected:
	
	void addRoom(BlockVolume& blocks, Random& random, LevelChunk& lc, const Vec3& room);
	void addTunnel(BlockVolume& blocks, Random& _random, LevelChunk& lc, const Vec3& startPos, float thickness, float yRot, float xRot, int step, int dist, float yScale);

	void addFeature(BlockVolume& blocks, LevelChunk& lc, Dimension& generator, Random& random, int x, int z);
};
