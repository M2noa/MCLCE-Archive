/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/
#include "Dungeons.h"

#include "world/level/newbiome/RareBiomeSpotLayer.h"

#include "world/level/biome/Biome.h"

RareBiomeSpotLayer::RareBiomeSpotLayer(RandomSeed seedMixup, LayerPtr& parent)
	: Layer(seedMixup) {
	mParent = parent;
}

void RareBiomeSpotLayer::fillArea(LayerData& layerData, int xo, int yo, int w, int h) {
	mParent->fillArea(layerData, xo - 1, yo - 1, w + 2, h + 2);

	for(auto y : range(h)) {
		for(auto x : range(w)) {
			initRandom(x + xo, y + yo);
			const int old = layerData.mParentArea[(x + 1) + (y + 1) * (w + 2)];
			if(nextRandom(57) == 0) {
				if(old == Biome::plains->mId) {
					layerData.mResult[x + y * w] = Biome::plains->mId + Biome::MUTATED_OFFSET;
				}
				else {
					layerData.mResult[x + y * w] = old;
				}
			}
			else {
				layerData.mResult[x + y * w] = old;
			}
		}
	}
	layerData.swap();
}
