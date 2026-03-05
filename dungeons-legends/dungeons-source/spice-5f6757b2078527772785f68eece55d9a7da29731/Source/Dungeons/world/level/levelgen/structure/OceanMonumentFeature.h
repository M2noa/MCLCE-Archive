/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

#include "world/level/levelgen/structure/StructureFeature.h"
#include "world/level/levelgen/structure/StructureStart.h"
#include "world/level/BlockSource.h"

class ChunkPos;

class OceanMonumentFeature : public StructureFeature {
public:
	OceanMonumentFeature();
	virtual std::string getFeatureName() override;	

	const MobList &getEnemies() const { return mMonumentEnemies; }

protected:
	Unique<StructureStart> createStructureStart(Dimension& generator, Random& random, const ChunkPos& lc) override;
	virtual bool isFeatureChunk(BiomeSource* biomeSource, Random& random, const ChunkPos& pos) override;

private:
	int mMonumentSpacing = 32;
	int mMinMonumentSeparation = 5;
	std::vector<int> allowedBiomes;
	std::vector<int> allowedSpawnBiomes;
	MobList mMonumentEnemies;
};

class OceanMonumentStart : public StructureStart {
public:
	OceanMonumentStart();
	OceanMonumentStart(Dimension& dim, Random& random, int x, int z);
	void createMonument(Dimension& dim, Random& random, int x, int z);
	virtual bool postProcess(BlockSource* level, Random& random, const BoundingBox& chunkBB) override;
	virtual void addAdditionalSaveData(CompoundTag& tag) override;
	virtual void readAdditionalSaveData(const CompoundTag& tag) override;
	StructureFeatureType getType() const override {
		return StructureFeatureType::Monument;
	}

private:
	bool isCreated = false;
};
