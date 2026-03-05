/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/

#pragma once

#include "world/phys/Vec3.h"

class Random;
class Vec2;

class SimplexNoise {
public:

	SimplexNoise(Random& r);
	float _getValue(const Vec2& vin) const;
	float _getValue(const Vec3& vin) const;
	void _add(float* buffer, float _x, float _y, int xSize, int ySize, float xs, float ys, float pow) const;
	void _add(float* buffer, const Vec3& vin, int xSize, int ySize, int zSize, const Vec3& scale, float pow) const;

private:

	static int _fastFloor(float x) {
		return x > 0 ? (int)x : (int)x - 1;
	}

	static float _dot(const int* g, float x, float y) {
		return g[0] * x + g[1] * y;
	}

	static float _dot(const int* g, float x, float y, float z) {
		return g[0] * x + g[1] * y + g[2] * z;
	}

	static const int GRAD3[12][3];
	static const float SQRT3;
	static const float F2;
	static const float G2;

	Vec3 mOrigin;
	int mNoiseMap[512];
};
