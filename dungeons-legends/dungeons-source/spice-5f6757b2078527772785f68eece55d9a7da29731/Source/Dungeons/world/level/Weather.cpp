/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/

#include "Dungeons.h"

// Primary header
#include "Weather.h"

// Secondary headers
#include "world/Direction.h"
#include "world/level/biome/Biome.h"
#include "world/level/block/TopSnowBlock.h"
#include "world/level/BlockSource.h"
#include "world/level/dimension/Dimension.h"
#include "world/level/Level.h"
#include "world/level/LevelConstants.h"
#include "world/level/storage/LevelData.h"
#include "world/level/material/Material.h"

Weather::Weather(Dimension& d) :
	mDimension(d)
	, mNoise(45127L, 1)
	, mTick(0) {
// 	mDimension.getLevel().addListener(*this);

	//set the rain level to whatever the leveldata states
	setRainLevel(mDimension.getLevel().getLevelData().getRainLevel());
}

Weather::~Weather() {
// 	mDimension.getLevel().removeListener(*this);
}

void Weather::_prepareWeather() {
}

float WEATHER_RAIN_PERIOD = 0.01f;

void Weather::levelEvent(LevelEvent type, const Vec3& pos, int data) {
	if (type == LevelEvent::StopRaining) {
		setRainLevel(0.0f);
	}
	else if (type == LevelEvent::StartRaining) {
		setRainLevel(((float)data) / 65535.f);
	}
	else if (type == LevelEvent::StartThunderstorm) {
		setLightningLevel(((float)data) / 65535.f);
	}
	else if (type == LevelEvent::StopThunderstorm) {
		setLightningLevel(0.0f);
	}
}

void Weather::tick() {
	if (!mDimension.hasWeather()) {
		return;
	}

	++mTick;

	mORainLevel = mRainLevel;
	mOLightningLevel = mLightningLevel;
	
	if (mTargetLightningLevel > mLightningLevel) {
		mLightningLevel = std::min(mTargetLightningLevel, mLightningLevel + 0.01f);
	}
	else {
		mLightningLevel = std::max(mTargetLightningLevel, mLightningLevel - 0.01f);
	}
	mLightningLevel = Math::clamp(mLightningLevel, 0.0f, 1.0f);

	if (mTargetRainLevel > mRainLevel) {
		mRainLevel = std::min(mTargetRainLevel, mRainLevel + 0.01f);
	}
	else {
		mRainLevel = std::max(mTargetRainLevel, mRainLevel - 0.01f);
	}
	mRainLevel = Math::clamp(mRainLevel, 0.0f, 1.0f);

	if (!mDimension.getLevel().isClientSide()) {
		serverTick();

		//HACK this prevents different weathers in different dimensions
		Level& level = mDimension.getLevel();
		LevelData& levelData = level.getLevelData();

		levelData.setRainLevel(mRainLevel);
		levelData.setLightningLevel(mLightningLevel);
	}

	if (mSkyFlashTime > 0) {
		--mSkyFlashTime;
	}

}

void Weather::serverTick() {
	Level& level = mDimension.getLevel();
	LevelData& levelData = level.getLevelData();

	// Lightning
	int lightningTime = level.getLevelData().getLightningTime() - 1;
	if (lightningTime <= 0) {
		// Don't set it to start doing lightning again automatically.
		levelData.setLightningLevel(0.0f);
		levelData.setLightningTime(calcLightningCycleTime());
// 		level.broadcastLevelEvent(LevelEvent::StopThunderstorm, Vec3::ZERO, 0);

		setLightningLevel(0.0f);
	}
	else {
		levelData.setLightningTime(lightningTime);
	}
	// Rain
	int rainTime = levelData.getRainTime() - 1;
	if (rainTime <= 0) {
		if (levelData.isRaining()) {
			levelData.setRainLevel(0.0f);
			levelData.setRainTime(calcRainCycleTime());
// 			level.broadcastLevelEvent(LevelEvent::StopRaining, Vec3::ZERO, 0);

			setRainLevel(0.0f);
		}
		else {
			// 10% chance of getting a lightning storm when it starts to rain
			const int rainDuration = calcRainDuration();
			if (level.getRandom().nextInt(10) == 0) {
				float newLightningLevel = level.getRandom().nextFloat(0.3f, 0.7f);
				levelData.setRainLevel(1.0f);	// Max rain level
				levelData.setRainTime(rainDuration);
				levelData.setLightningLevel(newLightningLevel);
				levelData.setLightningTime(rainDuration);
// 				level.broadcastLevelEvent(LevelEvent::StartRaining, Vec3::ZERO, Math::floor(1.0f * 65535.f));
// 				level.broadcastLevelEvent(LevelEvent::StartThunderstorm, Vec3::ZERO, Math::floor(newLightningLevel * 65535.f));

				setRainLevel(1.0f);
				setLightningLevel(newLightningLevel);
			}
			else {
				float newRainLevel = level.getRandom().nextFloat(0.3f, 0.8f);
				levelData.setRainLevel(newRainLevel);
				levelData.setRainTime(rainDuration);
// 				level.broadcastLevelEvent(LevelEvent::StartRaining, Vec3::ZERO, Math::floor(newRainLevel * 65535.f));

				setRainLevel(newRainLevel);
			}
		}
	}
	else {
		levelData.setRainTime(rainTime);
	}	
}

