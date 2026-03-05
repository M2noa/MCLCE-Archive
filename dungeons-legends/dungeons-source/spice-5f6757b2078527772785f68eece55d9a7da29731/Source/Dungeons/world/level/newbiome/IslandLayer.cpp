/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/
#include "Dungeons.h"

#include "world/level/newbiome/IslandLayer.h"

#include "world/level/biome/Biome.h"

IslandLayer::IslandLayer(RandomSeed seedMixup)
	: Layer(seedMixup) {
}

void IslandLayer::fillArea(LayerData& layerData, int xo, int yo, int w, int h) {
	for(auto y : range(h)) {
		for(auto x : range(w)) {
			initRandom(xo + x, yo + y);
			layerData.mResult[x + y * w] = (nextRandom(10) == 0) ? 1 : 0;
		}
	}
	// if (0, 0) is located here, place an island
	if(xo > -w && xo <= 0 && yo > -h && yo <= 0) {
		layerData.mResult[-xo + -yo * w] = 1;
	}
	layerData.swap();
}
