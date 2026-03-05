#pragma once

#include "world/level/BlockPos.h"
#include "world/level/newbiome/Layer.h"
#include "world/level/GeneratorType.h"
#include "CommonTypes.h"
//#include "platform/threading/ThreadLocal.h"

class Level;
class Biome;
class ChunkPos;
class LevelChunk;
class Layer;
class LayerData;

class BiomeSource {
public:

	BiomeSource(RandomSeed seed);
	BiomeSource(RandomSeed seed, GeneratorType type);
	virtual ~BiomeSource() {
	}

	virtual void fillBiomeData(LevelChunk* levelChunk, int x, int z) const;
	virtual void fillRawBiomeData(Biome** biomes, int x, int z) const;
	//static void debugVisulizeMap(int x, int y, int w, int h, std::weak_ptr<std::vector<int> > data, std::weak_ptr<ThreadLocal<BiomeSource> > source);
	virtual BlockPos getSpawnPosition() const;

	virtual bool findValidSpawnPosition(LayerData& layerData, int x, int z, BlockPos& spawnPosition) const;
	virtual bool isValidSpawn(int currentBiome) const;

	virtual bool containsOnly(int x, int z, int r, const std::vector<int>& allowed);

	virtual Biome* getBiome(int blockX, int blockZ);
	virtual Biome* getBiome(const BlockPos& block);

	RandomSeed getSeed() {
		return seed;
	}

private:

	LayerPtr getZoomedLayer();

	LayerPtr biomeLayer;
	LayerPtr zoomedLayer;
	std::array<int, 7> playerValidSpawns;
	RandomSeed seed;
};
