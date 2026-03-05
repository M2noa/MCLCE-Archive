/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/
#include "Dungeons.h"

#include "world/level/newbiome/Layer.h"

#include "world/level/biome/Biome.h"
#include "world/level/GeneratorType.h"
#include "world/level/newbiome/AddDeepOceanLayer.h"
#include "world/level/newbiome/AddEdgeLayer.h"
#include "world/level/newbiome/AddIslandLayer.h"
#include "world/level/newbiome/AddMushroomIslandLayer.h"
#include "world/level/newbiome/AddSnowLayer.h"
#include "world/level/newbiome/BiomeInitLayer.h"
#include "world/level/newbiome/BiomeEdgeLayer.h"
#include "world/level/newbiome/IslandLayer.h"
#include "world/level/newbiome/FuzzyZoomLayer.h"
#include "world/level/newbiome/RareBiomeSpotLayer.h"
#include "world/level/newbiome/RegionHillsLayer.h"
#include "world/level/newbiome/RemoveTooMuchOceanLayer.h"
#include "world/level/newbiome/RiverInitLayer.h"
#include "world/level/newbiome/RiverLayer.h"
#include "world/level/newbiome/RiverMixerLayer.h"
#include "world/level/newbiome/ShoreLayer.h"
#include "world/level/newbiome/SmoothLayer.h"
#include "world/level/newbiome/VoronoiZoom.h"

void LayerData::swap() {
	std::swap(mParentArea, mResult);
}

void Layer::getDefaultLayers(int64_t seed, LayerPtr& biomeLayer, LayerPtr& zoomedLayer, GeneratorType genType) {
	LayerPtr islandLayer = make_shared<IslandLayer>(1);
	islandLayer = make_shared<FuzzyZoomLayer>(2000, islandLayer);
	islandLayer = make_shared<AddIslandLayer>(1, islandLayer);
	islandLayer = make_shared<ZoomLayer>(2001, islandLayer);
	islandLayer = make_shared<AddIslandLayer>(2, islandLayer);
	islandLayer = make_shared<AddIslandLayer>(50, islandLayer);
	islandLayer = make_shared<AddIslandLayer>(70, islandLayer);
	islandLayer = make_shared<RemoveTooMuchOceanLayer>(2, islandLayer);
	islandLayer = make_shared<AddSnowLayer>(2, islandLayer);
	islandLayer = make_shared<AddIslandLayer>(3, islandLayer);
	islandLayer = make_shared<AddEdgeLayer>(2, islandLayer, AddEdgeLayer::Mode::COOL_WARM);
	islandLayer = make_shared<AddEdgeLayer>(2, islandLayer, AddEdgeLayer::Mode::HEAT_ICE);
	islandLayer = make_shared<AddEdgeLayer>(3, islandLayer, AddEdgeLayer::Mode::SPECIAL);
	islandLayer = make_shared<ZoomLayer>(2002, islandLayer);
	islandLayer = make_shared<ZoomLayer>(2003, islandLayer);
	islandLayer = make_shared<AddIslandLayer>(4, islandLayer);
	islandLayer = make_shared<AddMushroomIslandLayer>(5, islandLayer);
	islandLayer = make_shared<AddDeepOceanLayer>(4, islandLayer);
	islandLayer = ZoomLayer::zoom(1000, islandLayer, 0);
	//        }

	//        int zoomLevel = 0;
	int zoomLevel = 4;

	if (genType == GeneratorType::Legacy) {
		zoomLevel = 2;
	}

	LayerPtr riverLayer = islandLayer;
	riverLayer = ZoomLayer::zoom(1000, riverLayer, 0);
	riverLayer = make_shared<RiverInitLayer>(100, riverLayer);

	biomeLayer = islandLayer;
	biomeLayer = make_shared<BiomeInitLayer>(200, biomeLayer, genType);

	biomeLayer = ZoomLayer::zoom(1000, biomeLayer, 2);
	biomeLayer = make_shared<BiomeEdgeLayer>(1000, biomeLayer, genType == GeneratorType::Overworld);

	LayerPtr biomeRiverLayer = riverLayer;
	biomeRiverLayer = ZoomLayer::zoom(1000, biomeRiverLayer, 2);
	biomeLayer = make_shared<RegionHillsLayer>(1000, biomeLayer, biomeRiverLayer);

	riverLayer = ZoomLayer::zoom(1000, riverLayer, 2);
	riverLayer = ZoomLayer::zoom(1000, riverLayer, zoomLevel);
	riverLayer = make_shared<RiverLayer>(1, riverLayer);
	riverLayer = make_shared<SmoothLayer>(1000, riverLayer);

	biomeLayer = make_shared<RareBiomeSpotLayer>(1001, biomeLayer);

	for(auto i : range(zoomLevel)) {
		biomeLayer = make_shared<ZoomLayer>(1000 + i, biomeLayer);
		if(i == 0) {
			biomeLayer = make_shared<AddIslandLayer>(3, biomeLayer);
		}

		if(i == 1) {
			biomeLayer = make_shared<ShoreLayer>(1000, biomeLayer);
		}
	}

	biomeLayer = make_shared<SmoothLayer>(1000, biomeLayer);

	biomeLayer = make_shared<RiverMixerLayer>(100, biomeLayer, riverLayer);

	LayerPtr debugLayer = biomeLayer;
	zoomedLayer = make_shared<VoronoiZoom>(10, biomeLayer);

	biomeLayer->init(seed);
	zoomedLayer->init(seed);
}

