#pragma once

#include "world/level/biome/MutatedBiome.h"

class MutatedSavannaBiome : public MutatedBiome {
public:

	MutatedSavannaBiome(int id, Biome* contained);
	void buildSurfaceAt(Random& random, BlockVolume& blocks, const BlockPos& pos, float depthValue, Height seaLevel) override;

};
