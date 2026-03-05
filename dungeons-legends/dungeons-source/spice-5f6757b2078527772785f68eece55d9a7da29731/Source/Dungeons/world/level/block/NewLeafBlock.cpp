/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#include "world/level/block/NewLeafBlock.h"
#include "world/level/block/Sapling.h"
#include "world/item/ItemInstance.h"
#include "world/item/Item.h"

NewLeafBlock::NewLeafBlock(const std::string& nameId, int id)
	: LeafBlock(nameId, id) {
}

int NewLeafBlock::getVariant(int data) const {
	int leafType = getLeafType(data);
	if (leafType < 0 || leafType >= NUM_TYPES) {
		leafType = 0;
	}
	int index = mHasTransparentLeaves ? leafType : leafType + NUM_TYPES;
	return index;
}

void NewLeafBlock::dropExtraLoot(BlockSource& region, const BlockPos& pos, int data) const {
	int leafType = getLeafType(data);
	if(leafType == BIG_OAK_LEAF) {
		popResource(region, pos, ItemInstance(Item::mApple, 1, 0));
	}
}

std::string NewLeafBlock::buildDescriptionName(DataID data) const {
	int leafType = getLeafType(data);
	static const std::string LEAF_NAMES[NUM_TYPES] = {
		"acacia", "big_oak"
	};

	if(leafType < 0 || leafType >= NUM_TYPES) {
		leafType = 0;
	}

	return I18n::get(mDescriptionId + "." + LEAF_NAMES[leafType] + ".name");
}

DataID NewLeafBlock::getSpawnResourcesAuxValue(DataID data) const {
	return LeafBlock::getSpawnResourcesAuxValue(data) + Sapling::NEW_LEAF_OFFSET;
}
