/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#include "world/level/block/BeetrootBlock.h"
#include "world/level/BlockSource.h"
#include "world/level/Level.h"
// #include "world/entity/item/ItemEntity.h"
#include "world/item/Item.h"
// #include "world/level/Spawner.h"

BeetrootBlock::BeetrootBlock(const std::string& nameId, int id)
	: CropBlock(nameId, id) {
	mProperties = BlockProperty::BreakOnPush;
}

Item* BeetrootBlock::getBaseSeed() const {
	return Item::mSeeds_beetroot;
}

int BeetrootBlock::getBasePlantId() const {
	return Item::mBeetroot->getId();
}

int BeetrootBlock::getVariant(int data) const {
	auto growth = getBlockState(BlockState::Growth).get<int>(data);
	if (growth < MAX_GROWTH) {
		if (growth == 6) {
			growth = 5;
		}
		return growth >> 1;
	}
	else {
		return 3;
	}
}

void BeetrootBlock::spawnResources(BlockSource& region, const BlockPos& pos, int data, float odds, int bonusLootLevel) const {
	auto& level = region.getLevel();
	if (level.isClientSide()) {
		return;
	}

	auto growth = getBlockState(BlockState::Growth).get<int>(data);
	if (growth < 7) {
// 		const float s = 0.7f;
// 		const float xo = level.getRandom().nextFloat() * s + (1 - s) * 0.5f;
// 		const float yo = level.getRandom().nextFloat() * s + (1 - s) * 0.5f;
// 		const float zo = level.getRandom().nextFloat() * s + (1 - s) * 0.5f;
// 		region.getLevel().getSpawner().spawnItem(region, ItemInstance(getBaseSeed()), nullptr, Vec3(pos.x + xo, pos.y + yo, pos.z + zo), 10);
	} else {
		int rootCount = level.getRandom().nextInt(2) + 1;
		int count = 3;

		for (int i = 0; i < count; i++) {
			if(level.getRandom().nextInt(5 * 3) > growth) {
				continue;
			}

// 			const float s = 0.7f;
// 			const float xo = level.getRandom().nextFloat() * s + (1 - s) * 0.5f;
// 			const float yo = level.getRandom().nextFloat() * s + (1 - s) * 0.5f;
// 			const float zo = level.getRandom().nextFloat() * s + (1 - s) * 0.5f;
// 			region.getLevel().getSpawner().spawnItem(region, ItemInstance(getBaseSeed()), nullptr, Vec3(pos.x + xo, pos.y + yo, pos.z + zo), 10);
			
		}

// 		for (int i = 0; i < rootCount; i++) {
// 			const float s = 0.7f;
// 			const float xo = level.getRandom().nextFloat() * s + (1 - s) * 0.5f;
// 			const float yo = level.getRandom().nextFloat() * s + (1 - s) * 0.5f;
// 			const float zo = level.getRandom().nextFloat() * s + (1 - s) * 0.5f;
// 			
// 			region.getLevel().getSpawner().spawnItem(region, ItemInstance(Item::mBeetroot), nullptr, Vec3(pos.x + xo, pos.y + yo, pos.z + zo), 10);
// 
// 		}
	}
}
