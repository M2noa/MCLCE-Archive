#pragma once

#include "world/level/biome/Biome.h"

class SwampBiome : public Biome {
public:

	SwampBiome(int id);
	const FeaturePtr& getTreeFeature(Random* random) override;
	int getGrassColor(const BlockPos& pos) override;
	int getMapGrassColor(const BlockPos& pos) override;
	int getFoliageColor() override;
	int getMapFoliageColor() override;
	FullBlock getRandomFlowerTypeAndData(Random& random, const BlockPos& pos) override;
	void buildSurfaceAt(Random& random, BlockVolume& blocks, const BlockPos& pos, float depthValue, Height seaLevel) override;

};
