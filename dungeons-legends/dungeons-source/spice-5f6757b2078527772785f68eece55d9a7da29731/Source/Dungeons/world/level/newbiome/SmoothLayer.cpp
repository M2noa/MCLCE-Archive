/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/
#include "Dungeons.h"

#include "world/level/newbiome/SmoothLayer.h"

#include "world/level/biome/Biome.h"

SmoothLayer::SmoothLayer(RandomSeed seedMixup, LayerPtr& parent)
	: Layer(seedMixup) {
	mParent = parent;
}

void SmoothLayer::fillArea(LayerData& layerData, int xo, int yo, int w, int h) {
	int px = xo - 1;
	int py = yo - 1;
	int pw = w + 2;
	int ph = h + 2;
	mParent->fillArea(layerData, px, py, pw, ph);

	for(auto y : range(h)) {
		for(auto x : range(w)) {
			int l = layerData.mParentArea[(x + 0) + (y + 1) * pw];
			int r = layerData.mParentArea[(x + 2) + (y + 1) * pw];
			int u = layerData.mParentArea[(x + 1) + (y + 0) * pw];
			int d = layerData.mParentArea[(x + 1) + (y + 2) * pw];
			int c = layerData.mParentArea[(x + 1) + (y + 1) * pw];
			if(l == r && u == d) {
				initRandom((x + xo), (y + yo));
				if(nextRandom(2) == 0) {
					c = l;
				}
				else {
					c = u;
				}
			}
			else {
				if(l == r) {
					c = l;
				}
				if(u == d) {
					c = u;
				}
			}
			layerData.mResult[x + y * w] = c;
		}
	}
	layerData.swap();
}
