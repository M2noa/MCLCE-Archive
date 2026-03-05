#pragma once

#include "world/level/biome/Biome.h"

class HellBiome : public Biome {
public:

	HellBiome(int id);
	virtual float getCreatureProbability() override;
};
