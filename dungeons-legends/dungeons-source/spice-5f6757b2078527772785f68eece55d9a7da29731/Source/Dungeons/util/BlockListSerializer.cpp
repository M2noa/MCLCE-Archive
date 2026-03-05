/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#include "util/BlockListSerializer.h"

#include "world/level/block/Block.h"

void loadBlock(std::string& str, std::set<const Block*>& blocks) {
	const Block* block = Block::lookupByName(str);
	if (block) {
		blocks.insert(block);
	}
}

void BlockListSerializer::loadJSONSet(const std::string& name, std::set<const Block*>& blockSet, Json::Value root) {
	Json::Value blocks = root[name];
	if (!blocks.empty()) {
		if (blocks.isString()) {
			std::string tname = blocks.asString();
			loadBlock(tname, blockSet);
		}
		else if (blocks.isArray()) {
			for (auto iter = blocks.begin(); iter != blocks.end(); ++iter) {
				std::string tname = (*iter).asString();
				loadBlock(tname, blockSet);
			}
		}
	}
}
