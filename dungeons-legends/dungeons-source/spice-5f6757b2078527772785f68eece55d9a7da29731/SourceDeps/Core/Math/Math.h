#pragma once

namespace mce {
	class Math {
	public:

		// 4th Degree Lagrange Polynomial using samples at x = [-1, 0.5, 0, 0.5, 1]
		// Max error about 0.18 rad (10.3 deg) peaking at about x = +/- 0.9
		static float acos_approx(float x) {
			// acos(x) ~ (-2pi/9)x^3 - (5pi/18)x + (pi/2)
			return -0.698131700798f * x * x * x - 0.872664625997f * x + 1.57079632679f;
		}

		static int clamp(int v, int low, int high) {
			if (v > high) {
				return high;
			}
			return v > low ? v : low;
		}

		static float clamp(float v, float low, float high) {
			if (v > high) {
				return high;
			}
			return v > low ? v : low;
		}

	};
}
