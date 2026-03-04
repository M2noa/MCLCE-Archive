#pragma once
#include "Biome.h"

class RainforestBiome : public Biome
{
public:
	RainforestBiome(int id);
	virtual Feature *getTreeFeature(Random *random);
};