/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#include "world/level/block/IceBlock.h"
#include "world/level/BlockSource.h"
#include "world/level/material/Material.h"
#include "world/level/LightLayer.h"
#include "world/level/dimension/Dimension.h"
#include "world/entity/player/Player.h"

IceBlock::IceBlock(const std::string& nameId, int id, bool packed)
	: Block(nameId, id, Material::getMaterial(packed ? MaterialType::PackedIce : MaterialType::Ice)) {
	mFriction = 0.98f;
	setTicking(!packed);
	setSolid(packed);
	setPushesOutItems(true);
}

bool IceBlock::playerWillDestroy(Player& player, const BlockPos& pos, int data) const {
	// If no silk touch, destroy and ensure that water is created (if previous conditions are met); otherwise, do what is expected in base class
	ItemInstance* item = player.getSelectedItem();
	
// 	if (item == nullptr || !EnchantUtils::hasEnchant(Enchant::Type::MINING_SILKTOUCH, *item)) {
	if (item == nullptr) {
		BlockSource& region = player.getRegion();

		if (region.getDimensionConst().isUltraWarm()) {
			region.removeBlock(pos);
			return true;
		}

		const Material& below = region.getMaterial(pos.below());
		if (below.getBlocksMotion() || below.isLiquid()) {
			player.getRegion().setBlockAndData(pos, Block::mFlowingWater->mID, Block::UPDATE_ALL);
			return true;
		}
	}

	return Block::playerWillDestroy(player, pos, data);
}


int IceBlock::getResourceCount(Random& random, int data, int bonusLootLevel) const {
	return 0;
}

void IceBlock::tick(BlockSource& region, const BlockPos& pos, Random& random) const {
// 	if (region.getBrightness(LightLayer::BLOCK , pos) <= 11 - Block::mLightBlock[mID]) {
// 		return;
// 	}

	if (region.getDimensionConst().isUltraWarm()) {
		region.removeBlock(pos);
		return;
	}

	spawnResources(region, pos, region.getData(pos), 1.0f);
	region.setBlock(pos, Block::mFlowingWater->mID, Block::UPDATE_ALL);
}
