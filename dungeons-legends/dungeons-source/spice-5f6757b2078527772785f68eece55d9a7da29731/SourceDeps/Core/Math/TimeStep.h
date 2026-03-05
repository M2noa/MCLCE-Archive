#pragma once

#include <chrono>

namespace mce {

	typedef float time_step_t;

	struct TimeStep {

		TimeStep(const time_step_t deltaTime)
			: mDeltaTime(deltaTime)
			, mDeltaTimeSquared(deltaTime * deltaTime)
			, mAlpha()
			, mInterpolatedTime()
			, mFastForward(false) {

		}

		bool isInterpolated() const {
			return mAlpha != 0.f;
		}

		TimeStep& operator = (const TimeStep& timeStep) = delete;

		const time_step_t mDeltaTime;
		const time_step_t mDeltaTimeSquared;
		float mAlpha;
		time_step_t mInterpolatedTime;
		bool mFastForward;
		std::chrono::high_resolution_clock::time_point mTime;
	};

}