int Weather::calcRainDuration() {
	Level& level = mDimension.getLevel();
	return level.getRandom().nextInt(Level::TICKS_PER_DAY / 2) + Level::TICKS_PER_DAY / 2;
}

int Weather::calcRainCycleTime() {
	Level& level = mDimension.getLevel();
	return level.getRandom().nextInt(Level::TICKS_PER_DAY * 7) + Level::TICKS_PER_DAY / 2;
}

int Weather::calcLightningCycleTime() {
	Level& level = mDimension.getLevel();
	return level.getRandom().nextInt(300, 900) * SharedConstants::TicksPerSecond;
}

float Weather::getLightningLevel(float a) const {
	return mOLightningLevel + (mLightningLevel - mOLightningLevel) * a;
}

void Weather::setLightningLevel(float lightningLevel) {
	mTargetLightningLevel = lightningLevel;
}

float Weather::getRainLevel(float a) const {
	return mORainLevel + (mRainLevel - mORainLevel) * a;
}

void Weather::setRainLevel(float rainLevel) {
	mTargetRainLevel = rainLevel;
}

float Weather::getFogLevel() const {
	return mFogLevel;
}

void Weather::setFogLevel(float fogLevel) {
	mFogLevel = fogLevel;
}

bool Weather::isLightning() const {
	return getLightningLevel(1) > 0.0f;
}

bool Weather::isRaining() const {
	return getRainLevel(1) > 0.0f;
}

bool Weather::isRainingAt(BlockSource& region, const BlockPos& pos) const {
	if (!isRaining()) {
		return false;
	}
	if (!region.canSeeSky(pos)) {
		return false;
	}
	if (region.getTopRainBlockPos(pos).y > pos.y) {
		return false;
	}

	const Biome& b = region.getBiome(pos);

	if (region.isSnowTemperature(pos)) {
		return false;
	}
	return b.mRain;
}

bool Weather::isSnowingAt(BlockSource& region, const BlockPos& pos) const {
	if (!isRaining()) {
		return false;
	}
	if (!region.canSeeSky(pos)) {
		return false;
	}
	if (region.getTopRainBlockPos(pos).y > pos.y) {
		return false;
	}

	const Biome& b = region.getBiome(pos);
	if (!region.isSnowTemperature(pos)) {
		return false;
	}
	return b.mRain;
}

int Weather::getSkyFlashTime() const {
	return mSkyFlashTime;
}

void Weather::setSkyFlashTime(int flash) {
	mSkyFlashTime = flash;
}

void Weather::stop() {

	if (mRainLevel == 0.0f && mLightningLevel == 0.0f) {	//Already stopped, fixes recursion issues with events
		return;
	}

	mRainLevel = 0.0f;
	mORainLevel = 0.0f;
	mTargetRainLevel = 0.0f;
	mLightningLevel = 0.0f;
	mOLightningLevel = 0.0f;
	mTargetLightningLevel = 0.0f;
	mSkyFlashTime = 0;

	BlockSource& region = mDimension.getBlockSourceDEPRECATEDUSEPLAYERREGIONINSTEAD();
	Level& level = region.getLevel();

	if (level.isClientSide()) {
		return;
	}

	LevelData& levelData = level.getLevelData();

	// Lightning
	levelData.setLightningLevel(0.0f);
	levelData.setLightningTime(calcLightningCycleTime());

	levelData.setRainLevel(0.0f);
	levelData.setRainTime(calcRainCycleTime());

// 	level.broadcastLevelEvent(LevelEvent::StopRaining, Vec3::ZERO, 0);
// 	level.broadcastLevelEvent(LevelEvent::StopThunderstorm, Vec3::ZERO, 0);
}

