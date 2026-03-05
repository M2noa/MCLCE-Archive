#include "Dungeons.h"
#include "world/level/biome/MutatedBiome.h"
#include "world/level/biome/BiomeDecorator.h"

MutatedBiome::MutatedBiome(int id, Biome* containedBiome)
	: Biome(id, containedBiome->mBiomeType)
	, containedBiome(containedBiome) {
	setColor(containedBiome->mDebugMapColor, true);
	mName = containedBiome->mName + " M";

	mTopMaterial = containedBiome->mTopMaterial;
	mMaterial = containedBiome->mMaterial;
	mLeafColor = containedBiome->mLeafColor;
	mDepth = containedBiome->mDepth;
	mScale = containedBiome->mScale;
	mTemperature = containedBiome->mTemperature;
	mDownfall = containedBiome->mDownfall;
	mWaterColor = containedBiome->mWaterColor;
	mSnowAccumulation = containedBiome->mSnowAccumulation;
	mRain = containedBiome->mRain;

	_friendlies = containedBiome->_friendlies;
	_enemies = containedBiome->_enemies;
	//ambientFriendlies = Lists.newArrayList(containedBiome.ambientFriendlies);
	_waterFriendlies = containedBiome->_waterFriendlies;

	mTemperature = containedBiome->mTemperature;
	mDownfall = containedBiome->mDownfall;

	mDepth = containedBiome->mDepth + 0.1f;
	mScale = containedBiome->mScale + 0.2f;
}

void MutatedBiome::decorate(BlockSource* source, Random& random, const BlockPos& origin, bool legacy, float limit){
	containedBiome->decorator->decorate(source, random, this, origin, legacy, limit);
}

void MutatedBiome::buildSurfaceAt(Random& random, BlockVolume& blocks, const BlockPos& pos, float depthValue, Height seaLevel) {
	containedBiome->buildSurfaceAt(random, blocks, pos, depthValue, seaLevel);
}

float MutatedBiome::getCreatureProbability() {
	return containedBiome->getCreatureProbability();
}

const FeaturePtr& MutatedBiome::getTreeFeature(Random* random){
	return containedBiome->getTreeFeature(random);
}

int MutatedBiome::getFoliageColor() {
	return containedBiome->getFoliageColor();
}

int MutatedBiome::getMapFoliageColor() {
	return containedBiome->getMapFoliageColor();
}

int MutatedBiome::getGrassColor( const BlockPos& pos ) {
	return containedBiome->getGrassColor(pos);
}

int MutatedBiome::getMapGrassColor(const BlockPos& pos) {
	return getGrassColor(pos);
}

bool MutatedBiome::isSame(Biome* other) {
	return containedBiome->isSame(other);
}

Biome::BiomeTempCategory MutatedBiome::getTemperatureCategory() const {
	return containedBiome->getTemperatureCategory();
}
