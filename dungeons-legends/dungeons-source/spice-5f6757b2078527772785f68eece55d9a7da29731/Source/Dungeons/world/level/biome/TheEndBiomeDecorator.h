/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/
#pragma once

#include "world/level/biome/BiomeDecorator.h"
#include "world/level/levelgen/feature/SpikeFeature.h"

class Level;

class TheEndBiomeDecorator : public BiomeDecorator {
public:
	virtual void decorate(BlockSource* source, Random& random, Biome* biome, const BlockPos& origin, bool legacy, float limit) override;

	static std::vector<SpikeFeature::EndSpike> getSpikesForLevel(Level& level);

private:
	static std::vector<SpikeFeature::EndSpike> _loadSpikes(Random& random);
};
