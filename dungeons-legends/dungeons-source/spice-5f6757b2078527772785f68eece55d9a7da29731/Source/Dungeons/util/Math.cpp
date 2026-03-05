#include "Dungeons.h"

#include "util/Math.h"
#include "util/Random.h"
#include "world/phys/Vec2.h"
#include "world/phys/Vec3.h"

// #include "Performance.h"

namespace {
	class MathInitializer {
	public:
		MathInitializer() {
			Math::initMth();
		}
	};

	MathInitializer mathInitializer;
}

static Random _rand;

// const float PI = 3.1415926535897932384626433832795028841971f;	// exactly!
const float Math::TAU = 2.0f * PI;	// exactly!
const float Math::DEGRAD = PI / 180.0f;
const float Math::RADDEG = 180.0f / PI;

float Math::mSin[65536];
const float Math::mSinScale = 65536.0f / (2.0f * PI);

float Math::random() {
	return _rand.nextFloat();
}

int Math::random(int n){
	return _rand.nextInt(n);
}

int Math::nextInt(Random& random, int minInclusive, int maxInclusive){
	if (minInclusive >= maxInclusive) {
		return minInclusive;
	}
	return random.nextInt(maxInclusive - minInclusive + 1) + minInclusive;
}

float Math::nextFloat(Random& random, float min, float max) {
	if (min >= max) {
		return min;
	}
	return random.nextFloat() * (max - min) + min;
}

double Math::nextDouble(Random& random, double min, double max) {
	if (min >= max) {
		return min;
	}
	return random.nextDouble() * (max - min) + min;
}