bool Weather::canPlaceTopSnow(BlockSource& region, const BlockPos& pos, bool fromFallingSnow, bool checkSnowDepthLevel, int* newHeightAfterPlacement) {

	// Too hot for falling snow?
	if (!region.isSnowTemperature(pos)) {
		return false;
	}

	// Too bright for falling snow?
// 	float brightness = region.getBrightness(LightLayer::BLOCK, pos);
	float brightness = 0;
	if (brightness > 11) {
		return false;
	}

	// Too low or high for falling snow?
	if (fromFallingSnow && (pos.y < 0 || pos.y >= region.getMaxHeight())) {
		return false;
	}

	// [Check Existing BlockID]
	const Block& onTopOfBlock = region.getBlock(pos);
	if (onTopOfBlock.isType(Block::mTopSnow)) {

		// Full of top snow?
		int height = TopSnowBlock::dataIDToHeight(region.getData(pos));

		// If newHeightAfterPlacement != null then we want to add to our height.
		// If we want to add to our height (true when newHeightAfterPlacement != null) then
		// Check to see if our height is already at MAX.
		if (newHeightAfterPlacement != nullptr && height == TopSnowBlock::MAX_HEIGHT) {
			return false;
		}

		// Too much snow? (depends on biome's max snow level)
		if (checkSnowDepthLevel) {
			Biome& biome = region.getBiome(pos);
			int maxLayers = biome.getMaxSnowLayers();
			if (maxLayers > 0) {
				int snowDepthLevel = calcSnowBlockDepth(region, pos, maxLayers);
				if (snowDepthLevel >= maxLayers) {
					return false;	// we've reached the max level of snow from falling snow
				}
			}
		}

		// Output new height
		if (newHeightAfterPlacement) {
			*newHeightAfterPlacement = height + 1;
		}
		return true;
	}

	// can only replace AIR or 'recoverable blocks', otherwise return false.
	if (!onTopOfBlock.isType(Block::mAir) && !TopSnowBlock::checkIsRecoverableBlock(onTopOfBlock.mID)) {
		return false;
	}

	// [Check Below BlockID]
	// You *can* place topSnow on top of a 'full block' of topSnow
	BlockPos belowPos(pos.x, pos.y - 1, pos.z);
	auto& belowBlock = region.getBlock(belowPos);
	auto& material = belowBlock.getMaterial();
	if (belowBlock.hasProperty(BlockProperty::TopSnow)) {
		int heightData = TopSnowBlock::dataIDToHeight(region.getData(belowPos));
		if (heightData == TopSnowBlock::MAX_HEIGHT) {

			// Too much snow? (depends on biome's max snow level)
			if (checkSnowDepthLevel) {
				Biome& biome = region.getBiome(pos);
				int maxLayers = biome.getMaxSnowLayers();
				if (maxLayers > 0) {
					int snowDepthLevel = calcSnowBlockDepth(region, pos, maxLayers);
					if (snowDepthLevel >= maxLayers) {
						return false;	// we've reached the max level of snow from falling snow
					}
				}
			}

			if (newHeightAfterPlacement) {
				*newHeightAfterPlacement = 1;
			}
			return true;
		}
		return false;
	}
	else if (fromFallingSnow && (belowBlock.mID == Block::mIce->mID || belowBlock.mID == Block::mPackedIce->mID)) {
		// falling snow cannot be placed on ice or packedIce
		return false;
	}

	if (material.isType(MaterialType::Air) || !belowBlock.isSolid()) {
		const Material& belowMaterial = belowBlock.getMaterial();
		if (!belowMaterial.isType(MaterialType::Leaves) && !belowMaterial.isType(MaterialType::Ice) && !belowMaterial.isType(MaterialType::Dirt)) {
			return false;
		}
	}

	if (!region.getMaterial(pos.x, pos.y - 1, pos.z).getBlocksMotion()) {
		return false;
	}

	if (newHeightAfterPlacement) {
		*newHeightAfterPlacement = 1;
	}

	return true;
}

