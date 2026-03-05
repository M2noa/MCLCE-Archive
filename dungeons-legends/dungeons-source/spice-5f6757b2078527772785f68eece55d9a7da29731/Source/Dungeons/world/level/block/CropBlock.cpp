/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#include "world/level/block/CropBlock.h"
#include "world/level/BlockSource.h"
#include "world/entity/player/Player.h"
#include "world/level/Level.h"
#include "world/item/ItemInstance.h"
#include "world/item/Item.h"

CropBlock::CropBlock(const std::string& nameId, int id) :
	BushBlock(nameId, id, Material::getMaterial(MaterialType::Plant)) {
	setTicking(true);
	float ss = 0.5f;
	setVisualShape(Vec3(0.5f - ss, 0, 0.5f - ss), Vec3(0.5f + ss, 0.25f, 0.5f + ss));
	
	mRenderLayer = RENDERLAYER_ALPHATEST;
}

void CropBlock::tick(BlockSource& region, const BlockPos& pos, Random& random) const {
	BushBlock::tick(region, pos, random);
	if (region.getRawBrightness(pos) >= Brightness::MAX - 6) {
		auto growth = getBlockState(BlockState::Growth).get<int>(region.getData(pos));
		if (growth < MAX_GROWTH) {
			float growthSpeed = getGrowthSpeed(region, pos);
			if (random.nextInt(int(25 / growthSpeed)) == 0) {
				growth++;
				region.setBlockAndData(pos.x, pos.y, pos.z, mID, growth, Block::UPDATE_ALL);
			}
		}
	}
}

bool CropBlock::canSurvive(BlockSource& region, const BlockPos& pos) const {
	return (region.getRawBrightness(pos) >= 8 || region.canSeeSky(pos)) && mayPlaceOn(region.getBlock(pos.below()));
}

int CropBlock::getVariant(int data) const {
	return std::min(MAX_GROWTH, (DataID)data);
}

void CropBlock::spawnResources(BlockSource& region, const BlockPos& pos, int data, float odds, int bonusLootLevel) const {
	BushBlock::spawnResources(region, pos, data, odds, 0);

	auto& level = region.getLevel();
	if (level.isClientSide()) {
		return;
	}

	auto growth = getBlockState(BlockState::Growth).get<int>(region.getData(pos));
	if (growth >= MAX_GROWTH) {
		int count = 3 + bonusLootLevel;

		for (int i = 0; i < count; i++) {
			if (level.getRandom().nextInt(5 * 3) > growth) {
				continue;
			}

			popResource(region, pos, ItemInstance(getBaseSeed(), 1, 0));
		}
	}
}

int CropBlock::getResource(Random& random, int data, int bonusLootLevel) const {
	int growth = getBlockState(BlockState::Growth).get<int>(data);
	if (growth == MAX_GROWTH) {
		return getBasePlantId();
	}

	return getBaseSeed()->getId();
}

int CropBlock::getResourceCount( Random& random, int data, int bonusLootLevel) const {
	return 1;
}

ItemInstance CropBlock::asItemInstance(BlockSource& region, const BlockPos& pos, int blockData) const {
	return ItemInstance(getBaseSeed());
}

bool CropBlock::mayPlaceOn(const Block& block) const {
	return block.isType(Block::mFarmland);
}

Item* CropBlock::getBaseSeed() const {
	return Item::mSeeds_wheat;
}

int CropBlock::getBasePlantId() const {
	return Item::mWheat->getId();
}

const AABB& CropBlock::getAABB(BlockSource& region, const BlockPos& pos, AABB& bufferValue, int optionalData, bool isClipping, int clipData) const {
	if (isClipping) {
		int growth = std::min(MAX_GROWTH, getBlockState(BlockState::Growth).get<DataID>(region.getData(pos)));
		return bufferValue.set(Vec3::ZERO, Vec3(1.f, (1.f / 7.f) * (growth + 1), 1.f)).move(Vec3(pos));
	} else{
		return AABB::EMPTY;
	}
}

bool CropBlock::isCropBlock() const {
	return true;
}

float CropBlock::getGrowthSpeed(BlockSource& region, const BlockPos& pos) const {
	float speed = 1;

	BlockID n = region.getBlockID(pos.north());
	BlockID s = region.getBlockID(pos.south());
	BlockID w = region.getBlockID(pos.west());
	BlockID e = region.getBlockID(pos.east());
	BlockID d0 = region.getBlockID(pos.offset(-1, 0, -1));
	BlockID d1 = region.getBlockID(pos.offset(1, 0, -1));
	BlockID d2 = region.getBlockID(pos.offset(1, 0, 1));
	BlockID d3 = region.getBlockID(pos.offset(-1, 0, 1));

	bool horizontal = w == mID || e == mID;
	bool vertical = n == mID || s == mID;
	bool diagonal = d0 == mID || d1 == mID || d2 == mID || d3 == mID;

	for (int xx = pos.x - 1; xx <= pos.x + 1; xx++) {
		for (int zz = pos.z - 1; zz <= pos.z + 1; zz++) {
			BlockID t = region.getBlockID(xx, pos.y - 1, zz);

			float blockSpeed = 0;
			if (t == Block::mFarmland->mID) {
				blockSpeed = 1;
				DataID data = region.getData(xx, pos.y - 1, zz);
				if (Block::mFarmland->getBlockState(BlockState::MoisturizedAmount).get<int>(data) > 0) {
					blockSpeed = 3;
				}
			}

			if (xx != pos.x || zz != pos.z) {
				blockSpeed /= 4;
			}

			speed += blockSpeed;
		}
	}

	if (diagonal || (horizontal && vertical)) {
		speed /= 2;
	}

	return speed;
}

bool CropBlock::onFertilized(BlockSource& region, const BlockPos& pos, Entity* entity) const {
	if (entity != nullptr && entity->isCreative()) {
		DataID data = 0;
		getBlockState(BlockState::Growth).set(data, MAX_GROWTH);
		region.setBlockAndData(pos.x, pos.y, pos.z, mID, data, Block::UPDATE_ALL);
		return true;
	} else if (getBlockState(BlockState::Growth).get<int>(region.getData(pos)) >= MAX_GROWTH) {
		return false;
	} else {
		growCrops(region, pos);
		return true;
	}
}

bool CropBlock::canBeSilkTouched() const {
	return false;
}
