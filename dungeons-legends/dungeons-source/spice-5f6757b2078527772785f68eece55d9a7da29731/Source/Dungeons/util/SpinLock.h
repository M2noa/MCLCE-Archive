/********************************************************
*   (c) Mojang. All rights reserved                       *
*   (c) Microsoft. All rights reserved.                   *
*********************************************************/
#pragma once

#include "common_header.h"

class SpinLock {
public:

	SpinLock() {
		mLocked.clear();
	}

	bool try_lock() {
		//return true if we set the lock to true and previously it was false
		return mLocked.test_and_set(std::memory_order_acquire) == false;
	}

	void lock() {
		while (!try_lock()) {
		}
	}

	void unlock() {
		mLocked.clear(std::memory_order_release);
	}

private:

	std::atomic_flag mLocked;
};
