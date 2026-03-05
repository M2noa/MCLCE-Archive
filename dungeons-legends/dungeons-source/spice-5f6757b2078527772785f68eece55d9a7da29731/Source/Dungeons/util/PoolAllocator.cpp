#include "Dungeons.h"

#include "util/PoolAllocator.h"

// #include "platform/threading/WorkerPool.h"
//#include "platform/threading/atomicops.h"
#include "util/StringUtils.h"

#ifdef _DEBUG
#define NOPOOL
#endif

PoolAllocator::PoolAllocator(int blockSize ) :
	MemoryTracker("Pool " + Util::toNiceString(blockSize) + "B" )
	, mOwnerID(std::this_thread::get_id())
	, BLOCK_SIZE(blockSize){

}

PoolAllocator::~PoolAllocator(){
	while (!mIdlePool.empty()) {
		if (mIdlePool.front() != nullptr) {
			free(mIdlePool.front());	 /*safe*/
		}
		mIdlePool.pop();
	}
}

void* PoolAllocator::get(){
#ifdef NOPOOL
	++mAllocated;
	return malloc(BLOCK_SIZE); /*safe*/
#else
	LockGuard lock(mPoolMutex);

	void* block;
	if (mIdlePool.empty()) {
		block = malloc(BLOCK_SIZE); /*safe*/
		++mAllocated;
	}
	else {
		block = mIdlePool.front();
		mIdlePool.pop();
	}

#ifdef DEBUG_STATS
	mBusyPool.insert(block);
#endif

	return block;
#endif
}

void PoolAllocator::release(void* block){
#ifdef NOPOOL
	free(block);	 /*safe*/
	--mAllocated;
#else
	LockGuard lock(mPoolMutex);

#ifdef DEBUG_STATS
	DEBUG_ASSERT(mBusyPool.count(block) == 1, "Error: released a block not owned by the pool!");
	mBusyPool.erase(block);
#endif

	mIdlePool.push(block);
#endif
}

bool PoolAllocator::trim(){
	DEBUG_ASSERT_MAIN_THREAD;

	LockGuard lock(mPoolMutex);

	if (mIdlePool.empty()) {
		return false;
	}

	while (!mIdlePool.empty()) {
		if (mIdlePool.front() != nullptr) {
			free(mIdlePool.front()); 	 /*safe*/
		}
		mIdlePool.pop();
		--mAllocated;
	}
	return true;
}

MemoryStats PoolAllocator::getStats() const {
	return MemoryStats(mAllocated * BLOCK_SIZE, mAllocated);
}
