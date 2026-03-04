#pragma once
#include "Feature.h"

class TallGrassFeature : public Feature
{
private:
	int tile;
	int type;

public:
	TallGrassFeature (int tile, int type);

	virtual bool place(Level *level, Random *random, int x, int y, int z);
};