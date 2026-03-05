/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#include "world/level/block/OreBlock.h"
#include "world/level/material/Material.h"
// #include "world/item/DyePowderItem.h"

OreBlock::OreBlock(const std::string& nameId, int id)
	: Block(nameId, id, Material::getMaterial(MaterialType::Stone)) {
}

int OreBlock::getResource(Random& random, int data, int bonusLootLevel) const {
	if (isType(Block::mCoalOre)) {
		return Item::mCoal->getId();
	} else if (isType(Block::mDiamondOre)) {
		return Item::mDiamond->getId();
	} else if (isType(Block::mLapisOre)) {
		return Item::mDye_powder->getId();
	} else if (isType(Block::mEmeraldOre)) {
		return Item::mEmerald->getId();
	} else if (isType(Block::mQuartzOre)) {
		return Item::mNetherQuartz->getId();
	} else {
		return mID;
	}
}

int OreBlock::getResourceCount(Random& random, int data, int bonusLootLevel) const {
	int count = 1;

	/* bonusLootLevels
	   Level 1 = 33% chance to multiply drops by 2
	   Level 2 = 25% chance each to multiply drops by 2 or 3
	   Level 3 = 20% chance each to multiply drops by 2, 3, or 4 */
	if (bonusLootLevel > 0 && getResource(random, data, bonusLootLevel) != mID) {
		// assuming normal enchant levels, returns in a range [0, 3], with 0 having greater odds of being rolled than other results
		int bonus = std::max(0, random.nextInt(bonusLootLevel + 2) - 1);
		count = getResourceCount(random, data) * (bonus + 1);
	} else if (mID == Block::mLapisOre->mID) {
		count = 4 + random.nextInt(5);
	}

	return count;
}

int OreBlock::getExperienceDrop(Random& random) const {
	if (isType(Block::mCoalOre)) {
		return random.nextInt(0, 2);
	} else if (isType(Block::mDiamondOre)) {
		return random.nextInt(3, 7);
	} else if (isType(Block::mLapisOre)) {
		return random.nextInt(2, 5);
	} else if (isType(Block::mQuartzOre)) {
		return random.nextInt(2, 5);
	} else if (isType(Block::mEmeraldOre)) {
		return random.nextInt(3, 7);
	} else {
		return 0;
	}
}

DataID OreBlock::getSpawnResourcesAuxValue(DataID data) const {
	// lapis spawns blue dye
// 	if (isType(Block::mLapisOre)) {
// 		return DyePowderItem::BLUE;
// 	}

	return 0;
}
