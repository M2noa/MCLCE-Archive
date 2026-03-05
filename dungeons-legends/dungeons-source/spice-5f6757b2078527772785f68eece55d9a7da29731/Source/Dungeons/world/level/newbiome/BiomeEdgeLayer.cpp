/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/
#include "Dungeons.h"

#include "world/level/newbiome/BiomeEdgeLayer.h"

#include "world/level/biome/Biome.h"

BiomeEdgeLayer::BiomeEdgeLayer(RandomSeed seedMixup, LayerPtr& parent, bool generateJungles)
	: Layer(seedMixup)
	, mGenerateJungles(generateJungles) {
	mParent = parent;
}

void BiomeEdgeLayer::fillArea(LayerData& layerData, int xo, int yo, int w, int h) {
	mParent->fillArea(layerData, xo - 1, yo - 1, w + 2, h + 2);

	for(auto y : range(h)) {
		for(auto x : range(w)) {
			initRandom(x + xo, y + yo);
			const int old = layerData.mParentArea[(x + 1) + (y + 1) * (w + 2)];

			if(checkEdge(layerData, x, y, w, old, Biome::extremeHills->mId, Biome::smallerExtremeHills->mId)) {
			}
			else if(checkEdgeStrict(layerData, x, y, w, old, Biome::mesaRock->mId, Biome::mesa->mId)) {
			}
			else if(checkEdgeStrict(layerData, x, y, w, old, Biome::mesaClearRock->mId, Biome::mesa->mId)) {
			}
			else if(checkEdgeStrict(layerData, x, y, w, old, Biome::redwoodTaiga->mId, Biome::taiga->mId)) {
			}
			else if(old == Biome::desert->mId) {
				int _n = layerData.mParentArea[(x + 1) + (y + 1 - 1) * (w + 2)];
				int _e = layerData.mParentArea[(x + 1 + 1) + (y + 1) * (w + 2)];
				int _w = layerData.mParentArea[(x + 1 - 1) + (y + 1) * (w + 2)];
				int _s = layerData.mParentArea[(x + 1) + (y + 1 + 1) * (w + 2)];
				if(_n == Biome::iceFlats->mId || _e == Biome::iceFlats->mId || _w == Biome::iceFlats->mId || _s == Biome::iceFlats->mId) {
					layerData.mResult[x + y * w] = Biome::extremeHillsWithTrees->mId;
				}
				else {
					layerData.mResult[x + y * w] = old;
				}
			}
			else if(mGenerateJungles && old == Biome::swampland->mId) {
				// don't allow edge to desert or frozen land
				int _n = layerData.mParentArea[(x + 1) + (y + 1 - 1) * (w + 2)];
				int _e = layerData.mParentArea[(x + 1 + 1) + (y + 1) * (w + 2)];
				int _w = layerData.mParentArea[(x + 1 - 1) + (y + 1) * (w + 2)];
				int _s = layerData.mParentArea[(x + 1) + (y + 1 + 1) * (w + 2)];
				if(_n == Biome::desert->mId || _e == Biome::desert->mId || _w == Biome::desert->mId || _s == Biome::desert->mId || _n == Biome::taigaCold->mId || _e == Biome::taigaCold->mId
					|| _w == Biome::taigaCold->mId || _s == Biome::taigaCold->mId || _n == Biome::iceFlats->mId || _e == Biome::iceFlats->mId || _w == Biome::iceFlats->mId || _s == Biome::iceFlats->mId) {
					layerData.mResult[x + y * w] = Biome::plains->mId;
				}
				else if(_n == Biome::jungle->mId || _s == Biome::jungle->mId || _e == Biome::jungle->mId || _w == Biome::jungle->mId) {
					layerData.mResult[x + y * w] = Biome::jungleEdge->mId;
				}
				else {
					layerData.mResult[x + y * w] = old;
				}
			}
			else {
				layerData.mResult[x + y * w] = old;
			}
		}
	}
	layerData.swap();
}

bool BiomeEdgeLayer::checkEdge(LayerData& layerData, int x, int y, int w, int old, int checkFor, int addEdge) {
	if(_isSame(old, checkFor)) {
		const int _n = layerData.mParentArea[(x + 1) + (y + 1 - 1) * (w + 2)];
		const int _e = layerData.mParentArea[(x + 1 + 1) + (y + 1) * (w + 2)];
		const int _w = layerData.mParentArea[(x + 1 - 1) + (y + 1) * (w + 2)];
		const int _s = layerData.mParentArea[(x + 1) + (y + 1 + 1) * (w + 2)];

		if(!isValidTemperatureEdge(_n, checkFor) || !isValidTemperatureEdge(_e, checkFor) || !isValidTemperatureEdge(_w, checkFor) || !isValidTemperatureEdge(_s, checkFor)) {
			layerData.mResult[x + y * w] = addEdge;
		}
		else {
			layerData.mResult[x + y * w] = old;
		}
		return true;
	}
	return false;
}

bool BiomeEdgeLayer::checkEdgeStrict(LayerData& layerData, int x, int y, int w, int old, int checkFor, int addEdge) {
	if(old == checkFor) {
		const int _n = layerData.mParentArea[(x + 1) + (y + 1 - 1) * (w + 2)];
		const int _e = layerData.mParentArea[(x + 1 + 1) + (y + 1) * (w + 2)];
		const int _w = layerData.mParentArea[(x + 1 - 1) + (y + 1) * (w + 2)];
		const int _s = layerData.mParentArea[(x + 1) + (y + 1 + 1) * (w + 2)];

		if(!_isSame(_n, checkFor) || !_isSame(_e, checkFor) || !_isSame(_w, checkFor) || !_isSame(_s, checkFor)) {
			layerData.mResult[x + y * w] = addEdge;
		}
		else {
			layerData.mResult[x + y * w] = old;
		}
		return true;
	}
	return false;
}

bool BiomeEdgeLayer::isValidTemperatureEdge(int a, int b) {
	if(_isSame(a, b)) {
		return true;
	}

	const Biome* biome1 = Biome::getBiome(a);
	const Biome* biome2 = Biome::getBiome(b);

	if(biome1 != nullptr && biome2 != nullptr) {
		Biome::BiomeTempCategory at = biome1->getTemperatureCategory();
		Biome::BiomeTempCategory bt = biome2->getTemperatureCategory();
		return at == bt || at == Biome::BiomeTempCategory::MEDIUM || bt == Biome::BiomeTempCategory::MEDIUM;
	}
	return false;
}
