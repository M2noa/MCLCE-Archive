#pragma once

#include "world/level/biome/Biome.h"
class TaigaBiome : public Biome {
public:

	enum {
		NORMAL = 0,
		MEGA = 1,
		MEGA_SPRUCE = 2,
	};

	TaigaBiome(int id, int type);

	const FeaturePtr& getTreeFeature(Random* random) override;
	const FeaturePtr& getGrassFeature(Random* random) override;
	void decorate(BlockSource* source, Random& random, const BlockPos& origin, bool legacy, float limit) override;
	void buildSurfaceAt(Random& random, BlockVolume& blocks, const BlockPos& pos, float depthValue, Height seaLevel) override;

	Unique<Biome> createMutatedCopy(int id) override;

protected:

	int type;
};
