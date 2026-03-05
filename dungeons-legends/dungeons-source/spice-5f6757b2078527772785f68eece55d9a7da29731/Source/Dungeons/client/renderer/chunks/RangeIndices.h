/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/
#pragma once

#include "common_header.h"

struct RangeIndices {
	int mStart = 0, mCount = 0;

	int set(int s, int c);
	int getPolyCount() const;

};
