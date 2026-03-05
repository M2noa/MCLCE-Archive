/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#include "world/level/block/SlabBlock.h"
#include "world/level/material/Material.h"
#include "world/level/dimension/Dimension.h"
#include "world/Facing.h"
#include "world/level/BlockSource.h"
#include "world/item/ItemInstance.h"
//#include "world/redstone/circuit/CircuitSystem.h"
//#include "world/redstone/circuit/components/PoweredBlockComponent.h"
#include "world/level/Level.h"

SlabBlock::SlabBlock(const std::string& nameId, int id, bool _fullsize, const Material& mat)
	: Block(nameId, id, mat)
	, fullSize(_fullsize){
	setSolid(fullSize);
	setPushesOutItems(true);

	if (!fullSize) {
		mProperties = BlockProperty::HalfSlab;
	}
}

const AABB& SlabBlock::getVisualShape(DataID data, AABB& bufferAABB, bool isClipping	/* = false*/) const {
	if(fullSize) {
		bufferAABB.set(0, 0, 0, 1, 1, 1);
	} else {
		bool upper = !isBottomSlab(data);
		if(upper) {
			bufferAABB.set(0, 0.5f, 0, 1, 1, 1);
		} else{
			bufferAABB.set(0, 0, 0, 1, 0.5f, 1);
		}
	}

	return bufferAABB;
}

int SlabBlock::getPlacementDataValue(Entity& by, const BlockPos& pos, FacingID face, const Vec3& clickPos, int itemValue) const{
	if (fullSize) {
		return itemValue;
	}

	if (face == Facing::DOWN || (face != Facing::UP && clickPos.y > 0.5f)) {
		DataID data = itemValue;
		getBlockState(BlockState::TopSlotBit).set(data, true);
		return data;
	}

	return itemValue;
}

int SlabBlock::getResourceCount(Random& random, int data, int bonusLootLevel) const {
	if (fullSize) {
		return 2;
	}

	return 1;
}

static bool isHalfSlab(int blockId) {
	return blockId == Block::mWoodenSlab->mID || blockId == Block::mStoneSlab->mID || blockId == Block::mStoneSlab2->mID;
}

bool SlabBlock::isBottomSlab(BlockSource& region, const BlockPos& pos) const {
	return isBottomSlab(region.getData(pos));
}

bool SlabBlock::isBottomSlab(int data) {
	return !Block::mStoneSlab->getBlockState(BlockState::TopSlotBit).getBool(data);
}

bool SlabBlock::shouldRenderFace(BlockSource& region, const BlockPos& pos, FacingID face, const AABB& shape) const {
	if (fullSize) {
		return Block::shouldRenderFace(region, pos, face, shape);
	}

	if (face != Facing::UP && face != Facing::DOWN && !Block::shouldRenderFace(region, pos, face, shape)) {
		return false;
	}

	int ox = pos.x, oy = pos.y, oz = pos.z;
	ox += Facing::STEP_X[Facing::OPPOSITE_FACING[face]];
	oy += Facing::STEP_Y[Facing::OPPOSITE_FACING[face]];
	oz += Facing::STEP_Z[Facing::OPPOSITE_FACING[face]];

	bool isUpper = !isBottomSlab(region, BlockPos(ox, oy, oz));
	if (isUpper) {
		if (face == Facing::DOWN) {
			return true;
		}
		if (face == Facing::UP && Block::shouldRenderFace(region, pos, face, shape)) {
			return true;
		}

		return !(isHalfSlab(region.getBlockID(pos)) && !isBottomSlab(region.getData(pos)));
	} else {
		if (face == Facing::UP) {
			return true;
		}
		if (face == Facing::DOWN && Block::shouldRenderFace(region, pos, face, shape)) {
			return true;
		}

		return !(isHalfSlab(region.getBlockID(pos)) && !isBottomSlab(region.getData(pos)));
	}
}

bool SlabBlock::isObstructingChests(BlockSource& region, const BlockPos& pos) const {
	return isBottomSlab(region, pos);
	}

bool SlabBlock::canBeSilkTouched() const {
	return false;
}

 BlockProperty SlabBlock::getRedstoneProperty(BlockSource& region, const BlockPos& pos) const {
	if (!isBottomSlab(region.getData(pos))) {
		return BlockProperty::Power_BlockDown;
	}
	return BlockProperty::Unspecified;
}

bool SlabBlock::checkIsPathable(Entity& entity, const BlockPos& lastPathPos, const BlockPos& pathPos) const {
	return true;
}
