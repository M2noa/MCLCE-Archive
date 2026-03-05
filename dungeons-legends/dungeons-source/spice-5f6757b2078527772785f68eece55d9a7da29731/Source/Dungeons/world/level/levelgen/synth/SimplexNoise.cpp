/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/

#include "Dungeons.h"

#include "world/level/levelgen/synth/SimplexNoise.h"
#include "world/phys/Vec2.h"
#include "world/Pos.h"
#include "util/Math.h"
#include "util/Random.h"

const int SimplexNoise::GRAD3[12][3] {
	{
		1, 1, 0
	}, {
		-1, 1, 0
	}, {
		1, -1, 0
	}, {
		-1, -1, 0
	}, {
		1, 0, 1
	}, {
		-1, 0, 1
	}, {
		1, 0, -1
	}, {
		-1, 0, -1
	}, {
		0, 1, 1
	}, {
		0, -1, 1
	}, {
		0, 1, -1
	}, {
		0, -1, -1
	}
};
const float SimplexNoise::SQRT3 = Math::sqrt(3.0f);
const float SimplexNoise::F2 = 0.5f * (SQRT3 - 1.0f);
const float SimplexNoise::G2 = (3.0f - SQRT3) / 6.0f;

SimplexNoise::SimplexNoise(Random& random) {
	const float mOriginZ = random.nextFloat() * 256.0f;
	const float mOriginY = random.nextFloat() * 256.0f;
	const float mOriginX = random.nextFloat() * 256.0f;

	mOrigin = Vec3(mOriginX, mOriginY, mOriginZ);

	for (auto i : range(256)) {
		mNoiseMap[i] = i;
	}

	for (auto i : range(256)) {
		int j = random.nextInt(256 - i) + i;
		int tmp = mNoiseMap[i];
		mNoiseMap[i] = mNoiseMap[j];
		mNoiseMap[j] = tmp;

		mNoiseMap[i + 256] = mNoiseMap[i];
	}
}

float SimplexNoise::_getValue(const Vec2& vin) const {
	// Skew the input space to determine which simplex cell we're in
	float s = (vin.x + vin.y) * F2;	// Hairy factor for 2D
	int i = _fastFloor(vin.x + s);
	int j = _fastFloor(vin.y + s);
	float t = (i + j) * G2;
	Vec3 v3in(vin.x, vin.y, 0);
	Vec3 V0(i - t, j - t, 0);	// Unskew the cell origin back to (x,y) space
	Vec3 v0 = v3in - V0;	// The x,y distances from the cell origin
	// For the 2D case, the simplex shape is an equilateral triangle.
	// Determine which simplex we are in.
	Pos p1;	// Offsets for second (middle) corner of simplex in (i,j) coords
	if (v0.x > v0.y) {
		p1 = Pos(1, 0, 0);
	}	// lower triangle, XY order: (0,0)->(1,0)->(1,1)
	else {
		p1 = Pos(0, 1, 0);
	}	// upper triangle, YX order: (0,0)->(0,1)->(1,1)
		// A step of (1,0) in (i,j) means a step of (1-c,-c) in (x,y), and
		// a step of (0,1) in (i,j) means a step of (-c,1-c) in (x,y), where
		// c = (3-sqrt(3))/6

	Vec3 v1 = v0 - Vec3(p1) + G2;		// Offsets for middle corner in (x,y) unskewed coords
	Vec3 v2 = v0 - 1.0f + 2.0f * G2;	// Offsets for last corner in (x,y) unskewed coords
	// Work out the hashed gradient indices of the three simplex corners
	int ii = i & 255;
	int jj = j & 255;
	int gi0 = mNoiseMap[ii + mNoiseMap[jj]] % 12;
	int gi1 = mNoiseMap[ii + p1.x + mNoiseMap[jj + p1.y]] % 12;
	int gi2 = mNoiseMap[ii + 1 + mNoiseMap[jj + 1]] % 12;
	// Calculate the contribution from the three corners
	float t0 = 0.5f - v0.x * v0.x - v0.y * v0.y;
	float n0 = (t0 < 0) ? 0.0f : t0 * t0 * t0 * t0 * _dot(GRAD3[gi0], v0.x, v0.y);	// (x,y) of grad3 used for 2D gradient
	float t1 = 0.5f - v1.x * v1.x - v1.y * v1.y;
	float n1 = (t1 < 0) ? 0.0f : t1 * t1 * t1 * t1 * _dot(GRAD3[gi1], v1.x, v1.y);
	float t2 = 0.5f - v2.x * v2.x - v2.y * v2.y;
	float n2 = (t2 < 0) ? 0.0f : t2 * t2 * t2 * t2 * _dot(GRAD3[gi2], v2.x, v2.y);

	// Add contributions from each corner to get the const noise value.
	// The result is scaled to return values in the interval [-1,1].
	return 70.0f * (n0 + n1 + n2);
}

