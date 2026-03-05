#include "Dungeons.h"

#include "TripWireHookBlock.h"
#include "world/entity/Mob.h"
#include "world/level/Level.h"
#include "world/level/dimension/Dimension.h"
#include "world/phys/AABB.h"
#include "world/Facing.h"
#include "world/Direction.h"
#include "TripWireBlock.h"
#include "world/level/BlockSource.h"
#include "world/level/material/Material.h"
#include "world/redstone/Redstone.h"
// #include "world/level/BlockTickingQueue.h"
#include "world/level/block/ButtonBlock.h"
// #include "world/redstone/circuit/CircuitSystem.h"
// #include "world/redstone/circuit/components/ProducerComponent.h"

TripWireHookBlock::TripWireHookBlock(const std::string& nameId, int id) 
	: Block(nameId, id, Material::getMaterial(MaterialType::Decoration)) {
	setTicking(true);
	setSolid(false);
	mProperties = BlockProperty::BreakOnPush;
	mRenderLayer = RENDERLAYER_ALPHATEST;
	mTranslucency[id] = std::max(0.8f, mMaterial.getTranslucency());
}

const AABB& TripWireHookBlock::getAABB(BlockSource& region, const BlockPos& pos, AABB& bufferValue, int optionalData, bool isClipping, int clipData) const {
	if (isClipping) {
		_getShape(region, pos, bufferValue);
		return bufferValue.move( pos );
	}
	return AABB::EMPTY;
}

const AABB& TripWireHookBlock::getVisualShape(BlockSource& region, const BlockPos& pos, AABB& bufferAABB, bool isClipping) const {
	_getShape(region, pos, bufferAABB);
	return bufferAABB;
}

const AABB& TripWireHookBlock::getVisualShape(DataID data, AABB& bufferAABB, bool isClipping) const {
	_getShape(data, bufferAABB);
	return bufferAABB;
}

void TripWireHookBlock::_getShape(BlockSource& region, const BlockPos& pos, AABB& bufferValue) const {
	_getShape(region.getData(pos), bufferValue);
}

void TripWireHookBlock::_getShape(DataID data, AABB& bufferValue) const {
	int dir = getDirection(data);

	float sideSpace = 6.0f / 16.0f;
	float width = 4.0f / 16.0f;
	float bottomSpace = 1.0f / 16.0f;
	float height = 8.0f / 16.0f;
	float depth = 6.0f / 16.0f;

	switch (dir) {
	case Direction::EAST:
		bufferValue.set(Vec3(0.0f, bottomSpace, sideSpace), Vec3(depth, bottomSpace + height, sideSpace + width));
		break;
	case Direction::WEST:
		bufferValue.set(Vec3(1.0f - depth, bottomSpace, sideSpace), Vec3(1.0f, bottomSpace + height, sideSpace + width));
		break;
	case Direction::SOUTH:
		bufferValue.set(Vec3(sideSpace, bottomSpace, 0.0f), Vec3(sideSpace + width, bottomSpace + height, depth));
		break;
	default:
	case Direction::NORTH:
		bufferValue.set(Vec3(sideSpace, bottomSpace, 1.0f - depth), Vec3(sideSpace + width, bottomSpace + height, 1.0f));
		break;
	}
}

int TripWireHookBlock::getTickDelay() const {
	return 10;
}

bool TripWireHookBlock::mayPlace(BlockSource& region, const BlockPos& pos, FacingID face) const {
	if (face != Facing::UP && face != Facing::DOWN) {		
		return ButtonBlock::canAttachTo(region, pos, Facing::OPPOSITE_FACING[face]);
	}
	return false;
}

bool TripWireHookBlock::mayPlace(BlockSource& region, const BlockPos& pos) const {
	for (int facing = Facing::NORTH; facing < Facing::MAX; ++facing) {
		if (mayPlace(region, pos, facing))
			return true;
	}
	return false;
}

int TripWireHookBlock::getPlacementDataValue(Entity& by, const BlockPos& pos, FacingID face, const Vec3& clickPos, int itemValue) const {
	
	DataID data = 0;

	if (face == Facing::NORTH) {
		getBlockState(BlockState::Direction).set(data, Direction::NORTH);
	}
	if (face == Facing::SOUTH) {
		getBlockState(BlockState::Direction).set(data, Direction::SOUTH);
	}												   
	if (face == Facing::WEST) {
		getBlockState(BlockState::Direction).set(data, Direction::WEST);
	}												   
	if (face == Facing::EAST) {
		getBlockState(BlockState::Direction).set(data, Direction::EAST);
	}

	return data;
}

