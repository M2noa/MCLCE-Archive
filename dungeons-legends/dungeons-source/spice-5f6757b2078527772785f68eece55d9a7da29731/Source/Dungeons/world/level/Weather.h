/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/

#pragma once

#include "world/level/levelgen/synth/PerlinSimplexNoise.h"
#include "world/level/LevelListener.h"

class BlockPos;
class Dimension;

class Weather :
	public LevelListener {

public:

	Weather(Dimension& d);
	~Weather();

	void tick();

	float getLightningLevel(float a) const;
	void setLightningLevel(float thunderLevel);
	float getRainLevel(float a) const;
	void setRainLevel(float rainLevel);
	float getFogLevel() const;
	void setFogLevel(float fogLevel);
	bool isLightning() const;
	bool isRaining() const;	// Confusing, but this is true for rain and snow
	bool isRainingAt(BlockSource& region, const BlockPos& pos) const;
	bool isSnowingAt(BlockSource& region, const BlockPos& pos) const;

	int getSkyFlashTime() const;
	void setSkyFlashTime(int flash);

	int calcRainDuration();
	int calcRainCycleTime();

	int calcLightningCycleTime();

	virtual void levelEvent(LevelEvent type, const Vec3& pos, int data) override;

	void stop();
	void serverTick();

	bool canPlaceTopSnow(BlockSource& region, const BlockPos& pos, bool fromFallingSnow, bool checkSnowDepthLevel, int* newHeightAfterPlacement);
	bool tryToPlaceTopSnow(BlockSource& region, const BlockPos& pos, bool fromFallingSnow, bool fillLowerNeighborsFirst);
	int calcSnowBlockDepth(BlockSource& region, const BlockPos& pos, int maxLayerDepth);
	void rebuildTopSnowToDepth(BlockSource& region, const BlockPos& testPos, int desiredDepth);

private:

	void _prepareWeather();

	PerlinSimplexNoise mNoise;

	int mTick = 0;

	float mORainLevel = 0, mRainLevel = 0, mTargetRainLevel = 0;
	float mOLightningLevel = 0, mLightningLevel = 0, mTargetLightningLevel = 0;
	float mFogLevel = 0;
	int mSkyFlashTime = 0;

	Dimension& mDimension;
};
