/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#include "world/level/block/LeverBlock.h"

#include "util/Math.h"
#include "world/entity/Entity.h"
#include "world/entity/player/Player.h"
#include "world/Facing.h"
#include "world/item/ItemInstance.h"
#include "world/level/BlockSource.h"
#include "world/level/Level.h"
#include "world/level/dimension/Dimension.h"
#include "world/level/material/Material.h"
#include "world/level/block/ButtonBlock.h"
#include "world/phys/AABB.h"
#include "world/redstone/Redstone.h"
// #include "world/redstone/circuit/CircuitSystem.h"
// #include "world/redstone/circuit/components/ProducerComponent.h"

LeverBlock::LeverBlock(const std::string& nameId, int id)
	: Block(nameId, id, Material::getMaterial(MaterialType::Decoration)) {
	setSolid(false);
	mProperties = BlockProperty::BreakOnPush;

	mRenderLayer = BlockRenderLayer::RENDERLAYER_ALPHATEST_SINGLE_SIDE;
}

const AABB& LeverBlock::getAABB(BlockSource& region, const BlockPos& pos, AABB& bufferValue, int optionalData, bool isClipping, int clipData) const {
	if (isClipping) {
		_getShape(region, pos, bufferValue);
		return bufferValue.move(pos);
	}
	return AABB::EMPTY;
}

const AABB& LeverBlock::getVisualShape(BlockSource& region, const BlockPos& pos, AABB& bufferAABB, bool isClipping) const {
	_getShape(region, pos, bufferAABB);
	return bufferAABB;
}

const AABB& LeverBlock::getVisualShape(DataID data, AABB& bufferAABB, bool isClipping) const {
	_getShape(data, bufferAABB);
	return bufferAABB;
}

void LeverBlock::_getShape(BlockSource& region, const BlockPos& pos, AABB& bufferValue) const {
	_getShape(region.getData(pos), bufferValue);
}

void LeverBlock::_getShape(DataID data, AABB& bufferValue) const {
	int dir = _getDirectionFromData(data);
	float r = 3 / 16.0f;
	float bottomSpacing = 4.0f / 16.0f;
	float height = 8.0f / 16.0f;
	switch (dir) {
	case 1:
		bufferValue.set(Vec3(0, bottomSpacing, 0.5f - r), Vec3(r * 2, bottomSpacing + height, 0.5f + r));
		break;
	case 2:
		bufferValue.set(Vec3(1 - r * 2, bottomSpacing, 0.5f - r), Vec3(1, bottomSpacing + height, 0.5f + r));
		break;
	case 3:
		bufferValue.set(Vec3(0.5f - r, bottomSpacing, 0), Vec3(0.5f + r, bottomSpacing + height, r * 2));
		break;
	case 4:
		bufferValue.set(Vec3(0.5f - r, bottomSpacing, 1 - r * 2), Vec3(0.5f + r, bottomSpacing + height, 1));
		break;
	case 5:
	case 6:
		r = 4 / 16.0f;
		bufferValue.set(Vec3(0.5f - r, 0.0f, 0.5f - r), Vec3(0.5f + r, 0.6f, 0.5f + r));
		break;
	case 0:
	case 7:
	default:
		r = 4 / 16.0f;
		bufferValue.set(Vec3(0.5f - r, 0.4f, 0.5f - r), Vec3(0.5f + r, 1.0f, 0.5f + r));
		break;
	}
}

bool LeverBlock::mayPlace(BlockSource& region, const BlockPos& pos, FacingID face) const {
	return ButtonBlock::canAttachTo(region, pos, Facing::OPPOSITE_FACING[face]);
}

bool LeverBlock::mayPlace(BlockSource& region, const BlockPos& pos) const {
	for (FacingID facing = 0; facing < Facing::MAX; ++facing) {
		if (mayPlace(region, pos, facing)) {
			return true;
		}
	}
	return false;
}

int LeverBlock::getPlacementDataValue(Entity& by, const BlockPos& pos, FacingID face, const Vec3& clickPos, int itemValue) const {
	bool bFlip = getBlockState(BlockState::OpenBit).getBool(itemValue);
	int dir = getLeverFacing(face);

	DataID data = 0;
	getBlockState(BlockState::FacingDirection).set(data, dir);
	getBlockState(BlockState::OpenBit).set(data, bFlip);

	// not sure what 6 and 7 are used for, feels like a hack to me
	if (dir == getLeverFacing(Facing::UP)) {
		if ((Math::floor(by.getYHeadRot() * 4 / (360) + 0.5f) & 1) == 0) {
			getBlockState(BlockState::FacingDirection).set(data, 5);
		}
		else {
			getBlockState(BlockState::FacingDirection).set(data, 6);
		}
	}
	else if (dir == getLeverFacing(Facing::DOWN)) {
		if ((Math::floor(by.getYHeadRot() * 4 / (360) + 0.5f) & 1) == 0) {
			getBlockState(BlockState::FacingDirection).set(data, 7);
		}
		else {
			getBlockState(BlockState::FacingDirection).set(data, 0);
		}
	}

	return data;
}

