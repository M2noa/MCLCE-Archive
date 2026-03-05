/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/
#include "Dungeons.h"

#include "BlockQueue.h"

void BlockQueue::reset() {
	mSize = mIndexCount = 0;
}

void BlockQueue::add(const BlockPos& pos) {
	mQueue[mSize++] = pos;
}

bool BlockQueue::empty() {
	return mSize == 0;
}

BlockQueue::Iterator BlockQueue::begin() {
	return Iterator(*this, 0);
}

BlockQueue::Iterator BlockQueue::end() {
	return Iterator(*this, mSize);
}

int BlockQueue::getSize() const {
	return mSize;
}

int BlockQueue::getIndexCount() const {
	return mIndexCount;
}

void BlockQueue::setIndexCount(int IndexCount) {
	mIndexCount = IndexCount;
}

BlockPos* BlockQueue::getQueue() {
	return mQueue;
}

bool BlockQueue::Iterator::operator!=(const Iterator& other) const {
	return mIndex != other.mIndex || &mBase != &other.mBase;
}

BlockQueue::Iterator::Iterator(BlockQueue& base, int idx) 
	: mBase(base)
	, mIndex(idx) {

}

BlockQueue::Iterator& BlockQueue::Iterator::operator++() {
	++mIndex;
	return *this;
}

BlockPos& BlockQueue::Iterator::operator*() {
	return mBase.mQueue[mIndex];
}
