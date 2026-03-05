/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/
#include "Dungeons.h"

#include "world/level/newbiome/RiverMixerLayer.h"

#include "world/level/biome/Biome.h"

RiverMixerLayer::RiverMixerLayer(RandomSeed seedMixup, LayerPtr& biomes, LayerPtr& rivers)
	: Layer(seedMixup)
	, mBiomes(biomes)
	, mRivers(rivers) {
	mParent = mParent;
}

void RiverMixerLayer::init(int64_t seed) {
	mBiomes->init(seed);
	mRivers->init(seed);
	Layer::init(seed);
}

void RiverMixerLayer::fillArea(LayerData& layerData, int xo, int yo, int w, int h) {
	mBiomes->fillArea(layerData, xo, yo, w, h);
	LayerData riverData;

	mRivers->fillArea(riverData, xo, yo, w, h);

	for(auto i : range(w * h)) {
		if(layerData.mParentArea[i] == Biome::ocean->mId || layerData.mParentArea[i] == Biome::deepOcean->mId) {
			layerData.mResult[i] = layerData.mParentArea[i];
		}
		else {
			if(riverData.mParentArea[i] == Biome::river->mId) {
				if(layerData.mParentArea[i] == Biome::iceFlats->mId) {
					layerData.mResult[i] = Biome::frozenRiver->mId;
				}
				else if(layerData.mParentArea[i] == Biome::mushroomIsland->mId || layerData.mParentArea[i] == Biome::mushroomIslandShore->mId) {
					layerData.mResult[i] = Biome::mushroomIslandShore->mId;
				}
				else {
					layerData.mResult[i] = riverData.mParentArea[i] & 0xff;
				}
			}
			else {
				layerData.mResult[i] = layerData.mParentArea[i];
			}
		}
	}
	layerData.swap();
}
