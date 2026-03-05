#pragma once
#include "Feature.h"

class Level;

class DeadBushFeature : public Feature
{
private:
	int tile;

public:
	DeadBushFeature (int tile);

	virtual bool place(Level *level, Random *random, int x, int y, int z);
};