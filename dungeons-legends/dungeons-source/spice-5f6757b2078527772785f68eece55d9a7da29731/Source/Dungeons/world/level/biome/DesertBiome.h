#pragma once

#include "world/level/biome/Biome.h"

class DesertBiome : public Biome {
public:

	DesertBiome(int id);
	void decorate(BlockSource* source, Random& random, const BlockPos& origin, bool legacy, float worldLimit) override;

};
