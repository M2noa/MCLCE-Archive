/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/

#include "Dungeons.h"

#include "world/level/block/ChorusPlantBlock.h"
#include "world/level/Level.h"
#include "world/level/BlockSource.h"
#include "world/level/Level.h"
#include "world/level/chunk/LevelChunk.h"
#include "world/item/ItemInstance.h"
#include "world/item/Item.h"
#include "world/Facing.h"                        // Directions for checking if can survive
#include "world/level/material/Material.h"

const float ChorusPlantBlock::PLANT_WIDTH = 4.0f / 16.0f;
const float ChorusPlantBlock::PLANT_HEIGHT = 4.0f / 16.0f;
const float ChorusPlantBlock::PLANT_ITEM_DIMENSION = 2.0f / 16.0f;

ChorusPlantBlock::ChorusPlantBlock(const std::string& nameId, int id)
	: Block(nameId, id, Material::getMaterial(MaterialType::Wood)) {
	setSolid(false);

	mProperties = mProperties | BlockProperty::BreakOnPush;
}

void ChorusPlantBlock::tick(BlockSource& region, const BlockPos& pos, Random& random) const {
	if (!canSurvive(region, pos)) {
		//region.getLevel().destroyBlock(region, pos, true);
	}
}

int ChorusPlantBlock::getResource(Random& random, int data, int bonusLootLevel) const {
	return Item::mChorusFruit->getId();
}

int ChorusPlantBlock::getResourceCount(Random& random, int data, int bonusLootLevel) const {
	return random.nextInt(2);
}

bool ChorusPlantBlock::mayPlace(BlockSource& region, const BlockPos& pos) const {
	if (Block::mayPlace(region, pos)) {
		return canSurvive(region, pos);
	}

	return false;
}

bool ChorusPlantBlock::isWaterBlocking() const {
	return false;
}

void ChorusPlantBlock::neighborChanged(BlockSource& region, const BlockPos& pos, const BlockPos& neighborPos) const {
	// Only tick the after a neighbor has changed and cannot survive
	// TODO
// 	if (!canSurvive(region, pos)) {
// 		auto chunk = region.getChunk(pos);
// 		if (chunk) {
// 			chunk->getTickQueue().add(region, pos, getId(), 1);
// 		}
// 	}
}

bool ChorusPlantBlock::canSurvive(BlockSource& region, const BlockPos& pos) const {
	bool emptyAbove = region.isEmptyBlock(pos.above());
	bool emptyBelow = region.isEmptyBlock(pos.below());

	for (auto& neighborID : Facing::Plane::HORIZONTAL) {
		BlockPos neighborPos = pos.neighbor(neighborID);
		Block neighbor = region.getBlock(neighborPos);

		if (neighbor.canGrowChorus()) {
			// May only have neighbors if blocks above OR below are empty
			if (!emptyAbove && !emptyBelow) {
				return false;
			}
			// Standing on, or direct neighbor with plant or endstone below
			Block belowNeighbor = region.getBlock(neighborPos.below());
			if (belowNeighbor.canGrowChorus()) {
				return true;
			}
		}
	}

	Block below = region.getBlock(pos.below());
	return below.canGrowChorus();
}

bool ChorusPlantBlock::checkIsPathable(Entity& entity, const BlockPos& lastPathPos, const BlockPos& pathPos) const {
	return false;
}

// Check if the neighbor block can be connected to
bool ChorusPlantBlock::connectsTo(BlockSource& region, const BlockPos& thisPos, const BlockPos& otherPos) const {
	const Block& block = region.getBlock(otherPos);

	return block.canGrowChorus() || block.isType(Block::mChorusFlowerBlock);
}

bool ChorusPlantBlock::canBeSilkTouched() const {
	return false;
}

bool ChorusPlantBlock::canProvideSupport(BlockSource& region, const BlockPos& pos, FacingID face, BlockSupportType type) const {
	return false;
}

