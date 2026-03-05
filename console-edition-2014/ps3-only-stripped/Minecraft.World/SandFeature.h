#pragma once
#include "Feature.h"
class Random;

class SandFeature : public Feature
{
private:
	int tile;
    int radius;

public:
	SandFeature(int radius, int tile);
    virtual bool place(Level *level, Random *random, int x, int y, int z);
};