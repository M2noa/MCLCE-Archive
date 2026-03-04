#pragma once

#include "Biome.h"

class RiverBiome : public Biome
{
public:
	RiverBiome(int id) : Biome(id)
	{
		friendlies.clear();
		friendlies_chicken.clear();	// 4J added since chicken now separated from main friendlies
		friendlies_wolf.clear();	// 4J added since wolf now separated from main friendlies
	}
};