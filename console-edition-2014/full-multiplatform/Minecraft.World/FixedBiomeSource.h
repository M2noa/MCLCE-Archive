#pragma once
#include "BiomeSource.h"

class FixedBiomeSource : public BiomeSource
{
private:
    Biome *biome;
    float temperature, downfall;

public:
	using BiomeSource::getTemperature;

    FixedBiomeSource(Biome *fixed, float temperature, float downfall);

    virtual Biome *getBiome(ChunkPos *cp);
    virtual Biome *getBiome(int x, int z);
    virtual float getTemperature(int x, int z);
	virtual void getTemperatureBlock(floatArray& temperatures, int x, int z, int w, int h) const;
	virtual floatArray getTemperatureBlock(int x, int z, int w, int h) const;
    virtual void getTemperatureBlock(doubleArray& temperatures, int x, int z, int w, int h) const;
	virtual void getDownfallBlock(floatArray &downfalls, int x, int z, int w, int h) const;
	virtual floatArray getDownfallBlock(int x, int z, int w, int h) const;
	virtual float getDownfall(int x, int z) const;
	virtual void getDownfallBlock(doubleArray downfalls, int x, int z, int w, int h);
    virtual void getBiomeBlock(BiomeArray& biomes, int x, int z, int w, int h, bool useCache) const;
	virtual void getBiomeIndexBlock(byteArray& biomeIndices, int x, int z, int w, int h, bool useCache) const;

	// 4J-PB added in from beyond 1.8.2
	virtual BiomeArray getRawBiomeBlock(int x, int z, int w, int h) const;
	virtual void getRawBiomeBlock(BiomeArray &biomes, int x, int z, int w, int h) const;

	////////////////////////////////////
	virtual TilePos *findBiome(int x, int z, int r, Biome *toFind, Random *random);
	virtual TilePos *findBiome(int x, int z, int r, vector<Biome *> allowed, Random *random);
	virtual bool containsOnly(int x, int z, int r, Biome *allowed);
	virtual bool containsOnly(int x, int z, int r, vector<Biome *> allowed);
};
