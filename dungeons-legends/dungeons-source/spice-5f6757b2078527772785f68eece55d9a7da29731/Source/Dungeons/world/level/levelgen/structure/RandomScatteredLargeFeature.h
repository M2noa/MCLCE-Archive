#pragma once

#include "StructureFeature.h"
#include "StructureStart.h"
#include "world/level/BlockSource.h"

class BiomeSource;
class Random;
class ChunkPos;

class ScatteredFeatureStart : public StructureStart {
public:
	ScatteredFeatureStart();
	ScatteredFeatureStart(int x, int z);

	ScatteredFeatureStart(Dimension& region, Random& random, int chunkX, int chunkZ);

	StructureFeatureType getType() const override {
		return StructureFeatureType::Temple;
	}
};

class RandomScatteredLargeFeature : public StructureFeature {

public:
	RandomScatteredLargeFeature();
	
	virtual std::string getFeatureName() override;
	std::vector<int> allowedBiomes;

	const MobList& getEnemiesAt(const BlockPos& pos);

protected:
	virtual bool isFeatureChunk(BiomeSource* biomeSource, Random& random, const ChunkPos& pos) override;

	virtual Unique<StructureStart> createStructureStart(Dimension& generator, Random& random, const ChunkPos& cp) override;

private:
	int mSpacing = 32;
	int mMinSeparation = 8;
	MobList mEmptyEnemies;
	MobList mWitchHutEnemies;
};

