/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/
#include "Dungeons.h"

#include "world/level/newbiome/SmoothZoomLayer.h"

#include "world/level/biome/Biome.h"

SmoothZoomLayer::SmoothZoomLayer(RandomSeed seedMixup, LayerPtr& parent)
	: Layer(seedMixup) {
	mParent = parent;
}

void SmoothZoomLayer::fillArea(LayerData& layerData, int xo, int yo, int w, int h) {
	const int px = xo >> 1;
	const int py = yo >> 1;
	const int pw = (w >> 1) + 3;
	const int ph = (h >> 1) + 3;
	mParent->fillArea(layerData, px, py, pw, ph);

	DEBUG_ASSERT(pw * 2 * ph * 2 <= 32 * 32, "Tmp layer is to small!");
	int tmp[32 * 32];

	const int ww = (pw << 1);

	for(auto y : range(ph - 1)) {
		const int ry = y << 1;
		int pp = ry * ww;
		int ul = layerData.mParentArea[(0 + 0) + (y + 0) * pw];
		int dl = layerData.mParentArea[(0 + 0) + (y + 1) * pw];

		for(auto x : range(pw - 1)) {
			initRandom((x + px) << 1, (y + py) << 1);

			int ur = layerData.mParentArea[(x + 1) + (y + 0) * pw];
			int dr = layerData.mParentArea[(x + 1) + (y + 1) * pw];

			tmp[pp] = ul;
			tmp[pp++ + ww] = ul + (dl - ul) * (nextRandom(256)) / 256;
			tmp[pp] = ul + (ur - ul) * (nextRandom(256)) / 256;

			int a = ul + (ur - ul) * (nextRandom(256)) / 256;
			int b = dl + (dr - dl) * (nextRandom(256)) / 256;
			tmp[pp++ + ww] = a + (b - a) * (nextRandom(256)) / 256;

			ul = ur;
			dl = dr;
		}
	}

	for(auto y : range(h)) {
		int* tmpOffsetPosition = tmp + ((y + (yo & 1)) * (pw << 1) + (xo & 1)) * sizeof(int);
		int* destOffsetPosition = layerData.mResult + (y * w * sizeof(int));
		memcpy(destOffsetPosition, tmpOffsetPosition, sizeof(int) * w);
	}
	layerData.swap();
}

LayerPtr SmoothZoomLayer::zoom(RandomSeed seed, LayerPtr& sup, int count) {
	LayerPtr result = sup;

	for(auto i : range(count)) {
		result = make_shared<SmoothZoomLayer>(seed + i, result);
	}
	return result;
}
