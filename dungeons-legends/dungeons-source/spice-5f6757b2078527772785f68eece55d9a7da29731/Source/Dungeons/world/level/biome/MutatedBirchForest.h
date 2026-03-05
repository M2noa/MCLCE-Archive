#pragma once

#include "world/level/biome/MutatedBiome.h"

class MutatedBirchForest : public MutatedBiome {
public:

	MutatedBirchForest(int id, Biome* containedBiome);
	const FeaturePtr& getTreeFeature(Random* random) override;

};
