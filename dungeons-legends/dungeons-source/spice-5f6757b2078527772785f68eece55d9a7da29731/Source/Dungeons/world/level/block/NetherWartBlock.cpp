/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#include "world/level/block/NetherWartBlock.h"
#include "world/level/BlockSource.h"
#include "world/level/Level.h"
#include "world/item/Item.h"
#include "world/item/ItemInstance.h"

const int NetherWartBlock::MAX_AGE = 3;

NetherWartBlock::NetherWartBlock(const std::string& nameId, int id)
	: BushBlock(nameId, id, Material::getMaterial(MaterialType::Plant)) {
	setTicking(true);
	float ss = 0.5f;
	setVisualShape( Vec3(0.5f - ss, 0, 0.5f - ss), Vec3(0.5f + ss, 0.25f, 0.5f + ss));
}

void NetherWartBlock::tick(BlockSource& region, const BlockPos& pos, Random& random) const {
	DataID data = region.getData(pos);
	int age = getBlockState(BlockState::Age).get<int>(data);
	if (age < MAX_AGE) {
		if (random.nextInt(10) == 0) {
			getBlockState(BlockState::Age).set(data, age + 1);
			region.setBlockAndData(pos.x, pos.y, pos.z, mID, data, Block::UPDATE_CLIENTS);
		}
	}

	BushBlock::tick(region, pos, random);
}

int NetherWartBlock::getResource(Random& random, int data, int bonusLootLevel) const {
	return -1;
}

int NetherWartBlock::getResourceCount(Random& random, int data, int bonusLootLevel) const {
	return 0;
}

bool NetherWartBlock::canSurvive(BlockSource& region, const BlockPos& pos) const {
	return mayPlaceOn(region.getBlock(pos.below()));
}

/**
 * Using this method instead of destroy() to determine if seeds should be
 * dropped
 */
void NetherWartBlock::spawnResources(BlockSource& region, const BlockPos& pos, int data, float odds, int bonusLootLevel) const {
	if (region.getLevelConst().isClientSide()) {
		return;
	}

	int age = getBlockState(BlockState::Age).get<int>(data);

	int count = 1;
	if (age >= MAX_AGE) {
		count = 2 + region.getLevel().getRandom().nextInt(3);
	}

	for (int i = 0; i < count; i++) {
		popResource(region, pos, ItemInstance(Item::mNether_wart));
	}
}

void NetherWartBlock::growCropsToMax(BlockSource& region, const BlockPos& pos) {
	DataID data = 0;
	getBlockState(BlockState::Age).set(data, MAX_AGE);
	region.setBlockAndData(pos, mID, data, Block::UPDATE_CLIENTS);
}

bool NetherWartBlock::canBeSilkTouched() const {
	return false;
}

bool NetherWartBlock::mayPlaceOn(const Block& block) const {
	return block.isType(Block::mSoulSand);
}

ItemInstance NetherWartBlock::asItemInstance(BlockSource& region, const BlockPos& pos, int blockData) const {
	return ItemInstance(Item::mNether_wart);
}
