/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/
#include "Dungeons.h"

#include "world/level/newbiome/BiomeInitLayer.h"

#include "world/level/biome/Biome.h"

BiomeInitLayer::BiomeInitLayer(RandomSeed seedMixup, LayerPtr& parent, GeneratorType generator) :
	Layer(seedMixup) {
	mParent = parent;

	mWarmBiomes[0] = Biome::desert.get();
	mWarmBiomes[1] = Biome::desert.get();
	mWarmBiomes[2] = Biome::desert.get();
	mWarmBiomes[3] = Biome::savanna.get();
	mWarmBiomes[4] = Biome::savanna.get();
	mWarmBiomes[5] = Biome::plains.get();

	mMediumBiomes.push_back(Biome::forest.get());
	if(generator == GeneratorType::Overworld) {
		mMediumBiomes.push_back(Biome::roofedForest.get());
	}
	mMediumBiomes.push_back(Biome::extremeHills.get());

	mMediumBiomes.push_back(Biome::plains.get());
	mMediumBiomes.push_back(Biome::plains.get());//added more plains
	mMediumBiomes.push_back(Biome::plains.get());

	mMediumBiomes.push_back(Biome::birchForest.get());
	mMediumBiomes.push_back(Biome::swampland.get());

	mColdBiomes[0] = Biome::forest.get();
	mColdBiomes[1] = Biome::extremeHills.get();
	mColdBiomes[2] = Biome::taiga.get();
	mColdBiomes[3] = Biome::plains.get();

	mIceBiomes[0] = Biome::iceFlats.get();
	mIceBiomes[1] = Biome::iceFlats.get();
	mIceBiomes[2] = Biome::iceFlats.get();
	mIceBiomes[3] = Biome::taigaCold.get();

}

void BiomeInitLayer::fillArea(LayerData& layerData, int xo, int yo, int w, int h) {
	mParent->fillArea(layerData, xo, yo, w, h);

	for(auto y : range(h)) {
		for(auto x : range(w)) {
			initRandom(x + xo, y + yo);
			int old = layerData.mParentArea[x + y * w];
			int special = (old & SPECIAL_MASK) >> SPECIAL_SHIFT;
			old &= ~SPECIAL_MASK;
			if(_isOcean(old)) {
				layerData.mResult[x + y * w] = old;
			}
			else if(old == Biome::mushroomIsland->mId) {
				layerData.mResult[x + y * w] = old;
			}
			else if(old == Layer::WARM_ID) {
				if(special > 0) {
					if(nextRandom(3) == 0) {
						layerData.mResult[x + y * w] = Biome::mesaClearRock->mId;
					}
					else {
						layerData.mResult[x + y * w] = Biome::mesaRock->mId;
					}
				}
				else {
					layerData.mResult[x + y * w] = mWarmBiomes[nextRandom(NUM_WARM_BIOMES)]->mId;
				}
			}
			else if(old == Layer::MEDIUM_ID) {
				if(special > 0) {
					layerData.mResult[x + y * w] = Biome::jungle->mId;
				}
				else {
					layerData.mResult[x + y * w] = mMediumBiomes[nextRandom(mMediumBiomes.size())]->mId;
				}
			}
			else if(old == Layer:: COLD_ID) {
				if(special > 0) {
					layerData.mResult[x + y * w] = Biome::redwoodTaiga->mId;
				}
				else {
					layerData.mResult[x + y * w] = mColdBiomes[nextRandom(NUM_COLD_BIOMES)]->mId;
				}
			}
			else if(old == Layer::ICE_ID) {
				layerData.mResult[x + y * w] = mIceBiomes[nextRandom(NUM_ICE_BIOMES)]->mId;
			}
			else {
				layerData.mResult[x + y * w] = Biome::mushroomIsland->mId;
			}
		}
	}
	layerData.swap();
}
