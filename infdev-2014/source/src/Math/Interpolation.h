#pragma once

#include "./Math.h"

namespace Math
{
	namespace Interpolation
	{

		static inline float LinearTween(float t, float b, float c, float d)
		{
			return c * t / d + b;
		}

		static inline float EaseInQuad(float t, float b, float c, float d)
		{
			t /= d;
			return c * t * t + b;
		}

		static inline float EaseOutQuad(float t, float b, float c, float d)
		{
			t /= d;
			return -c * t * (t - 2) + b;
		}

		static inline float EaseInOutQuad(float t)
		{
			t *= 2;
			if(t < 1)
				return 0.5f * t * t;

			t--;
			return -0.5f * (t * (t - 2) - 1);
		}

		static inline float EaseInOutQuad(float t, float b, float c, float d)
		{
			t /= d * 0.5f;
			if(t < 1)
				return c * 0.5f * t * t + b;

			t--;
			return -c * 0.5f * (t * (t - 2) - 1) + b;
		}

		static inline float EaseInCubic(float t)
		{
			return t * t * t;
		}

		static inline float EaseInCubic(float t, float b, float c, float d)
		{
			t /= d;
			return c * t * t * t + b;
		}

		/// \brief Cubic ease line for interpolation (0...1 version)
		/// \param t time
		static float EaseInOutCubic(float t)
		{
			float m = round(t) * 2;
			t = t * 2.f - m;
			return 0.5f * (t * t * t + m);
		}

		/// \brief Cubic ease line for interpolation
		/// \brief t time
		/// \brief b start time
		/// \brief c delta time
		/// \brief d duration
		static float EaseInOutCubic(float t, float b, float c, float d)
		{
			t /= d * 0.5f;
			if(t < 1)
				return c * 0.5f * t * t * t + b;

			t -= 2;
			return c * 0.5f * (t * t * t + 2) + b;
		}

		static inline float EaseInQuart(float t, float b, float c, float d)
		{
			t /= d;
			return c * t * t * t * t + b;
		}

		static inline float EaseOutQuart(float t, float b, float c, float d)
		{
			t /= d;
			t--;
			return -c * (t * t * t * t - 1) + b;
		}

		static float EaseInOutQuart(float t, float b, float c, float d)
		{
			t /= d * 0.5f;
			if(t < 1)
				return c * 0.5f * t * t * t * t + b;

			t -= 2;
			return -c * 0.5f * (t * t * t * t - 2) + b;
		}

		static inline float EaseInQuint(float t, float b, float c, float d)
		{
			t /= d;
			return c * t * t * t * t * t + b;
		}

		static inline float EaseOutQuint(float t, float b, float c, float d)
		{
			t /= d;
			t--;
			return c * (t * t * t * t * t + 1) + b;
		}

		static float EaseInOutQuint(float t, float b, float c, float d)
		{
			t /= d * 0.5f;
			if(t < 1)
				return c * 0.5f * t * t * t * t * t + b;

			t -= 2;
			return c * 0.5f * (t * t * t * t * t + 2) + b;
		}

		static inline float EaseInSine(float t, float b, float c, float d)
		{
			return -c * cosf(t / d * (float(M_PI) * 0.5f)) + c + b;
		}

		static inline float EaseOutSine(float t, float b, float c, float d)
		{
			return c * sinf(t / d * (float(M_PI) * 0.5f)) + b;
		}

		static inline float EaseInOutSine(float t, float b, float c, float d)
		{
			return -c * 0.5f * (cos(float(M_PI) * t / d) - 1) + b;
		}

		static inline float EaseInExpo(float t, float b, float c, float d)
		{
			return c * powf(2, 10 * (t / d - 1)) + b;
		}

		static inline float EaseOutExpo(float t, float b, float c, float d)
		{
			return c * (-powf(2, -10 * t / d) + 1) + b;
		}

		static float EaseInOutExpo(float t, float b, float c, float d)
		{
			t /= d * 0.5f;
			if(t < 1)
				return c * 0.5f * powf(2, 10 * (t - 1)) + b;

			t--;
			return c * 0.5f * (-powf(2, -10 * t) + 2) + b;
		}

		static inline float EaseInCirc(float t, float b, float c, float d)
		{
			t /= d;
			return -c * (sqrt(1 - t * t) - 1) + b;
		}

		static inline float EaseOutCirc(float t, float b, float c, float d)
		{
			t /= d;
			t--;
			return c * sqrt(1 - t * t) + b;
		}

		static float EaseInOutCirc(float t, float b, float c, float d)
		{
			t /= d * 0.5f;
			if(t < 1)
				return -c * 0.5f * (sqrt(1 - t * t) - 1) + b;

			t -= 2;
			return c * 0.5f * (sqrt(1 - t * t) + 1) + b;
		}
	}
}