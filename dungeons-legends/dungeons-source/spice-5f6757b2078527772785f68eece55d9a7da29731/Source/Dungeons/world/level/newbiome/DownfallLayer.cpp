/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/
#include "Dungeons.h"

#include "world/level/newbiome/DownfallLayer.h"

#include "world/level/biome/Biome.h"

DownfallLayer::DownfallLayer( RandomSeed seedMixup, LayerPtr& parent) 
	: Layer(0) {
	mParent = parent;
}

void DownfallLayer::fillArea(LayerData& layerData, int xo, int yo, int w, int h) {
	mParent->fillArea(layerData, xo, yo, w, h);

	for(auto i : range(w * h)) {
		layerData.mResult[i] = Biome::getBiome(layerData.mParentArea[i], Biome::DEFAULT)->getDownfallInt();
	}
	layerData.swap();
}
