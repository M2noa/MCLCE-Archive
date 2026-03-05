/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/

#include "Dungeons.h"

#include "world/level/levelgen/synth/PerlinNoise.h"
#include "world/level/levelgen/synth/ImprovedNoise.h"
#include "world/phys/Vec2.h"
#include "world/phys/Vec3.h"
//#include "util/PerfTimer.h"

void PerlinNoise::_init(Random& random) {
	mNoiseLevels.resize(mLevels);
	for (auto&& noise : mNoiseLevels) {
		noise._init(random);
	}
}

PerlinNoise::~PerlinNoise() {

}

PerlinNoise::PerlinNoise(int levels) :
	mLevels(levels)
	, mMinLevel(0) {

	Random random;
	_init(random);
}

PerlinNoise::PerlinNoise(RandomSeed seed, int levels, int minLevel) :
	mLevels(levels)
	, mMinLevel(minLevel) {
	
	Random random(seed);
	_init(random);
}

PerlinNoise::PerlinNoise(Random& random, int levels, int minLevel) :
	mLevels(levels)
	, mMinLevel(minLevel) {

	_init(random);
}

float PerlinNoise::getValue(const Vec3& pos) const {
	float value = 0;
	float pow = 1;

	int i = 0;

	for (; i < mMinLevel; ++i, pow /= 2) {
	}

	for (; i < mLevels; i++) {
		value += mNoiseLevels[i]._getValue(pos * pow) / pow;
		pow /= 2;
	}

	return value;
}

void PerlinNoise::getRegion(float* buffer, const Vec3& pos, int xSize, int ySize, int zSize, const Vec3& scale) const {
// 	ScopedProfile("PerlinNoiseGetRegion");
	DEBUG_ASSERT(buffer != nullptr, "We don't create buffers as a part of perlin noise");
	const int size = xSize * ySize * zSize;
	memset(buffer, 0, size * sizeof(float));

	float pow = 1;
	int i = 0;

	for (; i < mMinLevel; ++i) {
		pow /= 2;
	}

	for (; i < mLevels; i++) {
// 		ScopedProfile("addNoiseLevel");
		mNoiseLevels[i]._readArea(buffer, pos, xSize, ySize, zSize, scale * pow, pow);
		pow /= 2;
	}
}

void PerlinNoise::getRegion(float* buffer, const Vec2& pos, int xSize, int zSize, const Vec2& scale, float pow ) const {
	return getRegion(buffer, Vec3(pos.x, 10.0f, pos.y), xSize, 1, zSize, Vec3(scale.x, 1, scale.y));
}

int PerlinNoise::hashCode() const {
	int x = 4711;

	for (int i = 0; i < mLevels; ++i) {
		x *= mNoiseLevels[i]._hashCode();
	}

	return x;
}
