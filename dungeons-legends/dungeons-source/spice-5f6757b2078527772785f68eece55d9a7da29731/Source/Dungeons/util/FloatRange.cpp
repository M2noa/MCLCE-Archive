/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#include "util/FloatRange.h"

#include <json/json.h>


float FloatRange::getValue(Random& random) {
	return random.nextFloat(rangeMin, rangeMax);
}

bool FloatRange::parseJson(Json::Value node) {
	if (!node.isNull()) {
		if (node.isNumeric()) {
			rangeMin = rangeMax = node.asFloat();
			return true;
		}
		else if (node.isObject()) {
			rangeMin = node["range_min"].asFloat();
			rangeMax = node["range_max"].asFloat();
			return true;
		}
		else if (node.isArray() && node.size() >= 2) {
			rangeMin = node[0].asFloat();
			rangeMax = node[1].asFloat();
			if (rangeMax < rangeMin) {
				std::swap(rangeMin, rangeMax);
			}
		}
	}
	return false;
}
