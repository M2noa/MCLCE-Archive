#pragma once
#include "Biome.h"

class ForestBiome : public Biome
{
public:
    ForestBiome(int id);

	virtual Feature *getTreeFeature(Random *random);
};
