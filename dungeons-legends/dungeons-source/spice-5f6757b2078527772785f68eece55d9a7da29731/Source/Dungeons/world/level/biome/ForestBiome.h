#pragma once

#include "world/level/biome/Biome.h"

class ForestBiome : public Biome {

public:

	enum {
		TYPE_NORMAL = 0,
		TYPE_FLOWER = 1,
		TYPE_BIRCH = 2,
		TYPE_ROOFED = 3,
	};

	ForestBiome(int id, int forestType);
	Biome& setColor(int color) override;
	Biome& setColor(int color, bool oddColor) override;
	const FeaturePtr& getTreeFeature(Random* random) override;
	FullBlock getRandomFlowerTypeAndData(Random& random, const BlockPos& pos) override;
	void decorate(BlockSource* source, Random& random, const BlockPos& origin, bool legacy, float limit) override;
	bool isHumid() override;

protected:

	Unique<Biome> createMutatedCopy(int id) override;

protected:

	int type;
};
