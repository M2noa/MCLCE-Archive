#include "Dungeons.h"

#include "util/SharedLock.h"

UniqueLock::UniqueLock(SharedMutex& mutex)
	: mLock(mutex.mMutex) {

}

SharedLock::SharedLock(SharedMutex& mutex)
	: mLock(mutex.mMutex) {
}
