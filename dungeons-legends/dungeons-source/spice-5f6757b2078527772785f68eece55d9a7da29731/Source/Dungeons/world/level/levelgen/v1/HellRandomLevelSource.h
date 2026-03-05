/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/
#pragma once

#include "world/level/chunk/ChunkSource.h"
// #include "platform/threading/ThreadLocal.h"
#include "world/level/biome/FixedBiomeSource.h"
#include "world/level/levelgen/structure/NetherBridgeFeature.h"
#include "world/level/levelgen/v1/LargeHellCaveFeature.h"
#include "util/Random.h"
#include "Core/Utility/buffer_span.h"

namespace HellGen {
	static const uint32_t GEN_DEPTH = 128;
	static const int CHUNK_HEIGHT = 8;
	static const uint32_t CHUNK_WIDTH = 4;
	static const uint32_t CHUNKCOUNT_XZ = 16 / CHUNK_WIDTH;
	static const uint32_t NOISE_SIZE_X = CHUNKCOUNT_XZ + 1;
	static const uint32_t NOISE_SIZE_Y = GEN_DEPTH / CHUNK_HEIGHT + 1;
	static const uint32_t NOISE_SIZE_Z = CHUNKCOUNT_XZ + 1;
}

class Biome;
class Level;
class Dimension;
class Block;
class PerlinNoise;
enum class StructureFeatureType : int8_t;
typedef std::unique_ptr<PerlinNoise> PerlinNoisePtr;

class HellRandomLevelSource : 
	public ChunkSource {
public:

	struct ThreadData {
		Random random;
		LargeHellCaveFeature hellCaveFeature;
		std::array<uint8_t, (HellGen::GEN_DEPTH * CHUNK_COLUMNS) + (HellGen::GEN_DEPTH * CHUNK_COLUMNS) / 2> blockBuffer;
	};

	HellRandomLevelSource(Level* level, Dimension* dimension, RandomSeed seed);

	virtual LevelChunk* requestChunk(const ChunkPos& cp, LoadMode lm) override;

	virtual void loadChunk(LevelChunk& levelChunk, bool forceImmediateReplacementDataLoad) override;

	virtual bool postProcess(ChunkViewSource& source) override;

	virtual const MobList& getMobsAt(BlockSource& blockSource, EntityType category, const BlockPos& pos) override;

	void prepareHeights(BlockVolume& blocks, int xOffs, int zOffs) override;
	void buildSurfaces(BlockVolume& blocks, const ChunkPos& chunkPos);

	std::string gatherStats();
	virtual bool findNearestFeature(StructureFeatureType feature, Player& player, BlockPos& pos) override;

	Unique<CompoundTag> getStructureTag() const override;
	void readStructureTag(const CompoundTag& tag) override;

private:

	void getHeights(float* noiseBuffer, int x, int y, int z);

private:

	PerlinNoisePtr lperlinNoise1;
	PerlinNoisePtr lperlinNoise2;
	PerlinNoisePtr perlinNoise1;
	PerlinNoisePtr perlinNoise2;
	PerlinNoisePtr perlinNoise3;
	PerlinNoisePtr scaleNoise;
	PerlinNoisePtr depthNoise;

// 	ThreadLocal<ThreadData> generatorHelpersPool;

	NetherBridgeFeature netherBridgeFeature;
};
