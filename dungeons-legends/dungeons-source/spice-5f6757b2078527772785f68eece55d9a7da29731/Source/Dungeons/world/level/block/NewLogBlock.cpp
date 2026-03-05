/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#include "world/level/block/NewLogBlock.h"
#include "world/level/material/Material.h"
#include "world/item/ItemInstance.h"
#include "world/level/Level.h"
#include "world/Facing.h"
#include "locale/I18n.h"

NewLogBlock::NewLogBlock(const std::string& nameId, int id)
	: LogBlock(nameId, id) {
	
}

int NewLogBlock::getResource(Random& random, int data, int bonusLootLevel) const {
	return Block::mLog2->mID;
}

std::string NewLogBlock::buildDescriptionName(DataID data) const {
	int mappedType = getBlockState(BlockState::MappedType).get<int>(data);
	if (mappedType < 0 || mappedType >= 2) {
		mappedType = 0;
	}

	static const std::array<std::string, 2> WOOD_NAMES = {
		"acacia", "big_oak"
	};

	return I18n::get(Block::BLOCK_DESCRIPTION_PREFIX + "log." + WOOD_NAMES[mappedType] + ".name");
}
