#pragma once

#include "common_header.h"

#include "Core/Utility/buffer_span.h"
#include "util/NibblePair.h"
#include "CommonTypes.h"

class LevelChunk;
class Random;
class BiomeSouce;
class Vec3;
struct BlockID;
class Dimension;
class BlockVolume;

class LargeCaveFeature {
public:
	void apply(BlockVolume& blocks, LevelChunk& lc, Dimension& biomeSource, Random& random);

protected:
	void addRoom(BlockVolume& blocks, Random& random, LevelChunk& lc, const Vec3& room);

	bool _thinSand(buffer_span<BlockID> blocks, Height height, int y );

	bool isDiggable(BlockID block, BlockID above);

	void addTunnel(BlockVolume& blocks, Random& _random, LevelChunk& lc, const Vec3& startPos, float thickness, float yRot, float xRot, int step, int dist, float yScale);

	void addFeature(BlockVolume& blocks, LevelChunk& lc, Dimension& generator, Random& random, int x, int z );

};
