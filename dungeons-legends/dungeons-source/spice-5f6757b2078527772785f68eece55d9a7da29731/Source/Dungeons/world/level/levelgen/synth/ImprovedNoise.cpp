/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/

#include "Dungeons.h"

#include "world/level/levelgen/synth/ImprovedNoise.h"
#include "util/Random.h"

ImprovedNoise::ImprovedNoise() {
	Random random(1);
	_init(random);
}

ImprovedNoise::ImprovedNoise(Random& random) {
	_init(random);
}

void ImprovedNoise::_init(Random& random) {
	mOrigin.x = random.nextFloat() * 256.f;
	mOrigin.y = random.nextFloat() * 256.f;
	mOrigin.z = random.nextFloat() * 256.f;

	for (int i = 0; i < 256; i++) {
		mNoiseMap[i] = i;
	}

	for (int i = 0; i < 256; i++) {
		int j = random.nextInt(256 - i) + i;
		int tmp = mNoiseMap[i];
		mNoiseMap[i] = mNoiseMap[j];
		mNoiseMap[j] = tmp;

		mNoiseMap[i + 256] = mNoiseMap[i];
	}
}

float ImprovedNoise::_noise(const Vec3& pos) const {
	int X, Y, Z;
	float u, v, w;

	Vec3 p = pos + mOrigin;
	p.x = _calcValues(p.x, X, u);
	p.y = _calcValues(p.y, Y, v);
	p.z = _calcValues(p.z, Z, w);

	float vv0 = 0, vv1 = 0, vv2 = 0, vv3 = 0;
	_blendCubeCorners(p, X, Y, Z, u, vv0, vv1, vv2, vv3);

	// ADD BLENDED RESULTS FROM 8 CORNERS OF CUBE
	return _lerp(w, _lerp(v, vv0, vv1), _lerp(v, vv2, vv3));
}

float ImprovedNoise::_lerp(float t, float a, float b) {
	return a + t * (b - a);
}

float ImprovedNoise::_grad2(int hash, float x, float z) {
	int h = hash & 15;	// CONVERT LO 4 BITS OF HASH CODE...

	// ...INTO 12 GRADIENT DIRECTIONS.
	float u = (1 - ((h & 8) >> 3)) * x;
	float v = h < 4 ? 0 : h == 12 || h == 14 ? x : z;

	return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
}

float ImprovedNoise::_grad(int hash, const Vec3& p) {
	int h = hash & 15;	// CONVERT LO 4 BITS OF HASH CODE...

	// ...INTO 12 GRADIENT DIRECTIONS.
	float u = h < 8 ? p.x : p.y;
	float v = h < 4 ? p.y : h == 12 || h == 14 ? p.x : p.z;

	return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
}

float ImprovedNoise::_calcValues(float val, int& index, float& lerp) {
	int vf = (int)val;
	if (val < vf) {
		vf--;
	}
	index = vf & 255;	// FIND UNIT THAT CONTAINS POINT.
	val -= vf;	// FIND RELATIVE COORD OF POINT IN CUBE.
	lerp = val * val * val * (val * (val * 6 - 15) + 10);	// COMPUTE FADE CURVES FOR COORD.

	return val;
}

float ImprovedNoise::_getValue(const Vec3& pos) const {
	return _noise(pos);
}

void ImprovedNoise::_readArea(float* buffer, const Vec3& pos, int xSize, int ySize, int zSize, const Vec3& s, float pow) const {
	if (ySize == 1) {
		int A = 0, AA = 0, B = 0, BA = 0;
		float vv0 = 0, vv2 = 0;
		int pp = 0;
		float scale = 1.0f / pow;

		for (int xx = 0; xx < xSize; xx++) {
			int X;
			float u;
			float x = (pos.x + xx) * s.x + mOrigin.x;
			x = _calcValues(x, X, u);

			for (int zz = 0; zz < zSize; zz++) {
				int Z;
				float w;
				float z = (pos.z + zz) * s.z + mOrigin.z;
				z = _calcValues(z, Z, w);

				A = mNoiseMap[X] + 0;
				AA = mNoiseMap[A] + Z;
				B = mNoiseMap[X + 1] + 0;
				BA = mNoiseMap[B] + Z;
				vv0 = _lerp(u, _grad2(mNoiseMap[AA], x, z), _grad(mNoiseMap[BA], Vec3(x - 1, 0, z)));
				vv2 = _lerp(u, _grad(mNoiseMap[AA + 1], Vec3(x, 0, z - 1)), _grad(mNoiseMap[BA + 1], Vec3(x - 1, 0, z - 1)));

				float val = _lerp(w, vv0, vv2);
				buffer[pp++] += val * scale;
			}
		}
		return;
	}

	int pp = 0;
	float scale = 1 / pow;
	int yOld = -1;
	float vv0 = 0, vv1 = 0, vv2 = 0, vv3 = 0;

	for (int xx = 0; xx < xSize; xx++) {
		int X;
		float u;
		float x = (pos.x + xx) * s.x + mOrigin.x;
		x = _calcValues(x, X, u);

		for (int zz = 0; zz < zSize; zz++) {
			int Z;
			float w;
			float z = (pos.z + zz) * s.z + mOrigin.z;
			z = _calcValues(z, Z, w);

			for (int yy = 0; yy < ySize; yy++) {
				int Y;
				float v;
				float y = (pos.y + yy) * s.y + mOrigin.y;
				y = _calcValues(y, Y, v);

				if (yy == 0 || Y != yOld) {
					yOld = Y;
					_blendCubeCorners(Vec3(x, y, z), X, Y, Z, u, vv0, vv1, vv2, vv3);
				}

				float v0 = _lerp(v, vv0, vv1);
				float v1 = _lerp(v, vv2, vv3);
				float val = _lerp(w, v0, v1);

				buffer[pp++] += val * scale;
			}
		}
	}
}

int ImprovedNoise::_hashCode() const {
	int x = 4711;

	for (int i = 0; i < 512; ++i) {
		x = x * 37 + mNoiseMap[i];
	}

	return x;
}

void ImprovedNoise::_blendCubeCorners(const Vec3& origin, int X, int Y, int Z, float u, float& vv0, float& vv1, float& vv2, float& vv3) const {
	int A = mNoiseMap[X] + Y;
	int AA = mNoiseMap[A] + Z;
	int AB = mNoiseMap[A + 1] + Z;
	int B = mNoiseMap[X + 1] + Y;
	int BA = mNoiseMap[B] + Z;		// HASH COORDINATES OF
	int BB = mNoiseMap[B + 1] + Z;	// THE 8 CUBE CORNERS

	Vec3 ul = origin - Vec3(1, 0, 0);
	Vec3 br = origin - Vec3(0, 1, 0);
	Vec3 bl = origin - Vec3(1, 1, 0);
	Vec3 fur = origin - Vec3(0, 0, 1);
	Vec3 ful = origin - Vec3(1, 0, 1);
	Vec3 fbr = origin - Vec3(0, 1, 1);
	Vec3 fbl = origin - Vec3(1, 1, 1);

	// BLEND RESULTS FROM 8 CORNERS OF CUBE
	vv0 = _lerp(u, _grad(mNoiseMap[AA], origin), _grad(mNoiseMap[BA], ul));
	vv1 = _lerp(u, _grad(mNoiseMap[AB], br), _grad(mNoiseMap[BB], bl));
	vv2 = _lerp(u, _grad(mNoiseMap[AA + 1], fur), _grad(mNoiseMap[BA + 1], ful));
	vv3 = _lerp(u, _grad(mNoiseMap[AB + 1], fbr), _grad(mNoiseMap[BB + 1], fbl));
}