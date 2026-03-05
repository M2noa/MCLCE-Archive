/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/
#include "Dungeons.h"

#include "world/level/newbiome/AddMushroomIslandLayer.h"

#include "world/level/biome/Biome.h"

AddMushroomIslandLayer::AddMushroomIslandLayer(RandomSeed seedMixup, LayerPtr& parent) :
	Layer(seedMixup) {
	mParent = parent;
}

void AddMushroomIslandLayer::fillArea(LayerData& layerData, int xo, int yo, int w, int h) {
	const int px = xo - 1;
	const int py = yo - 1;
	const int pw = w + 2;
	const int ph = h + 2;

	mParent->fillArea(layerData, px, py, pw, ph);

	for(auto y : range(h)) {
		for(auto x : range(w)) {
			const int n1 = layerData.mParentArea[(x + 0) + (y + 0) * pw];
			const int n2 = layerData.mParentArea[(x + 2) + (y + 0) * pw];
			const int n3 = layerData.mParentArea[(x + 0) + (y + 2) * pw];
			const int n4 = layerData.mParentArea[(x + 2) + (y + 2) * pw];
			const int c = layerData.mParentArea[(x + 1) + (y + 1) * pw];

			initRandom(x + xo, y + yo);
			if(c == 0 && (n1 == 0 && n2 == 0 && n3 == 0 && n4 == 0) && nextRandom(100) == 0) {
				layerData.mResult[x + y * w] = Biome::mushroomIsland->mId;
			}
			else {
				layerData.mResult[x + y * w] = c;
			}
		}
	}
	layerData.swap();
}
