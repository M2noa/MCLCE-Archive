/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/

#pragma once

#include "nbt/CompoundTag.h"

#include <json/json.h>
#include <set>

class Block;

class BlockListSerializer {
public:
	static void loadJSONSet(const std::string& name, std::set<const Block*>& outBlocks, Json::Value root);

	/*
	adors
	If someone needs to add NBT and wants reference, refer to ItemListSerializer
	*/
};

