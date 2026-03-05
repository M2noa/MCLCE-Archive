/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/
#include "Dungeons.h"

#include "world/level/newbiome/VoronoiZoom.h"

#include "world/level/biome/Biome.h"

VoronoiZoom::VoronoiZoom(RandomSeed seedMixup, LayerPtr& parent)
	: Layer(seedMixup) {
	mParent = parent;
}

void VoronoiZoom::fillArea(LayerData& layerData, int xo, int yo, int w, int h) {
	xo -= ZOOM / 2;
	yo -= ZOOM / 2;
	const int parentX = xo >> ZOOM_BITS;
	const int parentY = yo >> ZOOM_BITS;
	const int parentWidth = (w >> ZOOM_BITS) + 2;
	const int parentHeight = (h >> ZOOM_BITS) + 2;
	mParent->fillArea(layerData, parentX, parentY, parentWidth, parentHeight);

	const int scaledWidth = (parentWidth - ZOOM_OFFSET) << ZOOM_BITS;
#ifndef PUBLISH
	const int scaledHeight = (parentHeight - ZOOM_OFFSET) << ZOOM_BITS;
	DEBUG_ASSERT(scaledWidth * scaledHeight <= 32 * 32, "Scaled buffer is to small!");
#endif

	int scaledArea[32 * 32];

	for(auto y : range(parentHeight - ZOOM_OFFSET)) {
		int x = 0;
		int upperLeft = layerData.mParentArea[(x + 0) + (y + 0) * parentWidth];
		int downLeft = layerData.mParentArea[(x + 0) + (y + 1) * parentWidth];

		for(; x < parentWidth - ZOOM_OFFSET; x++) {
			const float s = ZOOM * 0.9f;
			initRandom((x + parentX) << ZOOM_BITS, (y + parentY) << ZOOM_BITS);
			const float x0 = (nextRandom(1024) / 1024.0f - 0.5f) * s;
			const float y0 = (nextRandom(1024) / 1024.0f - 0.5f) * s;

			initRandom((x + parentX + 1) << ZOOM_BITS, (y + parentY) << ZOOM_BITS);
			const float x1 = (nextRandom(1024) / 1024.0f - 0.5f) * s + ZOOM;
			const float y1 = (nextRandom(1024) / 1024.0f - 0.5f) * s;

			initRandom((x + parentX) << ZOOM_BITS, (y + parentY + 1) << ZOOM_BITS);
			const float x2 = (nextRandom(1024) / 1024.0f - 0.5f) * s;
			const float y2 = (nextRandom(1024) / 1024.0f - 0.5f) * s + ZOOM;

			initRandom((x + parentX + 1) << ZOOM_BITS, (y + parentY + 1) << ZOOM_BITS);
			const float x3 = (nextRandom(1024) / 1024.0f - 0.5f) * s + ZOOM;
			const float y3 = (nextRandom(1024) / 1024.0f - 0.5f) * s + ZOOM;

			int upperRight = layerData.mParentArea[(x + 1) + (y + 0) * parentWidth] & 0xFF;
			int downRight = layerData.mParentArea[(x + 1) + (y + 1) * parentWidth] & 0xFF;

			for(auto yy : range(ZOOM)) {
				int scaledPosition = ((y << ZOOM_BITS) + yy) * scaledWidth + (x << ZOOM_BITS);

				for(auto xx : range(ZOOM)) {
					float d0 = ((yy - y0) * (yy - y0) + (xx - x0) * (xx - x0));
					float d1 = ((yy - y1) * (yy - y1) + (xx - x1) * (xx - x1));
					float d2 = ((yy - y2) * (yy - y2) + (xx - x2) * (xx - x2));
					float d3 = ((yy - y3) * (yy - y3) + (xx - x3) * (xx - x3));

					if(d0 < d1 && d0 < d2 && d0 < d3) {
						scaledArea[scaledPosition++] = upperLeft;
					}
					else if(d1 < d0 && d1 < d2 && d1 < d3) {
						scaledArea[scaledPosition++] = upperRight;
					}
					else if(d2 < d0 && d2 < d1 && d2 < d3) {
						scaledArea[scaledPosition++] = downLeft;
					}
					else {
						scaledArea[scaledPosition++] = downRight;
					}
				}
			}

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