Layer::~Layer() {

}

Layer::Layer(RandomSeed seedMixup) {
	mSeedMixup = seedMixup;
	mSeedMixup *= mSeedMixup * 6364136223846793005l + 1442695040888963407l;
	mSeedMixup += seedMixup;
	mSeedMixup *= mSeedMixup * 6364136223846793005l + 1442695040888963407l;
	mSeedMixup += seedMixup;
	mSeedMixup *= mSeedMixup * 6364136223846793005l + 1442695040888963407l;
	mSeedMixup += seedMixup;
}

void Layer::init(int64_t seed) {
	mSeed = seed;
	if(mParent != nullptr) {
		mParent->init(seed);
	}
	mSeed *= mSeed * 6364136223846793005l + 1442695040888963407l;
	mSeed += mSeedMixup;
	mSeed *= mSeed * 6364136223846793005l + 1442695040888963407l;
	mSeed += mSeedMixup;
	mSeed *= mSeed * 6364136223846793005l + 1442695040888963407l;
	mSeed += mSeedMixup;
}

void Layer::initRandom(int64_t x, int64_t y) {
	mRval = mSeed;
	mRval *= mRval * 6364136223846793005l + 1442695040888963407l;
	mRval += x;
	mRval *= mRval * 6364136223846793005l + 1442695040888963407l;
	mRval += y;
	mRval *= mRval * 6364136223846793005l + 1442695040888963407l;
	mRval += x;
	mRval *= mRval * 6364136223846793005l + 1442695040888963407l;
	mRval += y;
}

int Layer::nextRandom(int max) {
	int result = (int)((mRval >> 24) % max);
	if(result < 0) {
		result += max;
	}
	mRval *= mRval * 6364136223846793005l + 1442695040888963407l;
	mRval += mSeed;
	return result;
}

bool Layer::_isSame(const int a, const int b) {
	if(a == b) {
		return true;
	}
	if(a == Biome::mesaRock->mId || a == Biome::mesaClearRock->mId) {
		return b == Biome::mesaRock->mId || b == Biome::mesaClearRock->mId;
	}

	Biome* biome1 = Biome::getBiome(a);
	Biome* biome2 = Biome::getBiome(b);

	if(biome1 != nullptr && biome2 != nullptr) {
		return biome1->isSame(biome2);
	}

	return false;
}

bool Layer::_isOcean(int id) {
	return id == Biome::ocean->mId || id == Biome::deepOcean->mId || id == Biome::frozenOcean->mId;
}

int Layer::_random(int* list, int count) {
	return list[nextRandom(count)];
}

int Layer::_modeOrRandom(const int a, const int b, const int c, const int d) {
	if(b == c && c == d) {
		return b;
	}
	if(a == b && a == c) {
		return a;
	}
	if(a == b && a == d) {
		return a;
	}
	if(a == c && a == d) {
		return a;
	}
	if(a == b && c != d) {
		return a;
	}
	if(a == c && b != d) {
		return a;
	}
	if(a == d && b != c) {
		return a;
	}
	if(b == c && a != d) {
		return b;
	}
	if(b == d && a != c) {
		return b;
	}
	if(c == d && a != b) {
		return c;
	}

	int randomSeed[] = {
		a, b, c, d
	};
	return _random(randomSeed, 4);
}
