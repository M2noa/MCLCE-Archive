/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/

#include "Dungeons.h"

#include "PistonBlock.h"
// #include "world/level/block/entity/PistonBlockEntity.h"

#include "world/Facing.h"
#include "world/entity/Mob.h"
#include "world/level/BlockSource.h"
#include "world/level/block/entity/BlockEntity.h"
#include "world/level/material/Material.h"

#include "world/level/Level.h"
#include "world/level/dimension/Dimension.h"
#include "world/redstone/Redstone.h"
// #include "world/redstone/circuit/CircuitSystem.h"
// #include "world/redstone/circuit/components/PistonConsumer.h"

const BlockPos PistonBlock::ARM_DIRECTION_OFFSETS[6] {
	{0.0f, -1.0f, 0.0f},		// DOWN = 0,
	{0.0f, 1.0f, 0.0f},			// UP = 1,
	{0.0f, 0.0f, 1.0f},			// NORTH = 2,
	{0.0f, 0.0f, -1.0f},		// SOUTH = 3,
	{1.0f, 0.0f, 0.0f},			// WEST = 4,
	{-1.0f, 0.0f, 0.0f}			// EAST = 5,
};

PistonBlock::PistonBlock(const std::string& nameId, int id, Type isSticky) 
	: EntityBlock(nameId, id, Material::getMaterial(MaterialType::Piston))
	, mType (isSticky) {
	
	setSolid(false);
	setPushesOutItems(true);

	addProperty(BlockProperty::Piston);
	
	mBlockEntityType = BlockEntityType::PistonArm;
	mTranslucency[id] = std::max(0.8f, mMaterial.getTranslucency());
}

int PistonBlock::getVariant(int data) const {
	return 0;
}

FacingID PistonBlock::getMappedFace(FacingID face, int data) const{
	FacingID pistonFacing = PistonBlock::getFacing(data);

	switch (face)
	{
	case Facing::UP:
		if (pistonFacing == Facing::DOWN) {
			return Facing::DOWN;
		}
		else if (pistonFacing == Facing::UP) {
			return Facing::UP;
		}
		else {
			return Facing::SOUTH;
		}
	case Facing::DOWN:
		if (pistonFacing == Facing::DOWN) {
			return Facing::UP;
		}
		else if (pistonFacing == Facing::UP) {
			return Facing::DOWN;
		}
		else {
			return Facing::SOUTH;
		}
	case Facing::NORTH:
		if (pistonFacing == Facing::NORTH) {
			return Facing::DOWN;
		}
		else if (pistonFacing == Facing::SOUTH) {
			return Facing::UP;
		}
		else {
			return Facing::SOUTH;
		}
	case Facing::SOUTH:
		if (pistonFacing == Facing::NORTH) {
			return Facing::UP;
		}
		else if (pistonFacing == Facing::SOUTH) {
			return Facing::DOWN;
		}
		else {
			return Facing::SOUTH;
		}
	case Facing::WEST:
		if (pistonFacing == Facing::EAST) {
			return Facing::UP;
		}
		else if (pistonFacing == Facing::WEST) {
			return Facing::DOWN;
		}
		else {
			return Facing::SOUTH;
		}
	case Facing::EAST:
		if (pistonFacing == Facing::WEST) {
			return Facing::UP;
		}
		else if (pistonFacing == Facing::EAST) {
			return Facing::DOWN;
		}
		else {
			return Facing::SOUTH;
		}
	default:
		break;
	}

	return Facing::SOUTH;
}

DataID PistonBlock::getSpawnResourcesAuxValue(DataID data) const {
	return getBlockState(BlockState::FacingDirection).get<DataID>(data);
}

int PistonBlock::getPlacementDataValue(Entity& by, const BlockPos& pos, FacingID face, const Vec3& clickPos, int itemValue) const {
	// legacy code while written.
	// PistonItem; should have an onUse that determines this position.
	// it seems we don't have such thing.

	return getPlacementFacingAll(by, pos, 180.0f);
}

