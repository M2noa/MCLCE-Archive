#pragma once
#include "Feature.h"

class Level;

class FlowerFeature : public Feature
{
private:
	int tile;

public:
	FlowerFeature (int tile);
	bool place(Level *level, Random *random, int x, int y, int z);
};