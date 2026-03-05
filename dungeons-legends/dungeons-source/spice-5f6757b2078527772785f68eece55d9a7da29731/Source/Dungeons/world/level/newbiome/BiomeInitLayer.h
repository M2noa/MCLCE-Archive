/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/
#pragma once

#include "world/level/newbiome/Layer.h"

class Biome;
class BiomeInitLayer : public Layer {
public:

	static const int NUM_WARM_BIOMES = 6;
	static const int NUM_COLD_BIOMES = 4;
	static const int NUM_ICE_BIOMES = 4;

	BiomeInitLayer(RandomSeed seedMixup, LayerPtr& parent, GeneratorType generator);
	virtual void fillArea(LayerData& layerData, int xo, int yo, int w, int h) override;

private:

	Biome* mWarmBiomes[NUM_WARM_BIOMES];
	std::vector<Biome*> mMediumBiomes;
	Biome* mColdBiomes[NUM_COLD_BIOMES];
	Biome* mIceBiomes[NUM_ICE_BIOMES];
};
