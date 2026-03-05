#pragma once

#include "world/level/biome/Biome.h"
#include "world/level/biome/BiomeDecorator.h"

class PerlinSimplexNoise;
class BlockSource;
class Random;
class BlockPos;

class MesaBiome : public Biome {
public:

	class Decorator : public BiomeDecorator {
public:

		virtual void decorateOres(BlockSource* source, Random& random, const BlockPos& origin) override;

	};

	MesaBiome(int id, bool brycePillars, bool hasForest);
	const FeaturePtr& getTreeFeature(Random* random) override;
	int getFoliageColor() override;
	int getMapFoliageColor() override;
	int getGrassColor(const BlockPos& pos) override;
	int getMapGrassColor(const BlockPos& pos) override;
	void refreshBiome(RandomSeed levelSeed) override;
	void buildSurfaceAt(Random& random, BlockVolume& blocks, const BlockPos& pos, float depthValue, Height seaLevel) override;

	Unique<Biome> createMutatedCopy(int id) override;

protected:

	void generateBands(RandomSeed seed);
	unsigned char getBand(int worldX, int y, int worldZ);

protected:

	bool brycePillars;
	bool hasForest;
	char clayBands[64];
	std::unique_ptr<PerlinSimplexNoise> pillarNoise;
	std::unique_ptr<PerlinSimplexNoise> pillarRoofNoise;
	std::unique_ptr<PerlinSimplexNoise> clayBandsOffsetNoise;
};
