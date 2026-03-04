#pragma once

#include "Feature.h"

class GroundBushFeature : public Feature
{
private:
	int leafTileType;
	int trunkTileType;

public:
	GroundBushFeature(int trunkType, int leafType);

	virtual bool place(Level *level, Random *random, int x, int y, int z);
};