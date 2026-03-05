/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/
#pragma once

#include "common_header.h"
#include "world/level/LevelConstants.h"
#include "world/level/BlockPos.h"

class BlockQueue {
public:

	class Iterator {
	public:
		Iterator(BlockQueue& base, int idx);
		
		Iterator& operator++();
		bool operator!=(const Iterator& other) const;
		BlockPos& operator*();

	protected:
		int mIndex;
		BlockQueue& mBase;
	};

	void reset();
	void add(const BlockPos& pos);
	bool empty();

	Iterator begin();
	Iterator end();

	int getSize() const;

	int getIndexCount() const;
	void setIndexCount(int IndexCount);

	BlockPos* getQueue();

private:
	BlockPos mQueue[RENDERCHUNK_VOLUME];
	int mSize;
	int mIndexCount;
};
