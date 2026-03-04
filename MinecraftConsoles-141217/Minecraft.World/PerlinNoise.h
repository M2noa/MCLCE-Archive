#pragma once
#include "Synth.h"
#include "ImprovedNoise.h"


class PerlinNoise: public Synth
{
	friend class PerlinNoise_SPU;
private:
    ImprovedNoise **noiseLevels;
    int levels;

public:
    PerlinNoise(int levels);
    PerlinNoise(Random *random, int levels);

	void init(Random *random, int levels);
	~PerlinNoise();

    virtual double getValue(double x, double y);
    double getValue(double x, double y, double z);
    doubleArray getRegion(doubleArray buffer, int x, int y, int z, int xSize, int ySize, int zSize, double xScale, double yScale, double zScale);
    doubleArray getRegion(doubleArray sr, int x, int z, int xSize, int zSize, double xScale, double zScale, double pow);
};
