#pragma once
#include "Synth.h"

class ImprovedNoise : public Synth
{
	friend class PerlinNoise_SPU;
private:
    int *p;

public:
    double scale;
    double xo, yo, zo;

    ImprovedNoise();
    ImprovedNoise(Random *random);
	void init(Random *random);

	~ImprovedNoise();

    double noise(double _x, double _y, double _z);

    double lerp(double t, double a, double b);
    double grad2(int hash, double x, double z);
    double grad(int hash, double x, double y, double z);
    virtual double getValue(double x, double y);
    double getValue(double x, double y, double z);

    void add(doubleArray buffer, double _x, double _y, double _z, int xSize, int ySize, int zSize, double xs, double ys, double zs, double pow);
};
