/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/
#include "Dungeons.h"

#include "world/level/newbiome/RemoveTooMuchOceanLayer.h"

#include "world/level/biome/Biome.h"

RemoveTooMuchOceanLayer::RemoveTooMuchOceanLayer(RandomSeed seedMixup, LayerPtr& parent)
	: Layer(seedMixup) {
	mParent = parent;
}

void RemoveTooMuchOceanLayer::fillArea(LayerData& layerData, int xo, int yo, int w, int h) {
	const int px = xo - 1;
	const int py = yo - 1;
	const int pw = w + 2;
	const int ph = h + 2;
	mParent->fillArea(layerData, px, py, pw, ph);

	for(auto y : range(h)) {
		for(auto x : range(w)) {
			const int _n = layerData.mParentArea[(x + 1) + (y + 1 - 1) * (w + 2)];
			const int _e = layerData.mParentArea[(x + 1 + 1) + (y + 1) * (w + 2)];
			const int _w = layerData.mParentArea[(x + 1 - 1) + (y + 1) * (w + 2)];
			const int _s = layerData.mParentArea[(x + 1) + (y + 1 + 1) * (w + 2)];

			const int c = layerData.mParentArea[(x + 1) + (y + 1) * pw];
			layerData.mResult[x + y * w] = c;
			initRandom(x + xo, y + yo);
			if(c == 0 && _n == 0 && _e == 0 && _w == 0 && _s == 0 && nextRandom(2) == 0) {
				layerData.mResult[x + y * w] = Layer::WARM_ID;
			}
		}
	}
	layerData.swap();
}
