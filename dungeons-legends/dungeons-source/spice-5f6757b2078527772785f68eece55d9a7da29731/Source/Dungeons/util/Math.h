/********************************************************
*   (c) Mojang. All rights reserved                       *
*   (c) Microsoft. All rights reserved.                   *
*********************************************************/
#pragma once

#include <cmath>
#include <climits>
#include <algorithm>
#include <set>
#include <vector>
#include <type_traits>

#include "Core/Utility/buffer_span.h"
//#include "Core/Debug/DebugUtils.h"

class Random;
class Vec2;
class Vec3;

class Math {

	static float mSin[];
	static const float mSinScale;

public:

	//static const float PI;
	static const float TAU;
	static const float RADDEG;
	static const float DEGRAD;

	static void initMth() {
		for (int i = 0; i < 65536; ++i) {
			mSin[i] = ::sin(i / mSinScale);
		}
	}

	static float sqrt(float x) {
		const float ret = ::sqrt(x);
		return ret;
	}

	static __inline float fastInvSqrt(float x) {
		const float xhalf = 0.5f * x;
		int i = *(int*)&x;
		i = 0x5f3759df - (i >> 1);
		x = *(float*)&i;
		x = x * (1.5f - xhalf * x * x);
		return x;
	}

	static float invSqrt(float x) {
		// 1.0f / sqrt(x);
		return fastInvSqrt(x);
	}

	static int floor(float v) {
		const int i = (int) v;
		return v < i ? i - 1 : i;
	}

	static int ceil(float v) {
		const int i = (int)v;
		return v == i ? i : i + 1;
	}

	static float cos(float x) {
		return mSin[(int)(x * mSinScale + 65536 / 4) & 65535];
	}

	static float sin(float x) {
		return mSin[(int)(x * mSinScale) & 65535];
	}

	static float atan(float x) {
		return ::atan(x);
	}

	static float atan2(float dy, float dx) {
		return ::atan2(dy, dx);
	}

	template<typename T>
	static T sign(T input){
		return static_cast<T>((input > 0) ? 1 : ((input < 0) ? -1 : 0));
	}

	static void safeIncrement(int& i) {
		if (i < INT_MAX) {
			++i;
		}
	}

	static float random();

	static int random(int n);

	static int nextInt(Random& random, int minInclusive, int maxInclusive);
	static float nextFloat(Random& random, float min, float max);
	static double nextDouble(Random& random, double min, double max);

	static unsigned int fastRandom() {
		static unsigned int x = 123456789;
		static unsigned int y = 362436069;
		static unsigned int z = 521288629;
		static unsigned int w = 88675123;

		const unsigned int t = x ^ (x << 11);
		x = y;
		y = z;
		z = w;
		return w = w ^ (w >> 19) ^ (t ^ (t >> 8));
	}

	static int intFloorDiv(int a, int b) {
		if (a < 0) {
			return -((-a - 1) / b) - 1;
		}
		return a / b;
	}

	static float absDecrease(float value, float with, float min) {
		if (value > 0) {
			return std::max(min, value - with);
		}
		return std::min(value + with, -min);
	}

	//float absIncrease(float value, float with, float max);
	
	template
	<typename T>
	static int round(T val) {
		return static_cast<int>(::round(val));
	}

	template
	<typename T1, typename T2>
	static auto max(T1 a, T2 b) -> typename std::remove_reference<decltype(a > b ? a : b)>::type {
		return a > b ? a : b;
	}

	template
	<typename T1, typename T2>
	static auto min(T1 a, T2 b) -> typename std::remove_reference<decltype(a < b ? a : b)>::type {
		return a < b ? a : b;
	}

	template<typename T>
	static T clamp(T v, T low, T high) {
		if (v > high) {
			return high;
		}
		return v > low ? v : low;
	}

	static float wrapDegrees(float input) {
		input = std::fmod(input, 360.0f);
		if (input >= 180) {
			input -= 360;
		}
		if (input < -180) {
			input += 360;
		}
		return input;
	}

	static float clampRotate(float current, float target, float maxDelta) {
		float diff = wrapDegrees(target - current);
		if (diff > maxDelta) {
			diff = maxDelta;
		}
		if (diff < -maxDelta) {
			diff = -maxDelta;
		}
		return current + diff;
	}

	static float lerp(float src, float dst, float alpha) {
		return src + (dst - src) * alpha;
	}

	static int lerp(int src, int dst, float alpha) {
		return src + (int)((dst - src) * alpha);
	}

	static float lerpRotate(float from, float to, float a) {
		return from + a * wrapDegrees(to - from);
	}

	static bool isNan(float f) {
		// volatile to get around android compiler optimizing this out. Not using std::nan because it returns false negatives on android
		volatile float temp = f;
		return !(temp == temp);
	}

	static float absMax(float a, float b) {
		if (a < 0) {
			a = -a;
		}
		if (b < 0) {
			b = -b;
		}
		return a > b ? a : b;
	}

