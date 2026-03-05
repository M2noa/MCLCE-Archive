#pragma once

class Level;
class Random;
class Biome;
class Feature;

class BiomeDecorator
{
	friend class DesertBiome;
	friend class ForestBiome;
	friend class PlainsBiome;
	friend class SwampBiome;
	friend class TaigaBiome;
	friend class MushroomIslandBiome;
	friend class BeachBiome;
	friend class JungleBiome;
protected:
	Level *level;
	Random *random;
	int xo;
	int zo;
	Biome *biome;

public:
	BiomeDecorator(Biome *biome);

	void decorate(Level *level, Random *random, int xo, int zo);

protected:
	Feature *clayFeature;
	Feature *sandFeature;
	Feature *gravelFeature;
	Feature *dirtOreFeature;
	Feature *gravelOreFeature;
	Feature *coalOreFeature;
	Feature *ironOreFeature;
	Feature *goldOreFeature;
	Feature *redStoneOreFeature;
	Feature *diamondOreFeature;
	Feature *lapisOreFeature;
	Feature *yellowFlowerFeature;
	Feature *roseFlowerFeature;
	Feature *brownMushroomFeature;
	Feature *redMushroomFeature;
	Feature *hugeMushroomFeature;
	Feature *reedsFeature;
	Feature *cactusFeature;
	Feature *waterlilyFeature;

	int waterlilyCount;
	int treeCount;
	int flowerCount;
	int grassCount;
	int deadBushCount;
	int mushroomCount;
	int reedsCount;
	int cactusCount;
	int gravelCount;
	int sandCount;
	int clayCount;
	int hugeMushrooms;
	bool liquids;

	void _init();

protected:
	virtual void decorate();


	void decorate(int count, Feature *feature);
	void decorateDepthSpan(int count, Feature *feature, int y0, int y1);
	void decorateDepthAverage(int count, Feature *feature, int yMid, int ySpan);
	void decorateOres();
};