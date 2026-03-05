/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/

#include "Dungeons.h"

#include "world/level/levelgen/synth/PerlinSimplexNoise.h"
#include "world/level/levelgen/synth/ImprovedNoise.h"
#include "world/phys/Vec3.h"
#include "world/phys/Vec2.h"
#include "world/level/levelgen/synth/SimplexNoise.h"

PerlinSimplexNoise::PerlinSimplexNoise(RandomSeed seed, int levels) :
	mLevels(levels) {

	Random r(seed);
	_init(r);
}

PerlinSimplexNoise::PerlinSimplexNoise(Random& r, int levels) :
	mLevels(levels) {

	_init(r);
}

PerlinSimplexNoise::~PerlinSimplexNoise() {
	//leave here so that the compiler generates the code for the destruction of SimplexNoise here
}

void PerlinSimplexNoise::_init(Random& r) {
	mNoiseLevels.reserve(mLevels);
	for (int a = 0; a < mLevels; ++a) {
		mNoiseLevels.emplace_back(r);
	}
}

float PerlinSimplexNoise::getValue(float x, float y) const {
	float value = 0;
	float pow = 1;

	for (int i = 0; i < mLevels; i++) {
		Vec2 pos(x, y);
		value += mNoiseLevels[i]._getValue(pos * pow) / pow;
		pow /= 2;
	}

	return value;
}

float PerlinSimplexNoise::getValue(const Vec3& pos) const {
	float value = 0;
	float pow = 1;

	for (int i = 0; i < mLevels; i++) {
		value += mNoiseLevels[i]._getValue(pos * pow) / pow;
		pow /= 2;
	}

	return value;
}

float* PerlinSimplexNoise::getRegion(float* buffer, const Vec2& pos, int xSize, int ySize, const Vec2& scale, float sizeScale, float powScale) const {
	DEBUG_ASSERT(buffer != nullptr, "We don't create buffers as a part of perlin noise");
	const int size = xSize * ySize;
	memset(buffer, 0, size * sizeof(float));

	float pow = 1;
	float mult = 1;
	for (auto i : range(mLevels)) {
		mNoiseLevels[i]._add(buffer, pos.x, pos.y, xSize, ySize, scale.x * mult * pow, scale.y * mult * pow, 0.55f / pow);
		mult *= sizeScale;
		pow *= powScale;
	}

	return buffer;
}

float* PerlinSimplexNoise::getRegion(float* buffer, const Vec3& pos, int xSize, int ySize, int zSize, const Vec3& scale) const {
	DEBUG_ASSERT(buffer != nullptr, "We don't create buffers as a part of perlin noise");
	const int size = xSize * ySize * zSize;
	memset(buffer, 0, size * sizeof(float));

	float pow = 1;

	for (auto i : range(mLevels)) {
		mNoiseLevels[i]._add(buffer, pos, xSize, ySize, zSize, scale * pow, 0.55f / pow);
		pow /= 2;
	}

	return buffer;
}
