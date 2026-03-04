#pragma once
#include "Synth.h"
#include "SimplexNoise.h"

class PerlinSimplexNoise : public Synth
{
private:
	SimplexNoise **noiseLevels;
    int levels;

public:
    PerlinSimplexNoise(int levels);
    PerlinSimplexNoise(Random *random, int levels);
	void init(Random *random, int levels);
	~PerlinSimplexNoise();

    virtual double getValue(double x, double y);
    double getValue(double x, double y, double z);

    doubleArray getRegion(doubleArray buffer, double x, double y, int xSize, int ySize, double xScale, double yScale, double sizeScale);   
    doubleArray getRegion(doubleArray buffer, double x, double y, int xSize, int ySize, double xScale, double yScale, double sizeScale, double powScale);
    doubleArray getRegion(doubleArray buffer, double x, double y, double z, int xSize, int ySize, int zSize, double xScale, double yScale, double zScale);
};