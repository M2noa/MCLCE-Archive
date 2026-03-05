/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#include "world/level/block/LadderBlock.h"
#include "world/level/material/Material.h"
#include "world/level/BlockSource.h"
#include "world/level/dimension/Dimension.h"
//#include "world/redstone/circuit/CircuitSystem.h"
//#include "world/redstone/circuit/components/ProducerComponent.h"
#include "world/item/ItemInstance.h"
#include "world/entity/Mob.h"
#include "world/Facing.h"

LadderBlock::LadderBlock(const std::string& nameId, int id) :
	Block(nameId, id, Material::getMaterial(MaterialType::Decoration)) {
	setSolid(false);
	mProperties = BlockProperty::BreakOnPush;
	mRenderLayer = RENDERLAYER_ALPHATEST;
	mTranslucency[id] = std::max(0.8f, mMaterial.getTranslucency());
}

const AABB& LadderBlock::getAABB(BlockSource& region, const BlockPos& pos, AABB& bufferValue, int optionalData, bool isClipping, int clipData) const {
	int dir = optionalData != 0 ? optionalData : region.getData(pos);

	getVisualShape(dir, bufferValue, isClipping);

	return bufferValue.move(Vec3(pos));
}

const AABB& LadderBlock::getVisualShape(DataID data, AABB& bufferAABB, bool isClipping /*= false*/) const {
	float r = (3.0f) / 16.0f;

	switch(data) {
	case Facing::NORTH:
		bufferAABB.set(Vec3(0, 0, 1 - r), Vec3::ONE);
		break;
	case Facing::SOUTH:
		bufferAABB.set(Vec3::ZERO, Vec3(1, 1, r));
		break;
	case Facing::WEST:
		bufferAABB.set(Vec3(1 - r, 0, 0), Vec3::ONE);
		break;
	case Facing::EAST:
		bufferAABB.set(Vec3::ZERO, Vec3(r, 1, 1));
		break;
	}

	return bufferAABB;
}

const AABB& LadderBlock::getVisualShape(BlockSource& region, const BlockPos& pos, AABB& bufferAABB, bool isClipping /*= false*/) const {
	return getVisualShape(region.getData(pos), bufferAABB, isClipping);
}

bool LadderBlock::mayPlace(BlockSource& region, const BlockPos& pos, FacingID face) const {
	
	for (FacingID facing = Facing::NORTH; facing < Facing::MAX; ++facing) {
		BlockPos relative = pos.neighbor(facing);
		auto& block = region.getBlock(relative);
	
		bool isSolidBlocking = block.getMaterial().isSolidBlocking();
		bool isCube = block.hasProperty(BlockProperty::CubeShaped) || block.hasProperty(BlockProperty::SolidBlocking);

		if (isSolidBlocking && isCube)
			return true;
	}

	return false;
}

int LadderBlock::getPlacementDataValue(Entity& by, const BlockPos& pos, FacingID face, const Vec3& clickPos, int itemValue) const {
	auto& region = by.getRegion();
	DataID data = 0;

	if(face == Facing::NORTH && region.isSolidBlockingBlock(pos.south())) {
		getBlockState(BlockState::FacingDirection).set(data, Facing::NORTH);
	}
	if(face == Facing::SOUTH && region.isSolidBlockingBlock(pos.north())) {
		getBlockState(BlockState::FacingDirection).set(data, Facing::SOUTH);
	}
	if(face == Facing::WEST && region.isSolidBlockingBlock(pos.east())) {
		getBlockState(BlockState::FacingDirection).set(data, Facing::WEST);
	}
	if(face == Facing::EAST && region.isSolidBlockingBlock(pos.west())) {
		getBlockState(BlockState::FacingDirection).set(data, Facing::EAST);
	}

	return data;
}

void LadderBlock::neighborChanged(BlockSource& region, const BlockPos& pos, const BlockPos& neighborPos) const {
	FacingID face = getBlockState(BlockState::FacingDirection).get<FacingID>(region.getData(pos));
	bool ok = false;

	if(face == Facing::NORTH && region.isSolidBlockingBlock(pos.south())) {
		ok = true;
	}
	if(face == Facing::SOUTH && region.isSolidBlockingBlock(pos.north())) {
		ok = true;
	}
	if(face == Facing::WEST && region.isSolidBlockingBlock(pos.east())) {
		ok = true;
	}
	if(face == Facing::EAST && region.isSolidBlockingBlock(pos.west())) {
		ok = true;
	}
	if(!ok) {
		//spawnResources(level, x, y, z, face); //@crafting
		popResource(region, pos, ItemInstance(Block::mLadder));
		region.setBlock(pos, BlockID::AIR, Block::UPDATE_ALL);
	}

	Block::neighborChanged(region, pos, neighborPos);
}

int LadderBlock::getResourceCount(Random& random, int data, int bonusLootLevel) const {
	return 1;
}

bool LadderBlock::canBeSilkTouched() const {
	return false;
}
