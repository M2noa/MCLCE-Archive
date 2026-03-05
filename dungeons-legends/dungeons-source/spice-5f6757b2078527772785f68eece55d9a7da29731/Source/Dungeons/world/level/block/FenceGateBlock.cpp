/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#include "world/level/block/FenceGateBlock.h"
#include "world/level/block/WoodBlock.h"
#include "world/level/BlockSource.h"
#include "world/level/material/Material.h"
#include "world/Direction.h"
#include "world/entity/Mob.h"
#include "world/entity/player/Player.h"
#include "world/level/Level.h"
#include "world/level/dimension/Dimension.h"
#include "world/level/block/LevelEvent.h"
// #include "world/redstone/circuit/CircuitSystem.h"
// #include "world/redstone/circuit/components/ConsumerComponent.h"
#include "world/redstone/Redstone.h"

FenceGateBlock::FenceGateBlock(const std::string& nameId, int id, WoodBlockType woodType) :
	Block(nameId, id, Material::getMaterial(MaterialType::Wood)) {
	setSolid(false);
	mProperties = BlockProperty::FenceGate;
	mTranslucency[id] = std::max(0.8f, mMaterial.getTranslucency());
}

void FenceGateBlock::onLoaded(BlockSource& region, const BlockPos& pos) const {
// 	if (!region.getLevel().isClientSide()) {
// 		auto &circuit = region.getDimension().getCircuitSystem();
// 		auto consumer = circuit.create<ConsumerComponent>(pos, &region, Facing::DOWN);
// 		if (consumer) {
// 			auto t = region.getBlockAndData(pos);
// 			bool wasOpen = getBlockState(BlockState::OpenBit).getBool(t.data);
// 			consumer->setStrength(wasOpen ? Redstone::SIGNAL_MAX : Redstone::SIGNAL_MIN);
// 			consumer->setAllowPowerUp(true);
// 		}
// 	}
}


void FenceGateBlock::onPlace(BlockSource& region, const BlockPos& pos) const {
	_checkIsInWall(region, pos);

	onLoaded(region, pos);
}

void FenceGateBlock::onRedstoneUpdate(BlockSource& region, const BlockPos& pos, int strength, bool isFirstTime) const {
	if (!isFirstTime) {
		auto t = region.getBlockAndData(pos);

		// the gate will be open if there is a signal
		getBlockState(BlockState::OpenBit).set(t.data, strength > 0);
		region.setBlockAndData(pos, t, UPDATE_CLIENTS);
		_onOpenChanged(region, pos);
	}
}

bool FenceGateBlock::mayPlace(BlockSource& region, const BlockPos& pos) const {
	if(!region.getMaterial(pos.below()).isSolid()) {
		return false;
	}

	return Block::mayPlace(region, pos);
}

const AABB& FenceGateBlock::getAABB(BlockSource& region, const BlockPos& pos, AABB& bufferValue, int optionalData, bool isClipping, int clipData) const {
	Vec3 p(pos);
	if(isClipping) {
		return bufferValue.set(Vec3::ZERO, Vec3::ONE).move(p);
	}

	int data = region.getData(pos);
	if(_isOpen(data)) {
		return AABB::EMPTY;
	}

	int direction = getBlockState(BlockState::Direction).get<int>(data);
	if(direction == Direction::NORTH || direction == Direction::SOUTH) {
		bufferValue.set(Vec3(p.x, p.y, p.z + 6.0f / 16.0f), Vec3(p.x + 1, p.y + 1.5f, p.z + 10.0f / 16.0f));
	} else {
		bufferValue.set(Vec3(p.x + 6.0f / 16.0f, p.y, p.z), Vec3(p.x + 10.0f / 16.0f, p.y + 1.5f, p.z + 1));
	}

	return bufferValue;
}

void FenceGateBlock::neighborChanged(BlockSource& region, const BlockPos& pos, const BlockPos& neighborPos) const {
	_checkIsInWall(region, pos);
}

int FenceGateBlock::getPlacementDataValue(Entity& by, const BlockPos& pos, FacingID face, const Vec3& clickPos, int itemValue) const {
	int dir = (((Math::floor(by.getRotation().y * 4 / (360) + 0.5f)) & 3)) % 4;

	DataID data = 0;
	getBlockState(BlockState::Direction).set(data, dir);

	return data;
}

bool FenceGateBlock::use(Player& player, const BlockPos& pos) const {
	auto& region = player.getRegion();
	auto t = region.getBlockAndData(pos);
	if(_isOpen(t.data)) {
		getBlockState(BlockState::OpenBit).set(t.data, false);
		region.setBlockAndData(pos, t, Block::UPDATE_ALL, &player);
	} else {
		// open the door from the player
		int dir = (((Math::floor(player.getRotation().y * 4 / (360) + 0.5f)) & 3)) % 4;
		int current = getBlockState(BlockState::Direction).get<int>(t.data);
		if(current == ((dir + 2) % 4)) {
			getBlockState(BlockState::Direction).set(t.data, dir);
		}

		getBlockState(BlockState::OpenBit).set(t.data, true);
		region.setBlockAndData(pos, t, Block::UPDATE_ALL, &player);
	}
	_onOpenChanged(region, pos);

// 	region.getLevel().broadcastDimensionEvent(region, LevelEvent::SoundOpenDoor, Vec3(pos.x + 0.5f, pos.y + 0.5f, pos.z + 0.5f), 0);
	return true;
}

bool FenceGateBlock::_isOpen(int data) const {
	return getBlockState(BlockState::OpenBit).getBool(data);
}

bool FenceGateBlock::_isInWall(int data) const {
	return getBlockState(BlockState::InWallBit).getBool(data);
}

bool FenceGateBlock::isInteractiveBlock() const {
	return true;
}

void FenceGateBlock::_checkIsInWall(BlockSource& region, const BlockPos& pos) const {
	FullBlock block = region.getBlockAndData(pos);
	BlockID firstFlank(0), secondFlank(0);

	// determine which direction gate is facing, and get data on the two blocks that flank this position in that
	// direction
	switch (getBlockState(BlockState::Direction).get<int>(block.data)) {
	case Direction::NORTH:
	case Direction::SOUTH:
		firstFlank = region.getBlockID(pos.x + 1, pos.y, pos.z);
		secondFlank = region.getBlockID(pos.x - 1, pos.y, pos.z);
		break;

	case Direction::EAST:
	case Direction::WEST:
		firstFlank = region.getBlockID(pos.x, pos.y, pos.z + 1);
		secondFlank = region.getBlockID(pos.x, pos.y, pos.z - 1);
		break;

	// shouldn't ever have an up or down facing fence gate, but just in case
	default:
		break;
	}

	DataID data = block.data;
	if (firstFlank == Block::mCobblestoneWall->mID || secondFlank == Block::mCobblestoneWall->mID) {
		getBlockState(BlockState::InWallBit).set(data, true);
	} else {
		getBlockState(BlockState::InWallBit).set(data, false);
	}

	// if data was changed, update blocks
	if (data != block.data) {
		block.data = data;
		region.setBlockAndData(pos, block, Block::UPDATE_ALL);
		_onOpenChanged(region, pos);
	}
}

void FenceGateBlock::_onOpenChanged(BlockSource& region, const BlockPos& pos) const {
		// Set one way collision on for any blocks that were standing where this gate ended up so they don't violently resolve out of it
// 		AABB bb;
// 		getAABB(region, pos, bb);
// 		const EntityList& entitiesInDoor = region.getEntities(nullptr, bb);
// 		for(Entity* entity : entitiesInDoor) {
// 			entity->onOnewayCollision(bb);
// 		}
}

