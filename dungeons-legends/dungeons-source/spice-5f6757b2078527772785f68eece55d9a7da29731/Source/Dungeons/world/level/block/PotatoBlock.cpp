/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#include "world/level/block/PotatoBlock.h"
#include "world/level/BlockSource.h"
#include "world/level/Level.h"
#include "world/item/Item.h"
#include "world/item/ItemInstance.h"

PotatoBlock::PotatoBlock(const std::string& nameId, int id)
	: CropBlock(nameId, id) {
	mProperties = BlockProperty::BreakOnPush;
}

Item* PotatoBlock::getBaseSeed() const {
	return Item::mPotato;
}

int PotatoBlock::getBasePlantId() const {
	return Item::mPotato->getId();
}

int PotatoBlock::getVariant(int data) const {
	auto growth = getBlockState(BlockState::Growth).get<int>(data);
	if (growth < MAX_GROWTH) {
		if (data == 6) {
			data = 5;
		}
		return data >> 1;
	}
	else {
		return 3;
	}
}

void PotatoBlock::spawnResources(BlockSource& region, const BlockPos& pos, int data, float odds, int bonusLootLevel) const {
	CropBlock::spawnResources(region, pos, data, odds, bonusLootLevel);
	// No poisonous things yet
	Level& level = region.getLevel();

	//if (level.isClientSide()) {
	//	return;
	//}

	//auto growth = getBlockState(BlockState::Growth).get<int>(data);
	//if (growth >= MAX_GROWTH) {
	//	if (level.getRandom().nextInt(50) == 0) {
	//		popResource(region, pos, ItemInstance(Item::mPoisonous_potato));
	//	}
	//}
}

int PotatoBlock::getResourceCount(Random& random, int data, int bonusLootLevel) const {
	return 1 + random.nextInt(bonusLootLevel + 1);
}
