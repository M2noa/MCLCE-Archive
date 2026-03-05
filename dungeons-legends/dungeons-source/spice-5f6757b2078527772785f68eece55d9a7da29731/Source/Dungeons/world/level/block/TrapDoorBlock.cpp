/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#include "world/level/block/TrapDoorBlock.h"
#include "world/level/material/Material.h"
#include "world/level/Level.h"
#include "world/level/dimension/Dimension.h"
#include "world/level/block/LevelEvent.h"
#include "world/level/BlockSource.h"
#include "world/entity/player/Player.h"
#include "world/Facing.h"
// #include "world/redstone/circuit/CircuitSystem.h"
// #include "world/redstone/circuit/components/ConsumerComponent.h"
#include "world/redstone/Redstone.h"

TrapDoorBlock::TrapDoorBlock(const std::string& nameId, int id, const Material& material )
	: Block(nameId, id, material) {
	float r = 0.5f;
	float h = 1.0f;
	setSolid(false);
	mRenderLayer = RENDERLAYER_ALPHATEST_SINGLE_SIDE;
	mTranslucency[id] = 0.8f;

	// just needs to not be a full block, which is the condition for a block to suffocate players
	mProperties = BlockProperty::Trap | BlockProperty::Walkable;

	Block::setVisualShape(Vec3(0.5f - r, 0, 0.5f - r), Vec3(0.5f + r, h, 0.5f + r));
}

const AABB& TrapDoorBlock::getVisualShape(DataID data, AABB& bufferAABB, bool isClipping) const {
	float r = (3 / 16.0f) - 0.005f;
	float buffer = .0005f;	// Buffer spacing to prevent Z-fighting when looking through holes in the trap door

	// upside down trapdoors open to fill the same area as rightside up trapdoors, so the orientation check is not
	// necessary for open doors
	if(_isOpen(data)) {
		switch (_getDirection(data)) {
		case DIR_NORTH:
			bufferAABB.set(Vec3(buffer, buffer, 1 - r), Vec3(1 - buffer, 1 - buffer, 1 - buffer));
			break;
		case DIR_SOUTH:
			bufferAABB.set(Vec3(buffer, buffer, buffer), Vec3(1 - buffer, 1 - buffer, r));
			break;
		case DIR_WEST:
			bufferAABB.set(Vec3(1 - r, buffer, buffer), Vec3(1 - buffer, 1 - buffer, 1 - buffer));
			break;
		case DIR_EAST:
			bufferAABB.set(Vec3(buffer, buffer, buffer), Vec3(r, 1 - buffer, 1 - buffer));
			break;
		default:
			break;
		}
	} else {
		if (_isUpsideDown(data)) {
			bufferAABB.set(Vec3(buffer, 1.0f - r, buffer), Vec3(1.0f - buffer, 1.0f - buffer, 1.0f - buffer));
		} else {
			bufferAABB.set(Vec3(buffer, buffer, buffer), Vec3(1.0f - buffer, r, 1.0f - buffer));
		}
	}

	return bufferAABB;
}

void TrapDoorBlock::_toggleOpen(BlockSource& region, Entity* user, const BlockPos& pos) const {
		FullBlock t = region.getBlockAndData(pos);

		getBlockState(BlockState::OpenBit).set(t.data, !_isOpen(t.data));
		region.setBlockAndData(pos, t, Block::UPDATE_ALL);

		// Set one way collision on for any blocks that were standing where this door ended up so they don't violently resolve out of it
// 		AABB bb;
// 		getAABB(region, pos, bb);
// 		const EntityList& entitiesInDoor = region.getEntities(nullptr, bb);
// 		for(Entity* entity : entitiesInDoor)
// 			entity->onOnewayCollision(bb);

// 		region.getLevel().broadcastDimensionEvent(region, LevelEvent::SoundOpenDoor, Vec3(pos) + Vec3(0.5f), 0);
}


bool TrapDoorBlock::use(Player& player, const BlockPos& pos) const {
	if (mMaterial != Material::getMaterial(MaterialType::Metal)) {
		_toggleOpen(player.getRegion(), &player, pos);
	}

	return true;
}

void TrapDoorBlock::setOpen(BlockSource& region, const BlockPos& pos, bool shouldOpen) const {
	auto t = region.getBlockAndData(pos);
	bool wasOpen = _isOpen(t.data);

	if (wasOpen != shouldOpen) {
		_toggleOpen(region, nullptr, pos);
	}
}

int TrapDoorBlock::getPlacementDataValue(Entity& by, const BlockPos& pos, FacingID face, const Vec3& clickPos, int itemValue) const {
	int dir = Math::floor(by.getRotation().y * 4 / 360 + 0.5f) & 3;

	DataID data = 0;

	if (dir == 0) {
		getBlockState(BlockState::Direction).set(data, DIR_NORTH);
	} else if (dir == 1) {
		getBlockState(BlockState::Direction).set(data, DIR_EAST);
	} else if (dir == 2) {
		getBlockState(BlockState::Direction).set(data, DIR_SOUTH);
	} else {
		getBlockState(BlockState::Direction).set(data, DIR_WEST);
	}

	bool bUpsideDown = (face == Facing::DOWN || (face != Facing::UP && clickPos.y > 0.5f));
	getBlockState(BlockState::UpsideDownBit).set(data, bUpsideDown);

	return data;
}

ItemInstance TrapDoorBlock::asItemInstance(BlockSource& region, const BlockPos& pos, int blockData) const {
	return ItemInstance(mBlocks[mID]);
}

int TrapDoorBlock::getResourceCount(Random& random, int data, int bonusLootLevel) const {
	return 1;
}
int TrapDoorBlock::getResource(Random& random, int data, int bonusLootLevel) const {
	return mID;
}

bool TrapDoorBlock::isInteractiveBlock() const {
	return true;
}

bool TrapDoorBlock::canBeSilkTouched() const {
	return false;
}

bool TrapDoorBlock::_isOpen(DataID data) const {
	return getBlockState(BlockState::OpenBit).getBool(data);
}

bool TrapDoorBlock::_isUpsideDown(DataID data) const {
	return getBlockState(BlockState::UpsideDownBit).getBool(data);
}

int TrapDoorBlock::_getDirection(DataID data) const {
	return getBlockState(BlockState::Direction).get<int>(data);
}

void TrapDoorBlock::onLoaded(BlockSource& region, const BlockPos& pos) const {

// 	if (!region.getLevel().isClientSide()) {
// 		auto &circuit = region.getDimension().getCircuitSystem();
// 		auto consumer = circuit.create<ConsumerComponent>(pos, &region, Facing::DOWN);
// 		if (consumer) {
// 			auto t = region.getBlockAndData(pos);
// 			bool wasOpen = _isOpen(t.data);
// 			consumer->setStrength( wasOpen ? Redstone::SIGNAL_MAX : Redstone::SIGNAL_MIN);
// 			consumer->setAllowPowerUp(true);
// 			consumer->setAcceptHalfPulse(true);
// 		}
// 	}
}

void TrapDoorBlock::onRedstoneUpdate(BlockSource& region, const BlockPos& pos, int strength, bool isFirstTime) const {
	if (!isFirstTime) {
		setOpen(region, pos, strength > 0);
	}
}

void TrapDoorBlock::onPlace(BlockSource& region, const BlockPos& pos) const {
	Block::onPlace(region, pos);
	onLoaded(region, pos);
}
