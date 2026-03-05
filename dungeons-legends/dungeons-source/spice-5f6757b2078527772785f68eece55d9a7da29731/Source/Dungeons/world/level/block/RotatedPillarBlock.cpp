/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#include "world/level/block/RotatedPillarBlock.h"
#include "world/Facing.h"
#include "world/item/ItemInstance.h"

// Down, Up, North, South, West
const int RotatedPillarBlock::mRotatedX[6] = {
	Facing::EAST, Facing::WEST, Facing::NORTH, Facing::SOUTH, Facing::DOWN, Facing::UP
};

const int RotatedPillarBlock::mRotatedZ[6] = {
	Facing::SOUTH, Facing::NORTH, Facing::DOWN, Facing::UP, Facing::WEST, Facing::EAST
};

RotatedPillarBlock::RotatedPillarBlock(const std::string& nameId, int id, const Material& material) :
	Block(nameId, id, material){
	mProperties = mProperties | BlockProperty::SolidBlocking;
}

ItemInstance RotatedPillarBlock::asItemInstance(BlockSource& region, const BlockPos& pos, int blockData) const {
	int type = getBlockState(BlockState::MappedType).get<int>(blockData);

	DataID data = 0;
	getBlockState(BlockState::MappedType).set(data, type);

	return ItemInstance(mID, 1, data);
}

int RotatedPillarBlock::getVariant(int data) const {
	return getType(data);
}

FacingID RotatedPillarBlock::getMappedFace(FacingID face, int data) const {
	int dir = getBlockState(BlockState::Direction).get<int>(data);
	if (dir == FACING_Y && (face == Facing::UP || face == Facing::DOWN)) {
		return Facing::UP;
	}
	else if (dir == FACING_X && (face == Facing::EAST || face == Facing::WEST)) {
		return Facing::UP;
	}
	else if (dir == FACING_Z && (face == Facing::NORTH || face == Facing::SOUTH)) {
		return Facing::UP;
	}
	return Facing::EAST;
}

int RotatedPillarBlock::getPlacementDataValue(Entity& by, const BlockPos& pos, FacingID face, const Vec3& clickPos, int itemValue) const {
	int facing = 0;

	switch (face) {
	case Facing::NORTH:
	case Facing::SOUTH:
		facing = FACING_Z;
		break;
	case Facing::EAST:
	case Facing::WEST:
		facing = FACING_X;
		break;
	case Facing::UP:
	case Facing::DOWN:
		facing = FACING_Y;
		break;
	}

	DataID data = itemValue;
	getBlockState(BlockState::Direction).set(data, facing);

	return data;
}

int RotatedPillarBlock::getType(int data) {
	return Block::mQuartzBlock->getBlockState(BlockState::MappedType).get<int>(data);
}
