/********************************************************
*   (c) Mojang. All rights reserved                       *
*   (c) Microsoft. All rights reserved.                   *
*********************************************************/
#pragma once

#include "common_header.h"

template<typename F, int MAXCOUNT>
class RunningAverage {
public:

	RunningAverage() {
		static_assert(MAXCOUNT > 0, "Invalid max count");
	}

	void append(const F& s) {
		mTotal += s;
		mSamples.push_back(s);

		if (mSamples.size() == MAXCOUNT) {
			mTotal -= mSamples.front();
			mSamples.pop_front();
		}
	}

	F getAverage() const {
		return mSamples.empty() ? F(0) : mTotal / (F)mSamples.size();
	}

protected:

	F mTotal = 0;
	std::deque<F> mSamples;
};
