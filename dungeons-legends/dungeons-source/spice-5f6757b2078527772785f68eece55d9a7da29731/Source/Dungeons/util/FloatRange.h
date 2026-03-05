/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

#include "util/Random.h"

struct FloatRange {
	float rangeMin = 0.0f;
	float rangeMax = 0.0f;

	float getValue(Random& random);
	bool parseJson(Json::Value node);
};
