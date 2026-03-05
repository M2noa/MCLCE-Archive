/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#include "world/level/block/CocoaBlock.h"
#include "world/level/BlockSource.h"
#include "world/level/Level.h"
#include "world/level/block/LogBlock.h"
#include "world/level/block/OldLogBlock.h"
// #include "world/item/DyePowderItem.h"
#include "world/Direction.h"
#include "world/item/ItemInstance.h"
#include "world/phys/AABB.h"
#include "world/level/material/Material.h"
#include "world/Facing.h"

CocoaBlock::CocoaBlock(const std::string& nameId, int id) :
	Block(nameId, id, Material::getMaterial(MaterialType::Plant)) {
	setSolid(false);
	setTicking(true);
	mRenderLayer = RENDERLAYER_OPTIONAL_ALPHATEST;
	mTranslucency[id] = std::max(0.8f, mMaterial.getTranslucency());
}

void CocoaBlock::tick(BlockSource& region, const BlockPos& pos, Random& random) const{
	if(!canSurvive(region, pos)) {
		spawnResources(region, pos, region.getData(pos), 0);
		region.setBlockAndData(pos, FullBlock(), UPDATE_ALL);
	} else if(region.getLevel().getRandom().nextInt(5) == 0) {
		DataID data = region.getData(pos);
		int age = getAge(data);
		if(age < 2) {
			getBlockState(BlockState::Age).set(data, age + 1);
			region.setBlockAndData(pos, mID, data, Block::UPDATE_ALL);
		}
	}
}

bool CocoaBlock::canSurvive(BlockSource& region, const BlockPos& pos) const {
	int dir = getBlockState(BlockState::Direction).get<int>(region.getData(pos));

	BlockPos offsetPos(pos);
	offsetPos.x += Direction::STEP_X[dir];
	offsetPos.z += Direction::STEP_Z[dir];

	auto attached = region.getBlockAndData(offsetPos);

	return attached.id == Block::mLog->mID && LogBlock::getType(attached.data) == enum_cast(OldLogBlock::LogType::Jungle);
}

const AABB& CocoaBlock::getVisualShape(DataID data, AABB& bufferAABB, bool isClipping) const {
	int dir = getBlockState(BlockState::Direction).get<int>(data);
	int age = getAge(data);

	int width = 4 + age * 2;
	int height = 5 + age * 2;

	float hWidth = width / 2.0f;

	switch(dir) {
	case Direction::SOUTH:
		bufferAABB.set((8.0f - hWidth) / 16.0f, (12.0f - height) / 16.0f, (15.0f - width) / 16.0f, (8.0f + hWidth) / 16.0f, (12.0f) / 16.0f, (15.0f) / 16.0f);
		break;
	case Direction::NORTH:
		bufferAABB.set((8.0f - hWidth) / 16.0f, (12.0f - height) / 16.0f, (1.0f) / 16.0f, (8.0f + hWidth) / 16.0f, (12.0f) / 16.0f, (1.0f + width) / 16.0f);
		break;
	case Direction::WEST:
		bufferAABB.set((1.0f) / 16.0f, (12.0f - height) / 16.0f, (8.0f - hWidth) / 16.0f, (1.0f + width) / 16.0f, (12.0f) / 16.0f, (8.0f + hWidth) / 16.0f);
		break;
	case Direction::EAST:
		bufferAABB.set((15.0f - width) / 16.0f, (12.0f - height) / 16.0f, (8.0f - hWidth) / 16.0f, (15.0f) / 16.0f, (12.0f) / 16.0f, (8.0f + hWidth) / 16.0f);
		break;
	}

	return bufferAABB;
}

int CocoaBlock::getPlacementDataValue(Entity& by, const BlockPos& pos, FacingID face, const Vec3& clickPos, int itemValue) const {
	if(face == Facing::UP || face == Facing::DOWN) {
		face = Facing::NORTH;
	}

	return Direction::DIRECTION_OPPOSITE[Direction::FACING_DIRECTION[face]];
}

void CocoaBlock::neighborChanged(BlockSource& region, const BlockPos& pos, const BlockPos& neighborPos) const{
	if(!canSurvive(region, pos)) {
		spawnResources(region, pos, region.getData(pos), 0);
		region.setBlockAndData(pos, FullBlock(), UPDATE_ALL);
	}
}

int CocoaBlock::getAge(int data) {
	return Block::mCocoa->getBlockState(BlockState::Age).get<int>(data);
}

void CocoaBlock::spawnResources(BlockSource& region, const BlockPos& pos, int data, float odds, int bonusLootLevel) const {
	int age = getAge(data);
	int count = 1;
	if(age >= 2) {
		count = 3;
	}

// 	for (int i = 0; i < count; i++) {
// 		popResource(region, pos, ItemInstance(Item::mDye_powder, 1, DyePowderItem::BROWN));
// 	}
}

ItemInstance CocoaBlock::asItemInstance(BlockSource& region, const BlockPos& pos, int blockData) const {
// 	return ItemInstance(Item::mDye_powder, 1, DyePowderItem::BROWN);
	return ItemInstance();
}

bool CocoaBlock::onFertilized(BlockSource& region, const BlockPos& pos, Entity* entity) const {
	DataID data = region.getData(pos);
	int age = getAge(data);
	if(age >= 2) {
		return false;
	}

	if(!region.getLevel().isClientSide()) {
		getBlockState(BlockState::Age).set(data, age + 1);
		region.setBlockAndData(pos, mID, data, Block::UPDATE_CLIENTS);
	}

	return true;
}

bool CocoaBlock::canBeSilkTouched() const {
	return false;
}
