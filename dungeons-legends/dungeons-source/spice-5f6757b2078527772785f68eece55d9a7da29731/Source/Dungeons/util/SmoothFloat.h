/********************************************************
*   (c) Mojang. All rights reserved                       *
*   (c) Microsoft. All rights reserved.                   *
*********************************************************/
#pragma once

class SmoothFloat {
	float mTargetValue;
	float mRemainingValue;
	float mLastAmount;

public:

	SmoothFloat()
		:   mTargetValue(0)
		, mRemainingValue(0)
		, mLastAmount(0){
	}

	float getNewDeltaValue(float deltaValue, float accelerationAmount) {
		mTargetValue += deltaValue;

		deltaValue = (mTargetValue - mRemainingValue) * accelerationAmount;
		mLastAmount = mLastAmount + (deltaValue - mLastAmount) * .5f;
		if ((deltaValue > 0 && deltaValue > mLastAmount) || (deltaValue < 0 && deltaValue < mLastAmount)) {
			deltaValue = mLastAmount;
		}
		mRemainingValue += deltaValue;

		return deltaValue;
	}

	float getTargetValue() const {
		return mTargetValue;
	}

};
