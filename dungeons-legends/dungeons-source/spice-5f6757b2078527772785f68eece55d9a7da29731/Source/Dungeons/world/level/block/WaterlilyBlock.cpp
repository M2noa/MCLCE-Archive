/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#include "world/level/block/WaterlilyBlock.h"
#include "world/level/BlockSource.h"
#include "world/level/Level.h"
#include "world/level/material/Material.h"
#include "world/entity/Entity.h"
#include "world/level/LevelConstants.h"

WaterlilyBlock::WaterlilyBlock(const std::string& nameId, int id)
	: BushBlock(nameId, id, Material::getMaterial(MaterialType::Plant)) {
	//TODO unused => float ss = 0.5f; is this an error?
	float hh = 0.25f / 16.0f;
	setVisualShape(Vec3::ZERO, Vec3(1, hh, 1));
	mTranslucency[id] = std::max(0.8f, mMaterial.getTranslucency());
}

int WaterlilyBlock::getColor(int auxData) const {
	return GUI_COLOR;
}

int WaterlilyBlock::getColor(BlockSource& region, const BlockPos& pos) const {
	return WORLD_COLOR;
}

bool WaterlilyBlock::mayPlaceOn(const Block& block) const {
	return block.isType(Block::mStillWater);
}

bool WaterlilyBlock::canSurvive(BlockSource& region, const BlockPos& pos) const {
	if(pos.y < 0 || pos.y >= region.getMaxHeight()) {
		return false;
	}

	auto below = region.getBlockAndData(pos.below());

	const Block* belowBlock = Block::mBlocks[below.id];
	if(belowBlock && belowBlock->getMaterial().isType(MaterialType::Water)) {
		int waterDepth = belowBlock->getBlockState(BlockState::LiquidDepth).get<int>(below.data);
		return waterDepth == 0;	/*source block*/
	}

	return false;
}

const AABB& WaterlilyBlock::getAABB(BlockSource& region, const BlockPos& pos, AABB& bufferValue, int optionalData, bool isClipping, int clipData) const {
	return Block::getAABB(region, pos, bufferValue);
}

void WaterlilyBlock::entityInside(BlockSource& region, const BlockPos& pos, Entity& entity) const {
	//pop if hit by a boat
	
// 	if (entity.hasCategory(EntityCategory::BoatRideable)) {
// 		entity.mPosDelta *= 0.5f;
// 
// 		spawnResources(region, pos, 0, 1.0f);
// 		region.setBlockAndData(pos, FullBlock::AIR, Block::UPDATE_ALL);
// 	}
}
