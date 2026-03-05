/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/
#pragma once

#include "world/level/chunk/ChunkSource.h"
//#include "platform/threading/ThreadLocal.h"
#include "world/level/levelgen/structure/EndCityFeature.h"
#include "world/level/levelgen/synth/SimplexNoise.h"

namespace TheEndGen {
	static const int GEN_DEPTH = 128;
	static const int DIMENSION_HEIGHT = 256;
	static const int CHUNK_HEIGHT = 4;
	static const int CHUNK_WIDTH = 8;
	static const int CHUNKCOUNT_XZ = 16 / TheEndGen::CHUNK_WIDTH;

	static const int NOISE_SIZE_X = CHUNKCOUNT_XZ + 1;
	static const int NOISE_SIZE_Y = GEN_DEPTH / CHUNK_HEIGHT + 1;
	static const int NOISE_SIZE_Z = CHUNKCOUNT_XZ + 1;

	static const int ISLAND_CHUNK_DISTANCE = 64;
	static const float ISLAND_THRESHOLD = -0.90f;
	static const int64_t ISLAND_CHUNK_DISTANCE_SQR = ISLAND_CHUNK_DISTANCE * ISLAND_CHUNK_DISTANCE;
}

class BlockVolume;
class Level;
class LevelChunk;
class PerlinNoise;
class SimplexNoise;
typedef std::unique_ptr<PerlinNoise> PerlinNoisePtr;
typedef std::unique_ptr<SimplexNoise> SimplexNoisePtr;
enum class StructureFeatureType : int8_t;

class TheEndRandomLevelSource : public ChunkSource {
public:
	struct ThreadData {
		std::array<BlockID, CHUNK_WIDTH * CHUNK_WIDTH * TheEndGen::GEN_DEPTH> blockBuffer;
	};

	TheEndRandomLevelSource(Level* level, Dimension* dimension, RandomSeed seed);
	
	virtual LevelChunk* requestChunk(const ChunkPos& cp, LoadMode lm) override;
	virtual void loadChunk(LevelChunk& levelChunk, bool forceImmediateReplacementDataLoad) override;

	virtual bool postProcess(ChunkViewSource& source) override;
	virtual void postProcessMobsAt(BlockSource* blockSource, int chunkWestBlock, int chunkNorthBlock, Random& random) override;

	virtual const MobList& getMobsAt(BlockSource& blockSource, EntityType category, const BlockPos& pos) override;

	void prepareHeights(BlockVolume& blocks, int xOffs, int zOffs) override;
	void buildSurfaces(BlockVolume& blocks, const ChunkPos& chunkPos);
	bool isIslandChunk(int chunkX, int chunkZ);

	std::string gatherStats();
	virtual bool findNearestFeature(StructureFeatureType feature, Player& player, BlockPos& pos) override;

	Unique<CompoundTag> getStructureTag() const override;
	void readStructureTag(const CompoundTag& tag) override;

private:
	void getHeights(float* noiseBuffer, int x, int y, int z);
	float getIslandHeightValue(int chunkX, int chunkZ, int subSectionX, int subSectionZ);

	PerlinNoisePtr mLPerlinNoise1;
	PerlinNoisePtr mLPerlinNoise2;
	PerlinNoisePtr mPerlinNoise1;

	SimplexNoisePtr mIslandNoise;

	//ThreadLocal<ThreadData> generatorHelpersPool;

	EndCityFeature mEndCityFeature;
};
