#pragma once

/*
          A random generator class based on Mersenne-Twister.

   "UPDATE"
   http://www.math.sci.hiroshima-u.ac.jp/~m-mat/MT/MT2002/elicense.html
   ---------------------------------------------------------------------

   Commercial Use of Mersenne Twister
   2001/4/6

   Until 2001/4/6, MT had been distributed under GNU Public License, but
   after 2001/4/6, we decided to let MT be used for any purpose, including
   commercial use. 2002-versions mt19937ar.c, mt19937ar-cok.c are considered
   to be usable freely.

 */
#include "common_header.h"

#include "CommonTypes.h"
#include "world/phys/Vec3.h"

#include <random>

class Random {
public:

	Random() {
		static std::random_device rnd;
		setSeed(rnd());
	}

	Random(RandomSeed seed) {
		setSeed( seed );
	}

	void setSeed(RandomSeed seed) {
		_seed = seed;
		_mti = N + 1;
		haveNextNextGaussian = false;
		nextNextGaussian = 0;
		init_genrand(seed);
	}

	RandomSeed getSeed() {
		return _seed;
	}

	bool nextBoolean() {
		return (genrand_int32() & 0x8000000) > 0;
	}

	float nextFloat() {
		return (float)genrand_real2();
	}

	float nextFloat(float max) {
		return nextFloat() * max;
	}

	float nextFloat(float min, float max) {
		return min + nextFloat() * (max - min);
	}

	double nextDouble() {
		return genrand_real2();
	}

	// @note: This returns a positive integer value
	int nextInt() {
		return (int)(genrand_int32() >> 1);
	}

	int nextInt(int n) {
		DEBUG_ASSERT(n > 0, "Can't generate a number in [0,0)");
		return n == 0 ? 0 : genrand_int32() % n;
	}

	//implement the RandomFunc concept from STL
	int operator()(int n) {
		return nextInt(n);
	}

	int nextInt(int min, int max) {
		return min + nextInt(max - min);
	}

	int nextUnsignedInt() {
		return (int)genrand_int32();
	}

	int nextUnsignedInt(unsigned int n) {
		DEBUG_ASSERT(n > 0, "Can't generate a number in [0,0)");
		return genrand_int32()  % n;
	}

	unsigned char nextUnsignedChar() {
		return genrand_int32() % 256;
	}

	float nextGaussian() {
		if (haveNextNextGaussian) {
			haveNextNextGaussian = false;
			return nextNextGaussian;
		}
		else {
			float v1, v2, s;
			do {
				v1 = 2 * nextFloat() - 1;	// between -1.0 and 1.0
				v2 = 2 * nextFloat() - 1;	// between -1.0 and 1.0
				s = v1 * v1 + v2 * v2;
			} while (s >= 1 || s == 0);
			float multiplier = std::sqrt(-2 * std::log(s) / s);
			nextNextGaussian = v2 * multiplier;
			haveNextNextGaussian = true;
			return v1 * multiplier;
		}
	}

	int nextGaussianInt(int n) {
		const int rand0 = nextInt(n);
		return rand0 - nextInt(n);
	}

	float nextGaussianFloat() {
		const auto rand0 = nextFloat();
		return rand0 - nextFloat();
	}

private:

	RandomSeed _seed;

	/* Period parameters */
	static const int N = 624;
	static const int M = 397;
	static const unsigned int MATRIX_A = 0x9908b0dfUL;	/* constant vector a */
	static const unsigned int UPPER_MASK = 0x80000000UL;/* most significant w-r bits */
	static const unsigned int LOWER_MASK = 0x7fffffffUL;/* least significant r bits */

	uint32_t _mt[N];/* the array for the state vector  */
	int _mti;	/* _mti==N+1 means _mt[N] is not initialized */

	bool haveNextNextGaussian;
	float nextNextGaussian;

