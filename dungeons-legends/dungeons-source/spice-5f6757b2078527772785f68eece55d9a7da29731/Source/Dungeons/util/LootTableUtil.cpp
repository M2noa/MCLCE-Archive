/********************************************************
*   (c) Mojang. All rights reserved                       *
*   (c) Microsoft. All rights reserved.                   *
*********************************************************/

#include "Dungeons.h"

#include "util/LootTableUtil.h"

#include "world/Container.h"
#include "world/level/Level.h"
// #include "world/level/storage/loot/LootTable.h"
// #include "world/level/storage/loot/LootTables.h"
// #include "world/level/storage/loot/LootTableContext.h"

namespace Util {
	void LootTableUtils::fillChest(Level& level, Container& container, Random& random, const std::string& tableName) {
		if (level.isClientSide()) {
			return;
		}

		// TODO: rherlitz
// 		LootTable* table = LootTables::lookupByName(tableName, *level.getServerResourcePackManager());
// 		if (table == nullptr) {
// 			return;
// 		}
// 		
// 		auto context = LootTableContext::Builder(&level).create();
// 		table->fill(container, random, context);
	}
}