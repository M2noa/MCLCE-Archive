/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/
#include "Dungeons.h"

#include "world/level/newbiome/AddSnowLayer.h"

#include "world/level/biome/Biome.h"

AddSnowLayer::AddSnowLayer(RandomSeed seedMixup, LayerPtr& parent) :
	Layer(seedMixup) {
	mParent = parent;
}

void AddSnowLayer::fillArea(LayerData& layerData, int xo, int yo, int w, int h) {
	const int px = xo - 1;
	const int py = yo - 1;
	const int pw = w + 2;
	const int ph = h + 2;
	mParent->fillArea(layerData, px, py, pw, ph);

	for(auto y : range(h)) {
		for(auto x : range(w)) {
			const int c = layerData.mParentArea[(x + 1) + (y + 1) * pw];
			initRandom(x + xo, y + yo);
			if(c == 0) {
				layerData.mResult[x + y * w] = 0;
			}
			else {
				int r = nextRandom(6);
				if(r == 0) {
					r = Layer::ICE_ID;
				}
				else if(r <= 1) {
					r = Layer::COLD_ID;
				}
				else {
					r = Layer::WARM_ID;
				}
				layerData.mResult[x + y * w] = r;
			}
		}
	}
	layerData.swap();
}
