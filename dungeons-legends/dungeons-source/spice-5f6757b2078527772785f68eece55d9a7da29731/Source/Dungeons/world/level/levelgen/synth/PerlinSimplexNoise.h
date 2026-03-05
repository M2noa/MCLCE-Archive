/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/

#pragma once

#include "util/Random.h"

class SimplexNoise;
class Vec3;

class PerlinSimplexNoise {
public:

	PerlinSimplexNoise(RandomSeed seed, int levels);
	PerlinSimplexNoise(Random& r, int levels);
	~PerlinSimplexNoise();

	float getValue(float x, float y) const;
	float getValue(const Vec3& pos) const;

	float* getRegion(float* buffer, const Vec3& pos, int xSize, int ySize, int zSize, const Vec3& scale) const;
	float* getRegion(float* buffer, const Vec2& pos, int xSize, int zSize, const Vec2& scale, float sizeScale, float powScale = 0.5f) const;

private:

	void _init(Random& r);

	const int mLevels;
	std::vector<SimplexNoise> mNoiseLevels;
};
