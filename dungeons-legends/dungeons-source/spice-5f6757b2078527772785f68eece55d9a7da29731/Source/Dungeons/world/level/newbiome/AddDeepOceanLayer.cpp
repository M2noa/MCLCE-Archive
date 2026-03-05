/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/
#include "Dungeons.h"

#include "world/level/newbiome/AddDeepOceanLayer.h"

#include "world/level/biome/Biome.h"

AddDeepOceanLayer::AddDeepOceanLayer(RandomSeed seedMixup, LayerPtr& parent) 
	: Layer(seedMixup) {
	mParent = parent;
}

void AddDeepOceanLayer::fillArea(LayerData& layerData, int originX, int originY, int width, int height) {
	const int px = originX - 1;
	const int py = originY - 1;
	const int pw = width + 2;
	const int ph = height + 2;
	mParent->fillArea(layerData, px, py, pw, ph);

	for(auto y : range(height)) {
		for(auto x : range(width)) {
			const int _n = layerData.mParentArea[(x + 1) + (y + 1 - 1) * (width + 2)];
			const int _e = layerData.mParentArea[(x + 1 + 1) + (y + 1) * (width + 2)];
			const int _w = layerData.mParentArea[(x + 1 - 1) + (y + 1) * (width + 2)];
			const int _s = layerData.mParentArea[(x + 1) + (y + 1 + 1) * (width + 2)];

			const int c = layerData.mParentArea[(x + 1) + (y + 1) * pw];
			int count = 0;
			if(_n == 0) {
				count++;
			}
			if(_e == 0) {
				count++;
			}
			if(_w == 0) {
				count++;
			}
			if(_s == 0) {
				count++;
			}

			if(c == 0 && count > 3) {
				layerData.mResult[x + y * width] = Biome::deepOcean->mId;
			}
			else {
				layerData.mResult[x + y * width] = c;
			}
		}
	}
	layerData.swap();
}