bool Weather::tryToPlaceTopSnow(BlockSource& region, const BlockPos& pos, bool fromFallingSnow, bool fillLowerNeighborsFirst) {

	// Should it snow at this pos?
	int newHeight = 0;
	bool addSnowToPos = canPlaceTopSnow(region, pos, fromFallingSnow, true, &newHeight);

	// If we already have snow and and wish to fill neighbors first
	if (fillLowerNeighborsFirst) {

		// Look at our NSEW neighbors and find the lowest height vs ours.
		int neighborNewHeight, lowestNewHeight = newHeight, halfNewHeight = newHeight / 2;
		BlockPos testPos, neighborPos, lowestPos = pos;

		for (int facing = Direction::NORTH; facing <= Direction::EAST; ++facing) {
			neighborPos = pos.neighbor(facing);
			if (canPlaceTopSnow(region, neighborPos, fromFallingSnow, true, &neighborNewHeight) && neighborNewHeight < lowestNewHeight) {

				// Limit the amount of top snow added when neighbor is not top rain block pos to half the input pos new
				// height
				// In this way falling snow adds to locations in a way that slopes down in covered areas.
				if (region.checkIsTopRainBlockPos(neighborPos) || neighborNewHeight < halfNewHeight) {
					lowestNewHeight = neighborNewHeight;
					lowestPos = neighborPos;
				}
			}
		}

		// If lowestPos != pos then we found a valid neighbor to place snow
		if (lowestPos != pos) {

			DEBUG_ASSERT(lowestNewHeight <= TopSnowBlock::MAX_HEIGHT, "Lowest New Height is too large");
			DataID data = TopSnowBlock::buildData(region, lowestPos, 1, true);
			region.setBlockAndData(lowestPos, {Block::mTopSnow->mID, data}, Block::UPDATE_CLIENTS);
			return true;
		}
	}

	if (addSnowToPos) {

		DEBUG_ASSERT(newHeight <= TopSnowBlock::MAX_HEIGHT, "New Height is too large");
		DataID data = TopSnowBlock::buildData(region, pos, 1, true);
		region.setBlockAndData(pos, {Block::mTopSnow->mID, data}, Block::UPDATE_CLIENTS);
		return true;
	}

	// We did not add any snow
	return false;
}

int Weather::calcSnowBlockDepth(BlockSource& region, const BlockPos& pos, int maxLayerDepth) {

	DEBUG_ASSERT(maxLayerDepth > 0, "need a positive maxDepth");
	int depth = 0;
	BlockID testBlockId = BlockID::AIR;
	BlockPos testPos = pos;
	do {
		testBlockId = region.getBlockID(testPos);
		if (testBlockId == Block::mTopSnow->mID) {
			depth += TopSnowBlock::dataIDToHeight(region.getData(testPos));
		}
		else if (testBlockId == Block::mSnow->mID) {
			depth += 8;
		}
		else {
			break;
		}

		--testPos.y;
	} while (--maxLayerDepth >= 0);

	return depth;
}

void Weather::rebuildTopSnowToDepth(BlockSource& region, const BlockPos& testPos, int desiredDepth) {

	DEBUG_ASSERT(desiredDepth >= 0, "need a positive set layer depth amount");

	// 1. find topPos (look for air)
	int topY = testPos.y;
	BlockID testBlockId = region.getBlockID(testPos.x, topY, testPos.z);
	while (testBlockId != BlockID::AIR) {
		++topY;
		testBlockId = region.getBlockID(testPos.x, topY, testPos.z);
	}
	BlockPos topPos(testPos.x, topY, testPos.z);

	int realSnowCount = 0;
	// 2. find the ground by counting blocks downward
	BlockPos bottomPos(topPos.x, topPos.y - 1, topPos.z);
	testBlockId = region.getBlockID(bottomPos);
	BlockProperty anySnowType = BlockProperty::TopSnow | BlockProperty::Snow;
	while (testBlockId.hasProperty(anySnowType)) {
		if (testBlockId == Block::mSnow->mID) {
			++realSnowCount;
		}
		--bottomPos.y;
		testBlockId = region.getBlockID(bottomPos);
	}

	// go above ground to snow or air
	if (!testBlockId.hasProperty(anySnowType)) {

		// Unless it's a top snow 'recoverable' block (such as grass)
		if (!TopSnowBlock::checkIsRecoverableBlock(testBlockId)) {
			++bottomPos.y;
		}
	}

	while (realSnowCount > 0) {
		region.setBlockAndData(bottomPos, Block::mSnow->mID, Block::UPDATE_CLIENTS);
		++bottomPos.y;
		--realSnowCount;
	}

	// 3. Setup Top Snow Blocks based on desired depth
	while (desiredDepth > 0) {

		int setDepth = std::min(desiredDepth, 8);

		DataID data = TopSnowBlock::buildData(region, bottomPos, setDepth, false);
		region.setBlockAndData(bottomPos, {Block::mTopSnow->mID, data}, Block::UPDATE_CLIENTS);

		desiredDepth -= setDepth;
		++bottomPos.y;
	}

	// 4. Set remaining blocks as AIR
	while (bottomPos.y < topPos.y) {

		region.setBlockAndData(bottomPos, FullBlock::AIR, Block::UPDATE_CLIENTS);
		++bottomPos.y;
	}
}
