#pragma once

#include "world/level/biome/Biome.h"

class SavannaBiome : public Biome {
public:

	SavannaBiome(int id);
	const FeaturePtr& getTreeFeature(Random* random) override;
	void decorate(BlockSource* source, Random& random, const BlockPos& origin, bool legacy, float limit) override;

	Unique<Biome> createMutatedCopy(int id) override;

};
