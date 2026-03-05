/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/

#pragma once

#include "nbt/CompoundTag.h"

#include <json/json.h>
#include <set>

class Item;

class ItemListSerializer {
public:
	//doesn't seem like we're going to need saving much, so lets worry about that later
	static void loadJSONSet(const std::string& name, std::set<const Item*>& outItems, Json::Value root);

	static void saveNBTSet(const std::string& name, CompoundTag& tag, const std::set<const Item*>& items);
	static void loadNBTSet(const std::string& name, std::set<const Item*>& outItems, const CompoundTag& tag);
};
