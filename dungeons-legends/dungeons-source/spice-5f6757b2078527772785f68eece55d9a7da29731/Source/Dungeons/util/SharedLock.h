/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

// iOS and MacOS do not support shared mutex
// unless your device is on MacOS 10.12 and
// iOS 10 which we cannot assume.
#ifndef MCPE_PLATFORM_APPLE
#include <shared_mutex>
#endif

class SharedMutex;

///////////////////////////////////////////////

class UniqueLock {
public:
	UniqueLock(SharedMutex& mutex);

private:
#ifndef MCPE_PLATFORM_APPLE
	std::unique_lock<std::shared_timed_mutex> mLock;
#else
	std::lock_guard<std::mutex> mLock;
#endif
};

///////////////////////////////////////////////

class SharedLock {
public:
	SharedLock(SharedMutex& mutex);

private:
#ifndef MCPE_PLATFORM_APPLE
	std::shared_lock<std::shared_timed_mutex> mLock;
#else
	std::lock_guard<std::mutex> mLock;
#endif
};

///////////////////////////////////////////////

class SharedMutex {
private:
	friend class UniqueLock;
	friend class SharedLock;

#ifndef MCPE_PLATFORM_APPLE
	std::shared_timed_mutex mMutex;
#else
	std::mutex mMutex;
#endif
};
