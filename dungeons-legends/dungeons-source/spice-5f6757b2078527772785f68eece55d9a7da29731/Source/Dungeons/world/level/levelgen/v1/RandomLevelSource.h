/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

#include "world/level/chunk/ChunkSource.h"
#include "world/level/levelgen/v1/LargeCaveFeature.h"
#include "SharedConstants.h"
// #include "platform/threading/ThreadLocal.h"
#include "world/level/biome/BiomeSource.h"
#include "world/level/levelgen/structure/VillageFeature.h"
#include "world/level/levelgen/structure/StrongholdFeature.h"
#include "world/level/levelgen/structure/MineshaftFeature.h"
#include "world/level/levelgen/structure/RandomScatteredLargeFeature.h"
#include "world/level/levelgen/feature/MonsterRoomFeature.h"
#include "world/level/levelgen/structure/OceanMonumentFeature.h"
#include "Core/Utility/buffer_span.h"
#include "util/NibblePair.h"

class Biome;
class Level;
class LevelChunk;
class Feature;
class PerlinNoise;
class FarlandsFeature;
class BlockTickingQueue;
class BlockVolume;
typedef std::unique_ptr<PerlinNoise> PerlinNoisePtr;
class PerlinSimplexNoise;
enum class StructureFeatureType : int8_t;

class RandomLevelSource : 
	public ChunkSource {

	static const float SNOW_CUTOFF;
	static const float SNOW_SCALE;
public:

	struct ThreadData {

		float buffer[1024];
		float depthBuffer[16 * 16];
		float dataBuffer[16 * 16];
		std::array<uint8_t, LEVEL_GEN_HEIGHT * CHUNK_COLUMNS + LEVEL_GEN_HEIGHT * CHUNK_COLUMNS / 2> blockBuffer;

		float* fi = nullptr;
		float* fis = nullptr;

		Random random;
		LargeCaveFeature caveFeature;
	};

	const bool legacyDevice;

	RandomLevelSource(Level* level, Dimension* dimension, RandomSeed seed, bool box);
	~RandomLevelSource();

	virtual LevelChunk* requestChunk(const ChunkPos& cp, LoadMode lm) override;

	virtual void loadChunk(LevelChunk& c, bool forceImmediateReplacementDataLoad) override;

	virtual bool postProcess( ChunkViewSource& source ) override;

	void _fixWaterAlongEdges(LevelChunk& lc, BlockSource& source, BlockTickingQueue& instaTickQueue);

	virtual void postProcessMobsAt(BlockSource* blockSource, int chunkWestBlock, int chunkNorthBlock, Random& random) override;

	void prepareHeights(BlockVolume& blocks, int xOffs, int zOffs) override;
	void buildSurfaces(ThreadData& thread, BlockVolume& blocks, LevelChunk& levelChunk, const ChunkPos& chunkPos);

	std::string gatherStats();

	const MobList& getMobsAt(BlockSource& blockSource, EntityType category, const BlockPos& pos) override;

	//bool save(bool force, ProgressListener progressListener) {
	virtual bool findNearestFeature(StructureFeatureType feature, Player& player, BlockPos& pos) override;

	Unique<CompoundTag> getStructureTag() const override;
	void readStructureTag(const CompoundTag& tag) override;

private:

	void getHeights(float* noiseBuffer, Biome** biomes, int x, int y, int z);

public:

	//Biome** biomes;
private:

	Level* mLevel;

	PerlinNoisePtr minLimitPerlinNoise;
	PerlinNoisePtr maxLimitPerlinNoise;
	PerlinNoisePtr mainPerlinNoise;
	Unique<PerlinSimplexNoise> surfaceNoise;
	PerlinNoisePtr scaleNoise;
	PerlinNoisePtr depthNoise;
	PerlinNoisePtr forestNoise;

	// This will always be the same values, can be shared between threads
	float biomeWeights[5 * 5];

// 	ThreadLocal<ThreadData> generatorHelpersPool;
	VillageFeature villageFeature;
	StrongholdFeature strongholdFeature;
	RandomScatteredLargeFeature scatteredFeature;
	MineshaftFeature mineshaftFeature;
	MonsterRoomFeature monsterRoomFeature;
	OceanMonumentFeature oceanMonumentFeature;
};
