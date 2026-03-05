/********************************************************
*   (c) Mojang. All rights reserved                       *
*   (c) Microsoft. All rights reserved.                   *
*********************************************************/
#pragma once

#include <string>

#include "util/Random.h"

class Container;
class Level;

namespace Util {
	class LootTableUtils {
	public:
		static void fillChest(Level& level, Container& container, Random& random, const std::string& tableName);

	};
}