int LeverBlock::getLeverFacing(int facing) {
	switch (facing) {
	case Facing::DOWN:
		return 0;
	case Facing::UP:
		return 5;
	case Facing::NORTH:
		return 4;
	case Facing::SOUTH:
		return 3;
	case Facing::WEST:
		return 2;
	case Facing::EAST:
		return 1;
	}
	return -1;
}

void LeverBlock::neighborChanged(BlockSource& region, const BlockPos& pos, const BlockPos& neighborPos) const {
	if (!_checkCanSurvive(region, pos)) {
		region.removeBlock(pos);
	}
}

bool LeverBlock::_checkCanSurvive(BlockSource &region, const BlockPos& pos) const {
	DataID data = region.getData(pos);
	int facing = _getFacingFromData(data);

	if (!mayPlace(region, pos, facing)) {
		spawnResources(region, pos, region.getData(pos), 1);
		region.removeBlock(pos);
		return false;
	}

	return true;
}

int LeverBlock::_getDirectionFromData(int data) const {
	return getBlockState(BlockState::FacingDirection).get<int>(data);
}

FacingID LeverBlock::_getFacingFromData(int data) const {
	int dir = _getDirectionFromData(data);
	if (dir == 1) {
		return Facing::EAST;
	} else if (dir == 2) {
		return Facing::WEST;
	} else if (dir == 3) {
		return Facing::SOUTH;
	} else if (dir == 4) {
		return Facing::NORTH;
	} else if (dir == 5 || dir == 6) {
		return Facing::UP;
	} else {
		return Facing::DOWN;
	}
}

bool LeverBlock::isAttachedTo(BlockSource& region, const BlockPos& pos, BlockPos& outAttachedTo) const {
	DataID data = region.getData(pos);
	FacingID facing = _getFacingFromData(data);
	outAttachedTo = pos - Facing::DIRECTION[facing];
	return true;
}

FacingID LeverBlock::_getOppositeDirectionFromData(int data) {
	return Facing::OPPOSITE_FACING[_getDirectionFromData(data)];
}

void LeverBlock::updateShape(BlockSource *level, const BlockPos& pos) {
	DEBUG_ASSERT(level != nullptr, "level is not set!");

	AABB tempAABB;
	_getShape(*level, pos, tempAABB);
	setVisualShape(tempAABB.min, tempAABB.max);
}

bool LeverBlock::use(Player& player, const BlockPos& pos) const {
	BlockSource& region = player.getRegion();

	if (region.getLevel().isClientSide()) {
		return true;
	}
	FullBlock data = region.getBlockAndData(pos);

	// toggle the lever
	auto& openState = getBlockState(BlockState::OpenBit);
	bool bOpen = openState.getBool(data.data);
	openState.set(data.data, !bOpen);

	region.setBlockAndData(pos, data, Block::UPDATE_ALL, &player);

// 	if (!region.getLevel().isClientSide()) {
// 		region.getDimension().getCircuitSystem().setStrength(pos,getSignal(region, pos, data.data));
// 	}
	
	region.getLevel().broadcastDimensionEvent(region, LevelEvent::ActivateBlock, Vec3(pos.x + 0.5f, pos.y + 0.5f, pos.z + 0.5f), bOpen ? 600 : 500);

	return true;
}

void LeverBlock::onLoaded(BlockSource& region, const BlockPos& pos) const {
// 	FullBlock data = region.getBlockAndData(pos);
// 	FacingID facing = _getFacingFromData(data.data);
// 
// 	if (!region.getLevel().isClientSide()) {
// 		auto component = region.getDimension().getCircuitSystem().create<ProducerComponent>(pos, &region, Facing::OPPOSITE_FACING[facing]); 
// 		if (component) {
// 			component->setStrength(getSignal(region, pos, data.data));
// 			component->allowAttachments(true);
// 			component->setAllowPowerUp(true);
// 		}
// 	}
}

void LeverBlock::onPlace(BlockSource& region, const BlockPos& pos) const {
	onLoaded(region, pos);
}

void LeverBlock::onRemove(BlockSource& region, const BlockPos& pos) const {

// 	if (!region.getLevel().isClientSide()) {
// 		region.getDimension().getCircuitSystem().removeComponents(pos);
// 	}
	Block::onRemove(region, pos/*, oldTileID*/);
}

int LeverBlock::getSignal(BlockSource& region, const BlockPos& pos, int dir) const {
	return (region.getData(pos) & 8) > 0 ? Redstone::SIGNAL_MAX : Redstone::SIGNAL_NONE;
}

 
ItemInstance LeverBlock::asItemInstance(BlockSource& region, const BlockPos& pos, int blockData) const {
	return ItemInstance(Block::mLever);
}

int LeverBlock::getResourceCount(Random& random, int data, int bonusLootLevel) const {
	return 1;
}

int LeverBlock::getResource(Random& random, int data, int bonusLootLevel) const {
	return Block::mLever->mID;
}

bool LeverBlock::shouldConnectToRedstone(BlockSource& region, const BlockPos& pos, int direction) const
{
	return true;
}
