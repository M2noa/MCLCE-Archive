#pragma once

#include "world/level/biome/BiomeSource.h"
#include "world/level/biome/Biome.h"

class FixedBiomeSource : public BiomeSource {

public:

	FixedBiomeSource(RandomSeed seed, int biomeId);

	virtual void fillBiomeData(LevelChunk* levelChunk, int x, int z) const override;

	virtual void fillRawBiomeData(Biome** biomes, int x, int z) const  override {
		UNUSED_PARAMETER(biomes,x,z);
	}

	virtual BlockPos getSpawnPosition()  const override {
		return BlockPos();
	}

	virtual bool findValidSpawnPosition(LayerData& layerData, int x, int z, BlockPos& spawnPosition) const  override {
		UNUSED_PARAMETER(layerData,x,z,spawnPosition);
		return false;
	}

	virtual bool isValidSpawn(int currentBiome) const  override {
		UNUSED_PARAMETER(currentBiome);
		return false;
	}

	virtual bool containsOnly(int x, int z, int r, const std::vector<int>& allowed) override {
		UNUSED_PARAMETER(x,z,r,allowed);
		return false;
	}

	virtual Biome* getBiome(int blockX, int blockZ) override;
	virtual Biome* getBiome(const BlockPos& block) override;

private:

	int biomeId;
};
