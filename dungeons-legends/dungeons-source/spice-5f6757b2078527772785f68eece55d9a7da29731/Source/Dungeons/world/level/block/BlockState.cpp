/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#include "BlockState.h"

BlockState::BlockState() 
	: mStartBit(0)
	, mNumBits(0)
	, mInitialized(0) {
}

void BlockState::initState(unsigned int& startBit, unsigned int numBits) {
	DEBUG_ASSERT(!mInitialized, "This state is already initialized! If you are using resetBlockState() and the same state twice, it could cause undefined behavior!");
	if (!mInitialized) {
		DEBUG_ASSERT(startBit < 4, "Start bit out of range!");

		mNumBits = numBits;
		mStartBit = startBit + mNumBits - 1;

		DEBUG_ASSERT(mStartBit < 4, "The bits covered by the state are out of range!");
		mInitialized = true;

		// update used bits for the block
		startBit += numBits;
	}
}

bool BlockState::isInitialized() const {
	return mInitialized;
}

bool BlockState::getBool(const DataID& data) const {
	DEBUG_ASSERT(mInitialized, "This block state was not added to this block when it was registered.");
	return ((0x1 << mStartBit) & data) > 0;
}