void TripWireHookBlock::neighborChanged(BlockSource& region, const BlockPos& pos, const BlockPos& neighborPos) const {

	if ( pos == neighborPos ) {
		return;
	}

	DataID data = region.getData(pos);
	int dir = getDirection(data);
	bool maybeLostAnchor = false;

	switch (dir) {
	case Direction::SOUTH: maybeLostAnchor = (neighborPos == pos.neighbor(Facing::NORTH)); break;
	case Direction::NORTH: maybeLostAnchor = (neighborPos == pos.neighbor(Facing::SOUTH)); break;
	case Direction::WEST: maybeLostAnchor = (neighborPos == pos.neighbor(Facing::EAST)); break;
	case Direction::EAST: maybeLostAnchor = (neighborPos == pos.neighbor(Facing::WEST)); break;
	default:
		break;
	}

	if (maybeLostAnchor && !checkCanSurvive(region, pos)) {
		region.removeBlock(pos);
	}
}

void TripWireHookBlock::calculateState(BlockSource &region, const BlockPos& pos, bool isBeingDestroyed, int data, bool canUpdate, int wireSource, int wireSourceData) const{

	int dir = getDirection(data);
	bool wasAttached = isAttached(data);
	bool wasPowered = isPowered(data);
	bool attached = !isBeingDestroyed;
	bool powered = false;
	int stepX = Direction::STEP_X[dir];
	int stepZ = Direction::STEP_Z[dir];
	int receiverPos = 0;
	int wiresData[WIRE_DIST_MAX];

	for (int i = 1; i < WIRE_DIST_MAX; i++) {
		BlockPos neighborPos(pos.x + stepX*i, pos.y, pos.z + stepZ*i);
		const Block& wireBlock = region.getBlock(neighborPos);

		if (wireBlock.isType(Block::mTripwireHook)) {
			int otherData = region.getData(neighborPos);

			if (getDirection(otherData) == Direction::DIRECTION_OPPOSITE[dir]) {
				receiverPos = i;
			}

			break;
		}
		else if (wireBlock.isType(Block::mTripwire) || i == wireSource) {
			int wireData = i == wireSource ? wireSourceData : region.getData(neighborPos);
			bool wireArmed = !Block::mTripwire->getBlockState(BlockState::DisarmedBit).getBool(wireData);
			bool wirePowered = Block::mTripwire->getBlockState(BlockState::PoweredBit).getBool(wireData);
			powered |= wireArmed && wirePowered;

			wiresData[i] = wireData;

			if (i == wireSource) {
// 				region.getTickQueue(pos)->add(region, pos, mID, getTickDelay());
				attached &= wireArmed;
			}
		}
		else {
			wiresData[i] = -1;
			attached = false;
		}
	}

	attached &= receiverPos > WIRE_DIST_MIN;
	powered &= attached;

	DataID newData = 0;
	getBlockState(BlockState::Direction).set(newData, dir);
	getBlockState(BlockState::PoweredBit).set(newData, powered);
	getBlockState(BlockState::AttachedBit).set(newData, attached);

	if (receiverPos > 0) {
		BlockPos neighborPos(pos.x + stepX * receiverPos, pos.y, pos.z + stepZ * receiverPos);
		int dirOpposite = Direction::DIRECTION_OPPOSITE[dir];
		FullBlock fb = region.getBlockAndData(neighborPos);
		fb.data = newData;
		getBlockState(BlockState::Direction).set(fb.data, dirOpposite);
		region.setBlockAndData( neighborPos, fb, Block::UPDATE_ALL);

// 		if (!region.getLevel().isClientSide()) {
// 			region.getDimension().getCircuitSystem().setStrength(neighborPos, powered ? Redstone::SIGNAL_MAX : Redstone::SIGNAL_NONE);
// 		}

		playSound(region, neighborPos, attached, powered, wasAttached, wasPowered);
	}

	playSound(region, pos, attached, powered, wasAttached, wasPowered);

	if (!isBeingDestroyed) {
		FullBlock fb = region.getBlockAndData(pos);
		fb.data = newData;
		region.setBlockAndData(pos, fb, Block::UPDATE_ALL);

// 		if (!region.getLevel().isClientSide()) {
// 			region.getDimension().getCircuitSystem().setStrength(pos, powered ? Redstone::SIGNAL_MAX : Redstone::SIGNAL_NONE);
// 		}
	}

	if (wasAttached != attached) {
		for (int i = 1; i < receiverPos; i++) {
			const BlockPos neighborPos(pos.x + stepX * i, pos.y, pos.z + stepZ * i);
			int wireData = wiresData[i];
			if (wireData < 0) {
				continue;
			}

			DataID newWireData = wireData;

			if (attached) {
				getBlockState(BlockState::AttachedBit).set(newWireData, true);
			}
			else {
				getBlockState(BlockState::AttachedBit).set(newWireData, false);
			}
			FullBlock fb = region.getBlockAndData(neighborPos);
			fb.data = newWireData;
			region.setBlockAndData(neighborPos, fb, Block::UPDATE_ALL);
		}
	}
}

void TripWireHookBlock::tick(BlockSource& region, const BlockPos& pos, Random& random) const {
	calculateState(region, pos, false, region.getData(pos), true, -1, 0);
}

ItemInstance TripWireHookBlock::asItemInstance(BlockSource& region, const BlockPos& pos, int blockData) const {
	return ItemInstance(Block::mTripwireHook);
}