	static float absMaxSigned(float a, float b) {
		return std::abs(a) > std::abs(b) ? a : b;
	}

	static float clampedLerp(float min, float max, float factor) {
		if(factor < 0) {
			return min;
		}
		if(factor > 1) {
			return max;
		}
		return min + (max - min) * factor;
	}

	static float signum(float a){
		return (a == 0.0f) ? 0.0f : (a < 0.0f) ? -1.0f : 1.0f;
	}

	static float wrap(float numer, float denom) {
		const float result = std::fmod(numer, denom);
		return result >= 0.0f ? result : (result + denom);
	}

	static float snapRotationToCardinal(float deg) {
		// Round to nearest 90 degrees, signum to round away from zero, so quadrant -0.8 rounds to -1 and 0.8 rounds to 1
		int quadrant = static_cast<int>(deg / 90.0f + signum(deg)*0.5f) % 4;
		// Now that we've rounded, bring it back from quadrants to degrees
		return static_cast<float>(quadrant * 90);
	}

	static float remainder(float dividend, float divisor) {
		return dividend - (trunc(dividend / divisor) * divisor);
	}

	// smoothly blends from 0.0 to 1.0, with a nice ease-in and ease-out
	static float hermiteBlend(float t) {
		return 3 * t*t - 2 * t*t*t;
	}

	static float log2(float t) {
		return std::log(t) / std::log(2.f);
	}

	template<class T>
	static size_t hash_accumulate(size_t hash, const T& toHash) {
		return hash ^ std::hash<T>()(toHash) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
	}

	template<class T>
	static size_t hash2(const T& a, const T& b) {
		size_t hash = hash_accumulate(0, a);
		return hash_accumulate(hash, b);
	}

	template<class T>
	static size_t hash3(const T& a, const T& b, const T& c) {
		size_t hash = hash_accumulate(0, a);
		hash = hash_accumulate(hash, b);
		return hash_accumulate(hash, c);
	}
};

namespace Util {
	template<class T>
	int removeAll(std::vector<T>& superset, const std::vector<T>& toRemove) {
		const int subSize = (int)toRemove.size();
		int removed = 0;

		for (int i = 0; i < subSize; ++i) {
			T elem = toRemove[i];
			const int size = (int)superset.size();

			for (int j = 0; j < size; ++j) {
				if (elem == superset[j]) {
					superset.erase( superset.begin() + j, superset.begin() + j + 1);
					++removed;
					break;
				}
			}
		}
		return removed;
	}

	template<class T>
	bool remove(std::vector<T>& list, const T& instance) {
		typename std::vector<T>::iterator it = std::find(list.begin(), list.end(), instance);
		if (it == list.end()) {
			return false;
		}

		list.erase(it);
		return true;
	}

	// Could perhaps do a template<template ..>
	template<class T>
	bool remove(std::set<T>& list, const T& instance) {
		typename std::set<T>::iterator it = std::find(list.begin(), list.end(), instance);
		if (it == list.end()) {
			return false;
		}

		list.erase(it);
		return true;
	}

	template <typename T>
	T swapEndian(T u)
	{
		static_assert (CHAR_BIT == 8, "CHAR_BIT != 8");

		union
		{
			T u;
			unsigned char u8[sizeof(T)];
		} source, dest;

		source.u = u;

		for (size_t k = 0; k < sizeof(T); k++)
			dest.u8[k] = source.u8[sizeof(T) - k - 1];

		return dest.u;
	}


	template<typename T>
	bool isAllZero(buffer_span<T> span) {
		DEBUG_ASSERT(span.byte_size() % sizeof(size_t) == 0, "Not aligned");\
		const size_t* ptr = reinterpret_cast<const size_t*>(span.data());
		const size_t* end = ptr + span.byte_size() / sizeof(size_t);

		size_t accum = 0;
		for (; ptr < end && accum == 0; ++ptr) {
			accum |= *ptr;
		}
		return accum == 0;
	}

	template<typename T>
	bool isAllOne(buffer_span<T> span) {
		DEBUG_ASSERT(span.byte_size() % sizeof(size_t) == 0, "Not aligned");

		const size_t* ptr = reinterpret_cast<const size_t*>(span.data());
		const size_t* end = ptr + span.byte_size() / sizeof(size_t);

		size_t accum = SIZE_MAX;
		for (; ptr < end && accum == SIZE_MAX; ++ptr) {
			accum &= *ptr;
		}
		return accum == SIZE_MAX;
	}


}

template<class T>
class Interpolated {
public:

	Interpolated() :
		mValue()
		, mOldValue() {
	}

	Interpolated(T start) :
		mValue(start)
		, mOldValue(start) {
	}

	void tick(T newValue){
		mOldValue = mValue;
		mValue = newValue;
	}

	void tick(){
		mOldValue = mValue;
	}

	T get(float a) const {
		return mOldValue + a * (mValue - mOldValue);
	}

private:
	T mValue, mOldValue;
};
