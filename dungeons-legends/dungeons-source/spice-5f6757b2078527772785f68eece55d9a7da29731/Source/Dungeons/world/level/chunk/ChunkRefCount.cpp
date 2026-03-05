/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/
#include "Dungeons.h"

#include "world/level/chunk/ChunkRefCount.h"

#include "world/level/chunk/LevelChunk.h"

ChunkRefCount::ChunkRefCount() 
	: mRefCount(0) {

}

ChunkRefCount::ChunkRefCount(LevelChunk& lc, int initial ) 
	: mChunk(&lc)
	, mRefCount(initial) {

}

ChunkRefCount::ChunkRefCount(ChunkPtr&& rhs, int initial) 
	: mChunk(std::move(rhs))
	, mRefCount(initial) {

}

ChunkRefCount::ChunkRefCount(ChunkRefCount&& rhs) {
	mChunk = std::move(rhs.mChunk);
	mRefCount = rhs.mRefCount;
	rhs.mRefCount = 0;
}

ChunkRefCount::~ChunkRefCount() {

}

ChunkRefCount& ChunkRefCount::operator=(ChunkRefCount&& rhs) {
	mChunk = std::move(rhs.mChunk);
	mRefCount = rhs.mRefCount;
	rhs.mRefCount = 0;
	return *this;
}

LevelChunk* ChunkRefCount::get() const {
	return mChunk.get();
}

LevelChunk* ChunkRefCount::operator->() const {
	return mChunk.get();
}

LevelChunk* ChunkRefCount::grab() {
	DEBUG_ASSERT_MAIN_THREAD;
	++mRefCount;
	return get();
}

int ChunkRefCount::release() {
	DEBUG_ASSERT_MAIN_THREAD;
	DEBUG_ASSERT(mRefCount > 0, "Releasing an already released chunk");

	return --mRefCount;
}

int ChunkRefCount::getRefCount() const {
	return mRefCount;
}

LevelChunk& ChunkRefCount::getChunk() {
	return *mChunk;
}

const LevelChunk& ChunkRefCount::getChunk() const {
	return *mChunk;
}

ChunkRefCount::ChunkPtr& ChunkRefCount::getChunkPtr() {
	return mChunk;
}
