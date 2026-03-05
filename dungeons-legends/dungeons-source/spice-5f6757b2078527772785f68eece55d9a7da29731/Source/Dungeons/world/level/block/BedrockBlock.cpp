/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#include "world/level/block/BedrockBlock.h"
#include "world/item/ItemInstance.h"
#include "world/level/BlockSource.h"
#include "world/level/material/Material.h"

BedrockBlock::BedrockBlock(const std::string& nameId, int id)
	: Block(nameId, id, Material::getMaterial(MaterialType::Stone)) {
}

int BedrockBlock::getResource(Random& random, int data, int bonusLootLevel /*= 0*/) const {
	return Block::mBedrock->mID;
}

ItemInstance BedrockBlock::asItemInstance(BlockSource& region, const BlockPos& pos, int blockData) const {
	return ItemInstance(Block::mBedrock);
}

ItemInstance BedrockBlock::getSilkTouchItemInstance(DataID data) const {
	return ItemInstance(Block::mBedrock);
}

bool BedrockBlock::isInfiniburnBlock(int data) const {
	return data == 1 ? true : Block::isInfiniburnBlock(data);
}