	/* initializes _mt[N] with a seed */
	void init_genrand(uint32_t s){
		_mt[0] = s;

		for (_mti = 1; _mti < N; _mti++) {
			_mt[_mti] =
				(1812433253 * (_mt[_mti - 1] ^ (_mt[_mti - 1] >> 30)) + _mti);
			/* See Knuth TAOCP Vol2. 3rd Ed. P.106 for multiplier. */
			/* In the previous versions, MSBs of the seed affect   */
			/* only MSBs of the array _mt[].                        */
			/* 2002/01/09 modified by Makoto Matsumoto             */
			//_mt[_mti] &= 0xffffffffUL;
			/* for >32 bit machines */
		}
	}

	/* initialize by an array with array-length */
	/* init_key is the array for initializing keys */
	/* key_length is its length */
	/* slight change for C++, 2004/2/26 */
	void init_by_array(uint32_t init_key[], int key_length){
		int i, j, k;
		init_genrand(19650218UL);
		i = 1;
		j = 0;
		k = (N > key_length ? N : key_length);

		for (; k; k--) {
			_mt[i] = (_mt[i] ^ ((_mt[i - 1] ^ (_mt[i - 1] >> 30)) * 1664525UL))
				+ init_key[j] + j;	/* non linear */
			_mt[i] &= 0xffffffffUL;	/* for WORDSIZE > 32 machines */
			i++;
			j++;
			if (i >= N) {
				_mt[0] = _mt[N - 1];
				i = 1;
			}
			if (j >= key_length) {
				j = 0;
			}
		}

		for (k = N - 1; k; k--) {
			_mt[i] = (_mt[i] ^ ((_mt[i - 1] ^ (_mt[i - 1] >> 30)) * 1566083941UL))
				- i;/* non linear */
			_mt[i] &= 0xffffffffUL;	/* for WORDSIZE > 32 machines */
			i++;
			if (i >= N) {
				_mt[0] = _mt[N - 1];
				i = 1;
			}
		}

		_mt[0] = 0x80000000UL;	/* MSB is 1; assuring non-zero initial array */
	}

	/* generates a random number on [0,0xffffffff]-interval */
	uint32_t genrand_int32(void){
		uint32_t y;
		static uint32_t mag01[2] = {
			0x0UL, MATRIX_A
		};
		/* mag01[x] = x * MATRIX_A  for x=0,1 */

		// Do a lazy evaluation of the state vector. Most of the time we don't use anywhere near the 694 entries in the
		// vector
		// On a slow windows phone this saved 30 ms per chunk on average
		if (_mti >= N + 1) {
			init_genrand(5489UL);	// a default initial seed is used
			_mti = 0;
		}
		else if (_mti == N) {
			_mti = 0;
		}

		if (_mti < N - M) {
			y = (_mt[_mti] & UPPER_MASK) | (_mt[_mti + 1] & LOWER_MASK);
			_mt[_mti] = _mt[_mti + M] ^ (y >> 1) ^ mag01[y & 0x1UL];
		}
		else if (_mti < N - 1) {
			y = (_mt[_mti] & UPPER_MASK) | (_mt[_mti + 1] & LOWER_MASK);
			_mt[_mti] = _mt[_mti + (M - N)] ^ (y >> 1) ^ mag01[y & 0x1UL];
		}
		else {
			y = (_mt[N - 1] & UPPER_MASK) | (_mt[0] & LOWER_MASK);
			_mt[N - 1] = _mt[M - 1] ^ (y >> 1) ^ mag01[y & 0x1UL];
		}

		y = _mt[_mti++];

		/* Tempering */
		y ^= (y >> 11);
		y ^= (y << 7) & 0x9d2c5680UL;
		y ^= (y << 15) & 0xefc60000UL;
		y ^= (y >> 18);

		return y;
	}

	/* generates a random number on [0,0x7fffffff]-interval */
	int genrand_int31(void){
		return (int)(genrand_int32() >> 1);
	}

