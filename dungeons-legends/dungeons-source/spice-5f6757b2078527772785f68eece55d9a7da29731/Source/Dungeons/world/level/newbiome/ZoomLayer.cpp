/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/
#include "Dungeons.h"

#include "world/level/newbiome/ZoomLayer.h"

#include "world/level/biome/Biome.h"

ZoomLayer::ZoomLayer(RandomSeed seedMixup, LayerPtr parent) :
	Layer(seedMixup) {
	mParent = parent;
}

void ZoomLayer::fillArea(LayerData& layerData, int xo, int yo, int w, int h) {
	const int parentX = xo >> ZOOM_BITS;
	const int parentY = yo >> ZOOM_BITS;
	const int parentWidth = (w >> ZOOM_BITS) + 2;
	const int parentHeight = (h >> ZOOM_BITS) + 2;

	mParent->fillArea(layerData, parentX, parentY, parentWidth, parentHeight);

	const int scaledWidth = (parentWidth - ZOOM_OFFSET) << ZOOM_BITS;
	const int scaledHeight = (parentHeight - ZOOM_OFFSET) << ZOOM_BITS;
	DEBUG_ASSERT(scaledWidth * scaledHeight <= 32 * 32, "Zoomlayer is to small!");
	UNUSED_VARIABLE(scaledHeight);
	int scaledArea[32 * 32];

	for(auto y : range(parentHeight - ZOOM_OFFSET)) {
		int scaledPosition = (y << ZOOM_BITS) * scaledWidth;

		int x = 0;
		int upperLeft = layerData.mParentArea[(x + 0) + (y + 0) * parentWidth];
		int downLeft = layerData.mParentArea[(x + 0) + (y + 1) * parentWidth];

		for(; x < parentWidth - ZOOM_OFFSET; x++) {
			initRandom((x + parentX) << ZOOM_BITS, (y + parentY) << ZOOM_BITS);

			const int upperRight = layerData.mParentArea[(x + 1) + (y + 0) * parentWidth];
			const int downRight = layerData.mParentArea[(x + 1) + (y + 1) * parentWidth];

			scaledArea[scaledPosition] = upperLeft;
			int randomData[] = {
				upperLeft, downLeft
			};
			scaledArea[scaledPosition++ + scaledWidth] = _random(randomData, 2);
			int randomData2[] = {
				upperLeft, upperRight
			};
			scaledArea[scaledPosition] = _random(randomData2, 2);
			scaledArea[scaledPosition++ + scaledWidth] = _modeOrRandom(upperLeft, upperRight, downLeft, downRight);

			upperLeft = upperRight;
			downLeft = downRight;
		}
	}

	for(auto y : range(h)) {
		int* scaledOffsetPosition = scaledArea + ((y + (yo & ZOOM_MASK)) * scaledWidth + (xo & ZOOM_MASK));
		int* destOffsetPosition = layerData.mResult + (y * w);
		memcpy(destOffsetPosition, scaledOffsetPosition, sizeof(int) * w);
	}
	layerData.swap();
}

LayerPtr ZoomLayer::zoom(RandomSeed seed, LayerPtr& sup, int count) {
	LayerPtr result = sup;

	for (auto i : range(count)) {
		UNUSED_VARIABLE(i);
		result = make_shared<ZoomLayer>(seed + 1, result);
	}
	return result;
}
