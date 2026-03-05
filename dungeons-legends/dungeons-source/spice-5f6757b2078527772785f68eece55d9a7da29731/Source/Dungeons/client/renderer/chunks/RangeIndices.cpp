/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/
#include "Dungeons.h"

#include "RangeIndices.h"

int RangeIndices::set(int s, int c) {
	DEBUG_ASSERT(s >= 0, "Invalid start");
	DEBUG_ASSERT(c >= 0, "Invalid Count");
	mStart = s;
	mCount = c;

	return s + c;
}

int RangeIndices::getPolyCount() const {
	return mCount / 3;
}
