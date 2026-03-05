#pragma once

#include "world/level/biome/Biome.h"

class MutatedBiome : public Biome {
public:

	MutatedBiome(int id, Biome* containedBiome);

	void decorate(BlockSource* source, Random& random, const BlockPos& origin, bool legacy, float limit) override;
	void buildSurfaceAt(Random& random, BlockVolume& blocks, const BlockPos& pos, float depthValue, Height seaLevel) override;
	float getCreatureProbability() override;
	const FeaturePtr& getTreeFeature(Random* random) override;
	int getFoliageColor() override;
	int getMapFoliageColor() override;
	int getGrassColor(const BlockPos& pos) override;
	int getMapGrassColor(const BlockPos& pos) override;
	bool isSame(Biome* other) override;
	BiomeTempCategory getTemperatureCategory() const override;

protected:

	Biome* containedBiome;
};