void PistonBlock::onLoaded(BlockSource& region, const BlockPos& pos) const {
// 	if(!region.getLevel().isClientSide()) {
// 		auto &circuit = region.getDimension().getCircuitSystem();
// 		FullBlock block = region.getBlockAndData(pos);
// 		FacingID facing = PistonBlock::getFacing(block.data);
// 		auto component = circuit.create<PistonConsumer>(pos, &region, facing);
// 		component->setBlockPowerFace(facing);
// 		
// 		PistonBlockEntity* piston = static_cast<PistonBlockEntity*>(region.getBlockEntity(pos));
// 		if (piston != nullptr) {
// 			component->setStrength(piston->isExpanded() ? Redstone::SIGNAL_MAX : Redstone::SIGNAL_MIN);
// 		}
// 	}
}

void PistonBlock::onPlace(BlockSource& region, const BlockPos& pos) const {
	Block::onPlace(region, pos);
	onLoaded(region, pos);
}

void PistonBlock::onRedstoneUpdate(BlockSource& region, const BlockPos& pos, int strength, bool isFirstTime) const {
}

ItemInstance PistonBlock::asItemInstance(BlockSource& region, const BlockPos& pos, int blockData) const {
	DataID data = 0;
	getBlockState(BlockState::FacingDirection).set(data, Facing::UP);

	if (isSticky()) {
		return ItemInstance(Block::mStickyPiston, 1, data);
	} else {
		return ItemInstance(Block::mPiston, 1, data);
	}
}

bool PistonBlock::canProvideSupport(BlockSource& region, const BlockPos& pos, FacingID face, BlockSupportType type) const {
	return true;
}

void PistonBlock::spawnResources(BlockSource& region, const BlockPos& pos, int data, float odds, int bonusLootLevel) const {
	DataID newData = 0;
	getBlockState(BlockState::FacingDirection).set(newData, Facing::UP);

	if (isSticky()) {
		popResource(region, pos, ItemInstance(Block::mStickyPiston, 1, newData));
	} else {
		popResource(region, pos, ItemInstance(Block::mPiston, 1, newData));
	}
}

//

void PistonBlock::neighborChanged(BlockSource& region, const BlockPos& pos, const BlockPos& neighborPos) const {
	// cant do the second part check here; no entity
// 	int data = region.getData(pos);
// 	BlockPos headPos = pos + getFacingDir(data);
// 
// 	if(headPos == neighborPos) {
// 		if(region.getBlockID(neighborPos) != Block::mPistonArm->mID) {
// 			PistonBlockEntity* arm = static_cast<PistonBlockEntity*>(region.getBlockEntity(pos));
// 			if (arm) {
// 				if (!arm->isRetracting()) { // Removal of the arm while retracting is intended
// 					if (arm->getProgress(0.0f) > 0.0001f) {
// 						spawnResources(region, pos, data, 1.0f, 0);
// 						region.setBlock(pos, BlockID::AIR, Block::UPDATE_ALL);
// 					}
// 				}
// 			}
// 		}
// 	}
}

bool PistonBlock::canBeSilkTouched() const {
	return false;
}

bool PistonBlock::getSecondPart(BlockSource& region, const BlockPos& pos, BlockPos& out) const {
// 	// retracted pistons do not have a second part
// 	PistonBlockEntity* pistonEntity = static_cast<PistonBlockEntity*>(region.getBlockEntity(pos));
// 	if(pistonEntity && pistonEntity->getProgress(1.0f) == 0.0f) {
// 		return false;
// 	}
// 
// 	int data = region.getData(pos);
// 	out = pos + getFacingDir(data);
	return true;
}

bool PistonBlock::shouldConnectToRedstone(BlockSource& region, const BlockPos& pos, int direction) const {
	FacingID pistonFacing = PistonBlock::getFacing(region.getData(pos));
	if (pistonFacing != Direction::DIRECTION_FACING[direction]) {
		return true;
	}

	return false;
}

bool PistonBlock::isSticky() const {
	return (mType == Type::Sticky);
}
