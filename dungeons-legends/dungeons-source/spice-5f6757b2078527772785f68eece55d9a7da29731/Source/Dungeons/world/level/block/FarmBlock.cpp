/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#include "world/entity/Entity.h"
#include "world/entity/EntityClassTree.h"
#include "world/level/block/FarmBlock.h"
#include "world/level/material/Material.h"
#include "world/level/Level.h"
#include "world/level/BlockSource.h"
#include "world/level/dimension/Dimension.h"
#include "world/level/Weather.h"
#include "util/Random.h"
#include "world/item/ItemInstance.h"
#include "world/Facing.h"

FarmBlock::FarmBlock(const std::string& nameId, int id) :
	Block(nameId, id, Material::getMaterial(MaterialType::Dirt)) {
	setTicking(true);
	setVisualShape( Vec3::ZERO, Vec3(1, 15 / 16.0f, 1));
	setLightBlock(Brightness::MAX);

	setSolid(false);
	mProperties = BlockProperty::Unspecified;
}

const AABB& FarmBlock::getAABB(BlockSource& region, const BlockPos& pos, AABB& bufferValue, int optionalData, bool isClipping, int clipData) const {
	bufferValue.set(0, 0, 0, 1, 1, 1);
	return bufferValue.move(Vec3(pos));
}

int FarmBlock::getVariant(int data) const {
	return getBlockState(BlockState::MoisturizedAmount).get<int>(data) > 0 ? 0 : 1;
}

void FarmBlock::tick(BlockSource& region, const BlockPos& pos, Random& random) const {
	DataID data = region.getData(pos);
	if(isNearWater(region, pos) || region.getDimension().getWeather().isRainingAt(region,pos.above())) {
		getBlockState(BlockState::MoisturizedAmount).set(data, 7);
		region.setBlockAndData(pos, mID, data, Block::UPDATE_CLIENTS);
	} else {
		int moisture = getBlockState(BlockState::MoisturizedAmount).get<int>(data);
		if(moisture > 0) {
			getBlockState(BlockState::MoisturizedAmount).set(data, moisture - 1);
			region.setBlockAndData(pos, mID, data, Block::UPDATE_CLIENTS);
		} else if(!isUnderCrops(region, pos)) {
			region.setBlock(pos, Block::mDirt->mID, Block::UPDATE_ALL);
		}
	}
}

void FarmBlock::onFallOn(BlockSource& region, const BlockPos& pos, Entity* entity, float fallDistance) const {
	if (EntityClassTree::isMob(entity->getEntityTypeId())) {  //Non-Mobs shouldn't harm farmland
		if (!region.getLevel().isClientSide() && region.getLevel().getRandom().nextFloat() < (fallDistance - .5f)) {
			region.setBlock(pos, Block::mDirt->mID, Block::UPDATE_ALL);
		}
	}

	Block::onFallOn(region, pos, entity, fallDistance);
}

void FarmBlock::neighborChanged(BlockSource& region, const BlockPos& pos, const BlockPos& neighborPos) const {
	Block::neighborChanged(region, pos, neighborPos);
	const Material& above = region.getMaterial(pos.above());
	if (above.isSolid()) {
		region.setBlock(pos, Block::mDirt->mID, Block::UPDATE_ALL);
	}
}

int FarmBlock::getResource(Random& random, int data, int bonusLootLevel) const {
	return Block::mDirt->getResource(random, 0, bonusLootLevel);
}

ItemInstance FarmBlock::asItemInstance(BlockSource& region, const BlockPos& pos, int blockData) const {
	return ItemInstance(Block::mDirt);
}

bool FarmBlock::canBeSilkTouched() const {
	return false;
}

bool FarmBlock::isUnderCrops(BlockSource& region, const BlockPos& pos) const {
	if (region.getBlock(pos.above()).isType(Block::mWheatCrop) || region.getBlock(pos.above()).isType(Block::mCarrotCrop) || region.getBlock(pos.above()).isType(Block::mPotatoCrop) || region.getBlock(pos.above()).isType(Block::mBeetrootCrop)) {
		return true;
	}

	return false;
}

bool FarmBlock::isNearWater(BlockSource& region, const BlockPos& pos) const {
	for(auto xx : range_incl(pos.x - 4, pos.x + 4)) {
		for(auto yy : range_incl(pos.y, pos.y + 1)) {
			for(auto zz : range_incl(pos.z - 4, pos.z + 4)) {
				if(region.getMaterial(xx, yy, zz).isType(MaterialType::Water)) {
					return true;
				}
			}
		}
	}

	return false;
}
