/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#include "world/level/block/OldLogBlock.h"
#include "world/level/material/Material.h"
#include "world/item/ItemInstance.h"
#include "world/level/Level.h"
#include "world/Facing.h"
#include "locale/I18n.h"

OldLogBlock::OldLogBlock(const std::string& nameId, int id)
	: LogBlock(nameId, id) {
	
}

int OldLogBlock::getResource(Random& random, int data, int bonusLootLevel) const {
	return Block::mLog->mID;
}

std::string OldLogBlock::buildDescriptionName(DataID data) const {
	int mappedType = getBlockState(BlockState::MappedType).get<int>(data);
	if (mappedType < 0 || mappedType >= 4) {
		mappedType = 0;
	}

	static const std::array<std::string, 4> WOOD_NAMES = {
		"oak", "spruce", "birch", "jungle"
	};

	return I18n::get(Block::BLOCK_DESCRIPTION_PREFIX + "log." + WOOD_NAMES[mappedType] + ".name");
}
