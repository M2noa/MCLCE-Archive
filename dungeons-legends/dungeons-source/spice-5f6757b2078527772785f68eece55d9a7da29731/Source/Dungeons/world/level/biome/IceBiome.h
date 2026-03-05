#pragma once

#include "world/level/biome/Biome.h"

class IceBiome : public Biome {
public:

	IceBiome(int id, bool superIcy);
	void decorate(BlockSource* source, Random& random, const BlockPos& origin, bool legacy, float limit) override;
	const FeaturePtr& getTreeFeature(Random* random) override;

	Unique<Biome> createMutatedCopy(int id) override;
	virtual float getCreatureProbability() override;

private:

	bool superIcy;
};
