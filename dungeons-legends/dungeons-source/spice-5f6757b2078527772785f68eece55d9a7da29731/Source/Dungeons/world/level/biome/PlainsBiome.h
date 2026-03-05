#pragma once

#include "world/level/biome/Biome.h"

class PlainsBiome : public Biome {
public:

	PlainsBiome(int id);

	FullBlock getRandomFlowerTypeAndData(Random& random, const BlockPos& pos) override;
	void decorate(BlockSource* source, Random& random, const BlockPos& origin, bool legacy, float limit) override;

	Unique<Biome> createMutatedCopy(int id) override;

	virtual FeaturePtr& getTreeFeature(Random* random) override;

public:

	bool sunflowers = false;
};
