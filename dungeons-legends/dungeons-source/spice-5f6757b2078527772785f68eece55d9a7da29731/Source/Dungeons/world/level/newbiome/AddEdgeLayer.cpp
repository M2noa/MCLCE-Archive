/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/
#include "Dungeons.h"

#include "world/level/newbiome/AddEdgeLayer.h"

#include "world/level/biome/Biome.h"

AddEdgeLayer::AddEdgeLayer(RandomSeed seedMixup, LayerPtr& parent, Mode mode) 
	: Layer(seedMixup)
	, mMode(mode) {
	mParent = parent;
}

void AddEdgeLayer::fillArea(LayerData& layerData, int originX, int originY, int width, int height) {
	switch(mMode) {
	default:
	case Mode::COOL_WARM:
		fillCoolWarm(layerData, originX, originY, width, height);
		break;
	case Mode::HEAT_ICE:
		fillHeatIce(layerData, originX, originY, width, height);
		break;
	case Mode::SPECIAL:
		fillIntroduceSpecial(layerData, originX, originY, width, height);
		break;
	}
}

void AddEdgeLayer::fillCoolWarm(LayerData& layerData, int originX, int originY, int width, int height) {
	const int parentX = originX - OFFSET;
	const int parentY = originY - OFFSET;
	const int parentWidth = OFFSET + width + OFFSET;
	const int parentHeight = OFFSET + height + OFFSET;
	mParent->fillArea(layerData, parentX, parentY, parentWidth, parentHeight);

	for(auto y : range(height)) {
		for(auto x : range(width)) {
			initRandom(x + originX, y + originY);

			int value = layerData.mParentArea[(x + OFFSET) + (y + OFFSET) * parentWidth];

			if(value == WARM_ID) {
				const int n = layerData.mParentArea[(x + OFFSET) + (y + OFFSET - 1) * parentWidth];
				const int e = layerData.mParentArea[(x + OFFSET + 1) + (y + OFFSET) * parentWidth];
				const int w = layerData.mParentArea[(x + OFFSET - 1) + (y + OFFSET) * parentWidth];
				const int s = layerData.mParentArea[(x + OFFSET) + (y + OFFSET + 1) * parentWidth];

				const bool coldNeighbor = n == COLD_ID || e == COLD_ID || w == COLD_ID || s == COLD_ID;
				const bool iceNeighbor = n == ICE_ID || e == ICE_ID || w == ICE_ID || s == ICE_ID;
				if(coldNeighbor || iceNeighbor) {
					value = MEDIUM_ID;
				}
			}

			layerData.mResult[x + y * width] = value;
		}
	}

	layerData.swap();
}

void AddEdgeLayer::fillHeatIce(LayerData& layerData, int originX, int originY, int width, int height) {
	const int parentX = originX - OFFSET;
	const int parentY = originY - OFFSET;
	const int parentWidth = OFFSET + width + OFFSET;
	const int parentHeight = OFFSET + height + OFFSET;
	mParent->fillArea(layerData, parentX, parentY, parentWidth, parentHeight);

	for(auto y : range(height)) {
		for(auto x : range(width)) {
			int value = layerData.mParentArea[(x + OFFSET) + (y + OFFSET) * parentWidth];

			if(value == ICE_ID) {
				const int n = layerData.mParentArea[(x + OFFSET) + (y + OFFSET - 1) * parentWidth];
				const int e = layerData.mParentArea[(x + OFFSET + 1) + (y + OFFSET) * parentWidth];
				const int w = layerData.mParentArea[(x + OFFSET - 1) + (y + OFFSET) * parentWidth];
				const int s = layerData.mParentArea[(x + OFFSET) + (y + OFFSET + 1) * parentWidth];

				const bool mediumNeighbor = n == MEDIUM_ID || e == MEDIUM_ID || w == MEDIUM_ID || s == MEDIUM_ID;
				const bool warmNeighbor = n == WARM_ID || e == WARM_ID || w == WARM_ID || s == WARM_ID;

				if(warmNeighbor || mediumNeighbor) {
					value = COLD_ID;
				}
			}

			layerData.mResult[x + y * width] = value;
		}
	}
	layerData.swap();
}

void AddEdgeLayer::fillIntroduceSpecial(LayerData& layerData, int originX, int originY, int width, int height) {
	mParent->fillArea(layerData, originX, originY, width, height);

	for(auto y : range(height)) {
		for(auto x : range(width)) {
			initRandom(x + originX, y + originY);

			int value = layerData.mParentArea[x + y * width];

			if(value != 0 && nextRandom(13) == 0) {
				value |= (((1 + nextRandom(15)) << SPECIAL_SHIFT) & SPECIAL_MASK);
			}

			layerData.mResult[x + y * width] = value;
		}
	}
	layerData.swap();
}
