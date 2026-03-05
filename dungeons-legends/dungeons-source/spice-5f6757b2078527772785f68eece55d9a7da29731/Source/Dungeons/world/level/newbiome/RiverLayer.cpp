/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/
#include "Dungeons.h"

#include "world/level/newbiome/RiverLayer.h"

#include "world/level/biome/Biome.h"

RiverLayer::RiverLayer(RandomSeed seedMixup, LayerPtr& parent) :
	Layer(seedMixup) {
	mParent = parent;
}

void RiverLayer::fillArea(LayerData& layerData, int xo, int yo, int w, int h) {
	int px = xo - 1;
	int py = yo - 1;
	int pw = w + 2;
	int ph = h + 2;
	mParent->fillArea(layerData, px, py, pw, ph);

	for(auto y : range(h)) {
		for(auto x : range(w)) {
			int l = riverFilter(layerData.mParentArea[(x + 0) + (y + 1) * pw]);
			int r = riverFilter(layerData.mParentArea[(x + 2) + (y + 1) * pw]);
			int u = riverFilter(layerData.mParentArea[(x + 1) + (y + 0) * pw]);
			int d = riverFilter(layerData.mParentArea[(x + 1) + (y + 2) * pw]);
			int c = riverFilter(layerData.mParentArea[(x + 1) + (y + 1) * pw]);
			if(c != l || c != u || c != r || c != d) {
				layerData.mResult[x + y * w] = Biome::river->mId;
			}
			else {
				layerData.mResult[x + y * w] = -1;
			}
		}
	}
	layerData.swap();
}

int RiverLayer::riverFilter(int value) {
	if(value >= 2) {
		return 2 + (value & 1);
	}
	return value;
}
