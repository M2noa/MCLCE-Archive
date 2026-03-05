#pragma once

#include "world/level/biome/Biome.h"

class RiverBiome : public Biome {
public:

	RiverBiome(int id) :
		Biome(id, Biome::BiomeType::River) {
		_friendlies.clear();
	}

};
