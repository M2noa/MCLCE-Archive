/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#include "world/level/block/NetherReactor.h"
#include "world/level/Level.h"
// #include "world/level/block/entity/NetherReactorBlockEntity.h"
#include "world/level/BlockSource.h"
#include "world/item/Item.h"
// #include "world/entity/item/ItemEntity.h"
// #include "world/level/Spawner.h"

NetherReactorBlock::NetherReactorBlock(const std::string& nameId, int id, const Material& material)
	: EntityBlock(nameId, id, material) {
	mBlockEntityType = BlockEntityType::NetherReactor;
}

void NetherReactorBlock::spawnResources(BlockSource& region, const BlockPos& pos, int data, float odds, int bonusLootLevel) const {
	if (region.getLevel().isClientSide()) {
		return;
	}

	for (int i = 0; i < 6; i++) {
		_dropItem(region, pos, Item::mIronIngot);
	}

	for (int i = 0; i < 3; i++) {
		_dropItem(region, pos, Item::mDiamond);
	}
}

void NetherReactorBlock::_dropItem(BlockSource& region, const BlockPos& pos, Item* item) const {
// 	auto& level = region.getLevel();
// 
// 	static const float s = 0.7f;
// 	const float xo = level.getRandom().nextFloat() * s + (1 - s) * 0.5f;
// 	const float yo = level.getRandom().nextFloat() * s + (1 - s) * 0.5f;
// 	const float zo = level.getRandom().nextFloat() * s + (1 - s) * 0.5f;
// 	level.getSpawner().spawnItem(region, ItemInstance(item, 1), nullptr, Vec3(pos.x + xo, pos.y + yo, pos.z + zo), 10);
}
