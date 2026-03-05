/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/
#include "Dungeons.h"

#include "world/level/newbiome/RegionHillsLayer.h"

#include "Core/Debug/Log.h"
#include "world/level/biome/Biome.h"

RegionHillsLayer::RegionHillsLayer(RandomSeed seedMixup, LayerPtr& parent, LayerPtr& riverLayer)
	: Layer(seedMixup)
	, mRiverLayer(riverLayer) {
	mParent = parent;
}

void RegionHillsLayer::fillArea(LayerData& layerData, int xo, int yo, int w, int h) {
	mParent->fillArea(layerData, xo - 1, yo - 1, w + 2, h + 2);
	LayerData riverData;
	mRiverLayer->fillArea(riverData, xo - 1, yo - 1, w + 2, h + 2);

	for(auto y : range(h)) {
		for(auto x : range(w)) {
			initRandom(x + xo, y + yo);
			const int old = layerData.mParentArea[(x + 1) + (y + 1) * (w + 2)];
			const int river = riverData.mParentArea[(x + 1) + (y + 1) * (w + 2)];
			const bool isSpecialHills = ((river - 2) % 29) == 0;
			if(old > 255) {
				LOGW("old! %d\n", old);
			}
			if(old != 0 && river >= 2 && ((river - 2) % 29) == 1 && old < Biome::MUTATED_OFFSET) {
				if(Biome::getBiome(old + Biome::MUTATED_OFFSET) != nullptr) {
					layerData.mResult[x + y * w] = old + Biome::MUTATED_OFFSET;
				}
				else {
					layerData.mResult[x + y * w] = old;
				}
			}
			else if(nextRandom(3) == 0 || isSpecialHills) {
				int next = old;
				if(old == Biome::desert->mId) {
					next = Biome::desertHills->mId;
				}
				else if(old == Biome::forest->mId) {
					next = Biome::forestHills->mId;
				}
				else if(old == Biome::birchForest->mId) {
					next = Biome::birchForestHills->mId;
				}
				else if(old == Biome::roofedForest->mId) {
					next = Biome::plains->mId;
				}
				else if(old == Biome::taiga->mId) {
					next = Biome::taigaHills->mId;
				}
				else if(old == Biome::redwoodTaiga->mId) {
					next = Biome::redwoodTaigaHills->mId;
				}
				else if(old == Biome::taigaCold->mId) {
					next = Biome::taigaColdHills->mId;
				}
				else if(old == Biome::plains->mId) {
					if(nextRandom(3) == 0) {
						next = Biome::forestHills->mId;
					}
					else {
						next = Biome::forest->mId;
					}
				}
				else if(old == Biome::iceFlats->mId) {
					next = Biome::iceMountains->mId;
				}
				else if(old == Biome::jungle->mId) {
					next = Biome::jungleHills->mId;
				}
				else if(old == Biome::ocean->mId) {
					next = Biome::deepOcean->mId;
				}
				else if(old == Biome::extremeHills->mId) {
					next = Biome::extremeHillsWithTrees->mId;
				}
				else if(old == Biome::savanna->mId) {
					next = Biome::savannaRock->mId;
				}
				else if(_isSame(old, Biome::mesaRock->mId)) {
					next = Biome::mesa->mId;
				}
				else if(old == Biome::deepOcean->mId) {
					if(nextRandom(3) == 0) {
						const int select = nextRandom(2);
						if(select == 0) {
							next = Biome::plains->mId;
						}
						else {
							next = Biome::forest->mId;
						}
					}
				}
				if(isSpecialHills && next != old) {
					if(Biome::getBiome(next + Biome::MUTATED_OFFSET) != nullptr) {
						next = next + Biome::MUTATED_OFFSET;
					}
					else {
						next = old;
					}
				}
				if(next == old) {
					layerData.mResult[x + y * w] = old;
				}
				else {
					const int _n = layerData.mParentArea[(x + 1) + (y + 1 - 1) * (w + 2)];
					const int _e = layerData.mParentArea[(x + 1 + 1) + (y + 1) * (w + 2)];
					const int _w = layerData.mParentArea[(x + 1 - 1) + (y + 1) * (w + 2)];
					const int _s = layerData.mParentArea[(x + 1) + (y + 1 + 1) * (w + 2)];

					int c = 0;
					if(_isSame(_n, old)) {
						c++;
					}
					if(_isSame(_e, old)) {
						c++;
					}
					if(_isSame(_w, old)) {
						c++;
					}
					if(_isSame(_s, old)) {
						c++;
					}
					if(c >= 3) {
						layerData.mResult[x + y * w] = next;
					}
					else {
						layerData.mResult[x + y * w] = old;
					}
				}
			}
			else {
				layerData.mResult[x + y * w] = old;
			}
		}
	}
	layerData.swap();
}
