/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#include "world/level/block/DeadBush.h"
#include "world/level/material/Material.h"
#include "world/item/Item.h"
#include "world/entity/player/Player.h"
#include "world/level/Level.h"

DeadBush::DeadBush(const std::string& nameId, int id)
	: BushBlock(nameId, id, Material::getMaterial(MaterialType::ReplaceablePlant)) {
	mRenderLayer = RENDERLAYER_ALPHATEST;
}

bool DeadBush::mayPlaceOn(const Block& block) const {
	return block.isType(Block::mSand) || block.isType(Block::mHardenedClay) || block.isType(Block::mStainedClay) || block.isType(Block::mPodzol);
}

int DeadBush::getResource(Random& random, int data, int bonusLootLevel) const {
	return Item::mStick->getId();
}

int DeadBush::getResourceCount(Random& random, int data, int bonusLootLevel) const {
	return random.nextInt(3);
}

void DeadBush::playerDestroy(Player* player, const BlockPos& pos, int data) const {
// TODO: rherlitz
// 	if(!player->getLevel().isClientSide() && player->getSelectedItem() != nullptr && player->getSelectedItem()->getId() == Item::mShears->getId()) {
// 		// drop leaf block instead of sapling
// 		popResource(player->getRegion(), pos, ItemInstance(Block::mDeadBush, 1, data));
// 	} else {
		BushBlock::playerDestroy(player, pos, data);
// 	}
}
