#pragma once

#include "CommonTypes.h"
#include "world/level/levelgen/synth/PerlinSimplexNoise.h"

class Feature;
class BlockSource;
class LevelChunk;
class Random;
class Biome;
class BlockPos;

class FlowerFeature;
typedef std::unique_ptr<FlowerFeature> FlowerFeaturePtr;
class DoublePlantFeature;
typedef std::unique_ptr<DoublePlantFeature> DoublePlantFeaturePtr;

class BiomeDecorator {
public:

	static char gaussianKernel[5][5];
	BiomeDecorator();

	virtual void decorate(BlockSource* source, Random& random, Biome* biome, const BlockPos& origin, bool legacy, float limit);

	virtual void decorateOres(BlockSource* source, Random& random, const BlockPos& origin);
	void decorateDepthSpan(BlockSource* levelChunk, Random& random, const BlockPos& origin, int count, FeaturePtr& feature, int y0, int y1);
	void decorateDepthAverage(BlockSource* levelChunk, Random& random, const BlockPos& origin, int count, FeaturePtr& feature, int yMid, int ySpan);
	
public:

	virtual ~BiomeDecorator();

	static const float SNOW_CUTOFF;
	static const float SNOW_SCALE;
	FeaturePtr clayFeature;
	FeaturePtr sandFeature;
	FeaturePtr gravelFeature;
	FeaturePtr dirtOreFeature;
	FeaturePtr gravelOreFeature;
	FeaturePtr graniteFeature;
	FeaturePtr dioriteFeature;
	FeaturePtr andesiteFeature;
	FeaturePtr coalOreFeature;
	FeaturePtr ironOreFeature;
	FeaturePtr goldOreFeature;
	FeaturePtr redStoneOreFeature;
	FeaturePtr diamondOreFeature;
	FeaturePtr lapisOreFeature;
	FlowerFeaturePtr flowerFeature;
	FeaturePtr brownMushroomFeature;
	FeaturePtr redMushroomFeature;
	FeaturePtr hugeMushroomFeature;
	FeaturePtr reedsFeature;
	FeaturePtr cactusFeature;
	FeaturePtr waterlilyFeature;
	FeaturePtr pumpkinFeature;
	FeaturePtr lavaSpringFeature;
	FeaturePtr waterSpringFeature;
	FeaturePtr deadBushFeature;
	FeaturePtr desertWellFeature;
	FeaturePtr oakFeature;
	FeaturePtr spruceTreeFeature;
	FeaturePtr birchFeature;
	FeaturePtr roofTreeFeature;
	DoublePlantFeaturePtr doublePlantFeature;

	FeaturePtr pineTreeFeature;

	FeaturePtr megaPineTreeFeature;
	FeaturePtr megaSpruceTreeFeature;

	FeaturePtr tallGrassFeature;
	FeaturePtr fernFeature;
	FeaturePtr fancyTreeFeature;
	FeaturePtr forestRockFeature;
	FeaturePtr swampTreeFeature;

	FeaturePtr iceSpikeFeature;
	FeaturePtr icePatchFeature;
	FeaturePtr jungleBushFeature;

	FeaturePtr jungleTreeFeature;
	FeaturePtr megaJungleTreeFeature;

	FeaturePtr melonFeature;
	FeaturePtr vinesFeature;

	FeaturePtr savannaTreeFeature;
	FeaturePtr superBirchFeature;
	FeaturePtr iceSpideFeature;

	int waterlilyCount = 0;
	float treeCount  = 0;
	int flowerCount = 2;
	int grassCount = 1;
	int deadBushCount = 0;
	int mushroomCount = 0;
	int reedsCount = 0;
	int cactusCount = 0;
	int gravelCount = 1;
	int sandCount = 3;
	int clayCount = 1;
	int hugeMushrooms = 0;
	bool liquids = true;

protected:

	int _getRandomHeight(int x, int z, BlockSource* source, Random& random, const BlockPos& origin );
	void _placeFeature(BlockSource* source, const FeaturePtr& feature, const BlockPos& origin, Random& random);

	BlockPos _getRandomSolidPosition(BlockSource* region, const BlockPos& chunkMin, Random& random);
	BlockPos _getRandomSurfacePosition(BlockSource* region, const BlockPos& chunkMin, Random& random);
	BlockPos _getRandomTreePosition(BlockSource* region, const BlockPos& chunkMin, Random& random);

};
