/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/
#include "Dungeons.h"

#include "world/level/newbiome/DownfallMixerLayer.h"

#include "world/level/biome/Biome.h"

DownfallMixerLayer::DownfallMixerLayer(LayerPtr downfall, LayerPtr& parent, int layer) 
	: Layer(0)
	, mLayer(layer)
	, mDownfall(downfall) {
	mParent = parent;
}

void DownfallMixerLayer::fillArea(LayerData& layerData, int xo, int yo, int w, int h) {
	mParent->fillArea(layerData, xo, yo, w, h);
	LayerData downfallData;

	mDownfall->fillArea(downfallData, xo, yo, w, h);

	for(auto i : range(w * h)) {
		layerData.mResult[i] = downfallData.mParentArea[i] + (Biome::getBiome(layerData.mParentArea[i], Biome::DEFAULT)->getDownfallInt() - downfallData.mParentArea[i]) / (mLayer + 1);
	}
	layerData.swap();
}