float SimplexNoise::_getValue(const Vec3& vin) const {
	const float F3 = 1.0f / 3.0f;
	float s = (vin.x + vin.y + vin.z) * F3;
	int i = _fastFloor(vin.x + s);
	int j = _fastFloor(vin.y + s);
	int k = _fastFloor(vin.z + s);
	const float G3 = 1.0f / 6.0f;
	float t = (i + j + k) * G3;
	Vec3 V0(i - t, j - t, k - t);
	Vec3 v0 = vin - V0;
	Pos p1;
	Pos p2;
	if (v0.x >= v0.y) {
		if (v0.y >= v0.z) {
			p1 = Pos(1, 0, 0);
			p2 = Pos(1, 1, 0);
		}	// X Y Z order
		else if (v0.x >= v0.z) {
			p1 = Pos(1, 0, 0);
			p2 = Pos(1, 0, 1);
		}	// X Z Y order
		else {
			p1 = Pos(0, 0, 1);
			p2 = Pos(1, 0, 1);
		}	// Z X Y order
	}
	else {	// x0<y0
		if (v0.y < v0.z) {
			p1 = Pos(0, 0, 1);
			p2 = Pos(0, 1, 1);
		}	// Z Y X order
		else if (v0.x < v0.z) {
			p1 = Pos(0, 1, 0);
			p2 = Pos(0, 1, 1);
		}	// Y Z X order
		else {
			p1 = Pos(0, 1, 0);
			p2 = Pos(1, 1, 0);
		}	// Y X Z order
	}
	// A step of (1,0,0) in (i,j,k) means a step of (1-c,-c,-c) in (x,y,z),
	// a step of (0,1,0) in (i,j,k) means a step of (-c,1-c,-c) in (x,y,z), and
	// a step of (0,0,1) in (i,j,k) means a step of (-c,-c,1-c) in (x,y,z), where
	// c = 1/6.

	Vec3 v1 = v0 - Vec3(p1) + G3;			// Offsets for second corner in (x,y,z) coords
	Vec3 v2 = v0 - Vec3(p2) + 2.0f * G3;	// Offsets for third corner in (x,y,z) coords
	Vec3 v3 = v0 - 1.0f + 3.0f * G3;		// Offsets for last corner in (x,y,z) coords
	// Work out the hashed gradient indices of the four simplex corners
	int ii = i & 255;
	int jj = j & 255;
	int kk = k & 255;
	int gi0 = mNoiseMap[ii + mNoiseMap[jj + mNoiseMap[kk]]] % 12;
	int gi1 = mNoiseMap[ii + p1.x + mNoiseMap[jj + p1.y + mNoiseMap[kk + p1.z]]] % 12;
	int gi2 = mNoiseMap[ii + p2.x + mNoiseMap[jj + p2.y + mNoiseMap[kk + p2.z]]] % 12;
	int gi3 = mNoiseMap[ii + 1 + mNoiseMap[jj + 1 + mNoiseMap[kk + 1]]] % 12;
	// Calculate the contribution from the four corners
	float t0 = 0.6f - v0.x * v0.x - v0.y * v0.y - v0.z * v0.z;
	float n0 = (t0 < 0) ? 0.0f : t0 * t0 * t0 * t0 * _dot(GRAD3[gi0], v0.x, v0.y, v0.z);
	float t1 = 0.6f - v1.x * v1.x - v1.y * v1.y - v1.z * v1.z;
	float n1 = (t1 < 0) ? 0.0f : t1 * t1 * t1 * t1 * _dot(GRAD3[gi1], v1.x, v1.y, v1.z);
	float t2 = 0.6f - v2.x * v2.x - v2.y * v2.y - v2.z * v2.z;
	float n2 = (t2 < 0) ? 0.0f : t2 * t2 * t2 * t2 * _dot(GRAD3[gi2], v2.x, v2.y, v2.z);
	float t3 = 0.6f - v3.x * v3.x - v3.y * v3.y - v3.z * v3.z;
	float n3 = (t3 < 0) ? 0.0f : t3 * t3 * t3 * t3 * _dot(GRAD3[gi3], v3.x, v3.y, v3.z);

	// Add contributions from each corner to get the const noise value.
	// The result is scaled to stay just inside [-1,1]
	return 32.0f * (n0 + n1 + n2 + n3);
}

void SimplexNoise::_add(float* buffer, float _x, float _y, int xSize, int ySize, float xs, float ys, float pow)  const {
	int pp = 0;

	for (auto yy : range(ySize)) {
		float yin = (_y + yy) * ys + mOrigin.y;

		for (auto xx : range(xSize)) {
			float xin = (_x + xx) * xs + mOrigin.x;
			buffer[pp++] += _getValue(Vec2(xin, yin)) * pow;
		}
	}
}

void SimplexNoise::_add(float* buffer, const Vec3& vin, int xSize, int ySize, int zSize, const Vec3& scale, float pow) const {
	int pp = 0;

	for (auto xx : range(xSize)) {
		float xin = (vin.x + xx) * scale.x + mOrigin.x;

		for (auto zz : range(zSize)) {
			float zin = (vin.z + zz) * scale.z + mOrigin.z;

			for (auto yy : range(ySize)) {
				float yin = (vin.y + yy) * scale.y + mOrigin.y;
				buffer[pp++] += _getValue(Vec3(xin, yin, zin)) * pow;
			}
		}
	}
}