	/* generates a random number on [0,1]-real-interval */
	double genrand_real1(void){
		return genrand_int32() * (1.0 / 4294967295.0);
		/* divided by 2^32-1 */
	}

	/* generates a random number on [0,1)-real-interval */
	double genrand_real2(void){
		return genrand_int32() * (1.0 / 4294967296.0);
		/* divided by 2^32 */
	}

	/* generates a random number on (0,1)-real-interval */
	double genrand_real3(void){
		return (((double)genrand_int32()) + 0.5) * (1.0 / 4294967296.0);
		/* divided by 2^32 */
	}

	/* generates a random number on [0,1) with 53-bit resolution*/
	double genrand_res53(void){
		uint32_t a = genrand_int32() >> 5, b = genrand_int32() >> 6;
		return (a * 67108864.0 + b) * (1.0 / 9007199254740992.0);
	}

	/* These real versions are due to Isaku Wada, 2002/01/09 added */

	//
	// Added helper (and quicker) functions
	//
	void rrDiff(float& x) {
		uint32_t u = genrand_int32();
		const float xx0 = (u        & 0xffff) / 65536.0f;	// 2 x 16 bits
		const float xx1 = ((u >> 16) & 0xffff) / 65536.0f;
		x = xx0 - xx1;
	}

	void rrDiff(float& x, float& y) {
		uint32_t u = genrand_int32();
		const float xx0 = ((u) & 0xff) / 256.0f;		// 4 x 8 bits
		const float xx1 = ((u >> 8) & 0xff) / 256.0f;
		const float yy0 = ((u >> 16) & 0xff) / 256.0f;
		const float yy1 = ((u >> 24) & 0xff) / 256.0f;
		x = xx0 - xx1;
		y = yy0 - yy1;
	}

	void rrDiff(float& x, float& y, float& z) {
		uint32_t u = genrand_int32();
		const float xx0 = ((u) & 0x1f) / 32.0f;		// 6 x 5 bits
		const float xx1 = ((u >> 5) & 0x1f) / 32.0f;
		const float yy0 = ((u >> 10) & 0x1f) / 32.0f;
		const float yy1 = ((u >> 15) & 0x1f) / 32.0f;
		const float zz0 = ((u >> 20) & 0x1f) / 32.0f;
		const float zz1 = ((u >> 25) & 0x1f) / 32.0f;
		x = xx0 - xx1;
		y = yy0 - yy1;
		z = zz0 - zz1;
	}

public:

	// @note: Do not use this (yet) in logic that needs
	//        to stay the same over versions (e.g. level gen)
	void next2float16(float& a, float& b) {
		uint32_t u = genrand_int32();
		a = (u & 0xffff) / 65536.0f;
		b = ((u >> 16) & 0xffff) / 65536.0f;
	}

	// @note: Do not use this (yet) in logic that needs
	//        to stay the same over versions (e.g. level gen)
	void next4float8(float& a, float& b, float& c, float& d) {
		uint32_t u = genrand_int32();
		a = ((u) & 0xff) / 256.0f;
		b = ((u >>  8) & 0xff) / 256.0f;
		c = ((u >> 16) & 0xff) / 256.0f;
		d = ((u >> 24) & 0xff) / 256.0f;
	}

	Vec3 nextVec3() {
		float randomX = nextFloat();
		float randomY = nextFloat();
		float randomZ = nextFloat();
		return Vec3(randomX, randomY, randomZ);
	}

	Vec3 nextVec3(float f) {
		float randomX = nextFloat(f);
		float randomY = nextFloat(f);
		float randomZ = nextFloat(f);
		return Vec3(randomX, randomY, randomZ);
	}

	Vec3 nextGaussianVec3() {
		float randomX = nextGaussian();
		float randomY = nextGaussian();
		float randomZ = nextGaussian();
		return Vec3(randomX, randomY, randomZ);
	}

};
