/********************************************************
*   (c) Mojang. All rights reserved                       *
*   (c) Microsoft. All rights reserved.                   *
*********************************************************/
#pragma once

class RetryDelay {
public:

	RetryDelay(bool startEnabled, int delayMs)
		: mDelayMs(delayMs)
		, mEnabled(startEnabled){
	}

	bool tryExecute(int timeMs) {
		if (!mEnabled || mActive) {
			return false;
		}

		if (mPreviousTriggerMs + mDelayMs <= timeMs) {
			mPreviousTriggerMs = timeMs;
			mActive = true;
			return true;
		}
		else {
			return false;
		}
	}

	void enable() {
		mEnabled = true;
	}

	void disable() {
		mActive = false;
		mEnabled = false;
		mPreviousTriggerMs = INT_MIN;
	}

	void restart() {
		DEBUG_ASSERT(mActive, "Should only be called when it's already active.");
		mActive = false;
	}

	bool isDisabled() const {
		return !mEnabled;
	}

private:

	const int mDelayMs;
	int mPreviousTriggerMs = INT_MIN;
	bool mActive = false;
	bool mEnabled = true;
};
