/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/

#pragma once

#include "world/phys/Vec3.h"

class Random;

class ImprovedNoise {
public:

	ImprovedNoise();
	ImprovedNoise(Random& random);

	void _init(Random& random);

	float _getValue(const Vec3& pos) const;
	void _readArea(float* buffer, const Vec3& pos, int xSize, int ySize, int zSize, const Vec3& s, float pow) const;
	int _hashCode() const;

private:

	static float _lerp(float t, float a, float b);
	static float _grad2(int hash, float x, float z);
	static float _grad(int hash, const Vec3& p);
	static float _calcValues(float val, int& index, float& lerp);

	float _noise(const Vec3& pos) const;
	void _blendCubeCorners(const Vec3& origin, int X, int Y, int Z, float u, float& vv0, float& vv1, float& vv2, float& vv3) const;

	Vec3 mOrigin;
	int mNoiseMap[512];
};