void TripWireHookBlock::playSound(BlockSource &region, const BlockPos& pos, bool attached, bool powered, bool wasAttached, bool wasPowered) const {
	Level& level = region.getLevel();
	Vec3 soundPos(pos + Vec3(0.5f, 0.1f, 0.5f));

// 	if (powered && !wasPowered) {
// 		level.broadcastSoundEvent(region, LevelSoundEvent::PowerOn, soundPos);
// 	}
// 	else if (!powered && wasPowered) {
// 		level.broadcastSoundEvent(region, LevelSoundEvent::PowerOff, soundPos);
// 	}
// 	else if (attached && !wasAttached) {
// 		level.broadcastSoundEvent(region, LevelSoundEvent::Attach, soundPos);
// 	}
// 	else if (!attached && wasAttached) {
// 		level.broadcastSoundEvent(region, LevelSoundEvent::Detach, soundPos);
// 	}
}

bool TripWireHookBlock::checkCanSurvive(BlockSource &region, const BlockPos& pos) const {
	DataID data = region.getData(pos);
	int dir = getDirection(data);

	bool canSurvive = true;

	switch (dir) {
	case Direction::SOUTH: canSurvive = ButtonBlock::canAttachTo(region, pos, Facing::NORTH); break;
	case Direction::NORTH: canSurvive = ButtonBlock::canAttachTo(region, pos, Facing::SOUTH); break;
	case Direction::WEST: canSurvive = ButtonBlock::canAttachTo(region, pos, Facing::EAST); break;
	case Direction::EAST: canSurvive = ButtonBlock::canAttachTo(region, pos, Facing::WEST); break;
	default:
		break;
	}

	if (!canSurvive) {
		spawnResources(region, pos, region.getData(pos), 1);
		return false;
	}

	return true;
}

void TripWireHookBlock::onLoaded(BlockSource& region, const BlockPos& pos) const {
	FacingID facing = Facing::NOT_DEFINED;

	int data = region.getData(pos);
	switch (getDirection(data)) {
	case Direction::WEST:
		facing = Facing::EAST;
		break;
	case Direction::EAST:
		facing = Facing::WEST;
		break;
	case Direction::NORTH:
		facing = Facing::SOUTH;
		break;
	case Direction::SOUTH:
		facing = Facing::NORTH;
		break;
	default:
		break;
	}

	//Reset the data on first placement or reload 
	FullBlock fb = region.getBlockAndData(pos);
	getBlockState(BlockState::PoweredBit).set(fb.data, false);
 	region.setBlockAndData(pos,fb,  Block::UPDATE_ALL);


// 	if (!region.getLevel().isClientSide()) {
// 		auto producer = region.getDimension().getCircuitSystem().create<ProducerComponent>(pos, &region, facing);
// 		if (producer) {
// 			producer->setStrength(Redstone::SIGNAL_NONE);
// 			producer->allowAttachments(true);
// 			producer->setAllowPowerUp(true);
// 
// 		}
// 	}
}

void TripWireHookBlock::onRemove(BlockSource& region, const BlockPos& pos) const  {
	int data = region.getData(pos);
	if (isAttached(data) || isPowered(data)) {
		calculateState(region, pos, true, data, false, -1, 0);
	}

	//if (powered) {
	//	region.updateNeighborsAt(pos, mID);
	//	int dir = data & MASK_DIR;

	//	if (dir == Direction::EAST) {
	//		region.updateNeighborsAt( pos.west(), mID);
	//	}
	//	else if (dir == Direction::WEST) {
	//		region.updateNeighborsAt( pos.east(), mID);
	//	}
	//	else if (dir == Direction::SOUTH) {
	//		region.updateNeighborsAt( pos.north(), mID);
	//	}
	//	else if (dir == Direction::NORTH) {
	//		region.updateNeighborsAt( pos.south(), mID);
	//	}
	//}

	Block::onRemove(region, pos);
}

void TripWireHookBlock::onPlace(BlockSource& region, const BlockPos& pos) const
{
	Block::onPlace(region, pos);
	onLoaded(region, pos);
	calculateState(region, pos, false, region.getData(pos), true, -1, 0);
}

int TripWireHookBlock::getResourceCount(Random& random, int data, int bonusLootLevel) const {
	return 1;
}

bool TripWireHookBlock::shouldConnectToRedstone(BlockSource& region, const BlockPos& pos, int direction) const {
	return true;
}

int TripWireHookBlock::getResource(Random& random, int data, int bonusLootLevel) const {
	return Block::mTripwireHook->mID;
}

int TripWireHookBlock::getDirection(DataID data) const {
	return getBlockState(BlockState::Direction).get<int>(data);
}

bool TripWireHookBlock::isPowered(DataID data) const {
	return getBlockState(BlockState::PoweredBit).getBool(data);
}

bool TripWireHookBlock::isAttached(DataID data) const {
	return getBlockState(BlockState::AttachedBit).getBool(data);
}
