/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/
#include "Dungeons.h"

#include "BasePressurePlateBlock.h"
#include "world/level/material/Material.h"
#include "world/level/BlockSource.h"
#include "world/level/Level.h"
#include "world/level/dimension/Dimension.h"
#include "world/entity/Entity.h"
//#include "world/level/BlockTickingQueue.h"
#include "world/redstone/Redstone.h"
#include "world/Facing.h"
// #include "world/redstone/circuit/CircuitSystem.h"
// #include "world/redstone/circuit/components/ProducerComponent.h"

BasePressurePlateBlock::BasePressurePlateBlock(const std::string& nameId, int id, const Material& material) 
	: Block(nameId, id, material) {
	setTicking(false);
	
	setSolid(false);
	mProperties = BlockProperty::PressurePlate | BlockProperty::BreakOnPush;
	mTranslucency[id] = 0.2f;
}

const AABB& BasePressurePlateBlock::getVisualShape(BlockSource& region, const BlockPos& pos, AABB& bufferAABB, bool isClipping) const {
	return getVisualShape(region.getData(pos), bufferAABB, isClipping);
}
const AABB& BasePressurePlateBlock::getVisualShape(DataID data, AABB& bufferAABB, bool isClipping) const {
	bool pressed = getRedstoneSignal(data) > Redstone::SIGNAL_NONE;
	float o = 1 / 16.0f;

	if (pressed) {
		bufferAABB.set( Vec3(o, 0, o), Vec3(1 - o, 0.5f / 16.0f, 1 - o));
	}
	else {
		bufferAABB.set( Vec3(o, 0, o), Vec3(1 - o, 1 / 16.0f, 1 - o));
	}

	return bufferAABB;
}

int BasePressurePlateBlock::getTickDelay() const {
	return SharedConstants::TicksPerSecond;
}


bool BasePressurePlateBlock::getCollisionShape(AABB& outAABB, BlockSource& region, const BlockPos& pos, Entity* entity) const {
	
	outAABB = getVisualShape(region, pos, outAABB, false);

	return true;
}


const AABB& BasePressurePlateBlock::getAABB(BlockSource& region, const BlockPos& pos, AABB& bufferValue, int optionalData, bool isClipping, int clipData) const {
	if (isClipping) {
		getCollisionShape(bufferValue, region, pos, nullptr);
		return bufferValue.move(pos);
	}
	return AABB::EMPTY;
}

bool BasePressurePlateBlock::checkIsPathable(Entity& entity, const BlockPos& lastPathPos, const BlockPos& pathPos) const {
	return true;
}

bool BasePressurePlateBlock::mayPlace(BlockSource& region, const BlockPos& pos) const {
	return region.canProvideSupport(pos.below(), Facing::UP, BlockSupportType::Any);
}

bool BasePressurePlateBlock::isAttachedTo(BlockSource& region, const BlockPos& pos, BlockPos& outAttachedTo) const {
	outAttachedTo = pos + BlockPos(0, -1, 0);
	return true;
}

void BasePressurePlateBlock::tick(BlockSource& region, const BlockPos& pos, Random& random) const {
	if (region.getLevel().isClientSide()) {
		return;
	}

	int oldSignal = getSignalForData(getRedstoneSignal(region.getData(pos)));
	int newSignal = getSignalStrength(region, pos);
	checkPressed(region, pos, oldSignal, newSignal);
}

void BasePressurePlateBlock::entityInside(BlockSource& region, const BlockPos& pos, Entity& entity) const {
	if (region.getLevel().isClientSide()) {
		return;
	}

	if (entity.getEntityTypeId() == EntityType::Experience) {
		return;
	}

	int oldSignal = getSignalForData(getRedstoneSignal(region.getData(pos)));
	int newSignal = getSignalStrength(region, pos);
	if (oldSignal == Redstone::SIGNAL_NONE && newSignal > Redstone::SIGNAL_NONE) {
// 		region.getTickQueue(pos)->remove(pos, mID);
		checkPressed(region, pos, oldSignal, newSignal);
	}
}

void BasePressurePlateBlock::checkPressed(BlockSource& region, const BlockPos& pos, int oldSignal, int newSignal) const {
	bool wasPressed = oldSignal > Redstone::SIGNAL_NONE;
	bool shouldBePressed = newSignal > Redstone::SIGNAL_NONE;

	if (!region.getLevel().isClientSide()) {
// 		region.getDimension().getCircuitSystem().setStrength(pos, newSignal);
	}
	
	DataID data = 0;
	getBlockState(BlockState::RedstoneSignal).set(data, getDataForSignal(newSignal));
	region.setBlockAndData(pos, FullBlock(mID, data), Block::UPDATE_ALL);
	
	if (shouldBePressed != wasPressed) {
// 		region.getLevel().broadcastDimensionEvent(region, LevelEvent::ActivateBlock, pos + Vec3(0.5f, 0.1f, 0.5f), shouldBePressed ? 600 : 500);
	}

	if (shouldBePressed) {
// 		region.getTickQueue( pos )->add(region, pos, mID, getTickDelay());
	}
}

const AABB BasePressurePlateBlock::getSensitiveAABB(const BlockPos& pos) const {
	float b = 1 / 16.0f;
	return AABB((float)pos.x - b, (float)pos.y, (float)pos.z - b, pos.x + 1.0f + b, pos.y + 0.25f, pos.z + 1.0f + b);
}

void BasePressurePlateBlock::onLoaded(BlockSource& region, const BlockPos& pos) const {
	if (!region.getLevel().isClientSide()) {
// 		auto producer = region.getDimension().getCircuitSystem().create<ProducerComponent>(pos, &region, Facing::DOWN);
// 		if (producer) {
// 			producer->setStrength(getSignalStrength(region, pos));
// 			producer->allowAttachments(true);
// 		}
	}
}

void BasePressurePlateBlock::onPlace(BlockSource& region, const BlockPos& pos) const {
	onLoaded(region, pos);
}

void BasePressurePlateBlock::neighborChanged(BlockSource& region, const BlockPos& pos, const BlockPos& neighborPos) const {
	if (!region.getLevel().isClientSide() && !mayPlace(region, pos)) {
		spawnResources(region, pos, 0, 1);
		region.setBlock(pos, BlockID::AIR, UPDATE_ALL);
	}
}

bool BasePressurePlateBlock::shouldConnectToRedstone(BlockSource& region, const BlockPos& pos, int direction) const
{
	return true;
}

int BasePressurePlateBlock::getVariant(int data) const {
	// No variants at this time, so ignore data
	return 0;
}

int BasePressurePlateBlock::getRedstoneSignal(DataID data) const {
	return getBlockState(BlockState::RedstoneSignal).get<int>(data);
}
