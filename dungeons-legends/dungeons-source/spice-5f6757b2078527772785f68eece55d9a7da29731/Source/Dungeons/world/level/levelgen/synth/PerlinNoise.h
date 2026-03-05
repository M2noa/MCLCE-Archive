/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/

#pragma once

#include "util/Random.h"

class ImprovedNoise;
class Vec2;
class Vec3;

class PerlinNoise {
public:

	PerlinNoise(int levels);
	PerlinNoise(RandomSeed seed, int levels, int minLevel = 0);
	PerlinNoise(Random& random, int levels, int minLevel = 0);
	~PerlinNoise();

	float getValue(const Vec3& pos) const;

	void getRegion(float* buffer, const Vec3& pos, int xSize, int ySize, int zSize, const Vec3& scale) const;
	void getRegion(float* buffer, const Vec2& pos, int xSize, int zSize, const Vec2& scale, float pow) const;

	int hashCode() const;

private:

	void _init(Random& random);

	const int mLevels;
	const int mMinLevel;

	std::vector<ImprovedNoise> mNoiseLevels;
};
