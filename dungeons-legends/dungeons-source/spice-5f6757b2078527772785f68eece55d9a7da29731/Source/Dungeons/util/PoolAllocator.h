/********************************************************
*   (c) Mojang. All rights reserved                       *
*   (c) Microsoft. All rights reserved.                   *
*********************************************************/
#pragma once

#include "common_header.h"

#include "Core/Utility/MemoryTracker.h"

class PoolAllocator :
	MemoryTracker {
public:

	const int BLOCK_SIZE;

	PoolAllocator( int blockSize );

	~PoolAllocator();

	int getAllocatedCount() const {
		return mAllocated;
	}

	void* get();

	void release(void* block);

	bool trim();

	virtual MemoryStats getStats() const override;

protected:

	std::thread::id mOwnerID;

	std::queue<void*> mIdlePool;

	int mAllocated = 0;

	std::mutex mPoolMutex;

#ifdef DEBUG_STATS
	std::unordered_set<void*> mBusyPool;
#endif

};
