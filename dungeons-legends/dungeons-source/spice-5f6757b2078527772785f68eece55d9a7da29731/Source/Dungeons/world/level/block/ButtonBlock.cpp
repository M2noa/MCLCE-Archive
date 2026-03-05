/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#include "world/level/block/ButtonBlock.h"
#include "world/level/BlockSource.h"
#include "world/phys/AABB.h"
#include "world/level/Level.h"
#include "world/level/dimension/Dimension.h"
#include "world/level/material/Material.h"
#include "world/entity/player/Player.h"
#include "world/redstone/Redstone.h"
#include "world/entity/Entity.h"
// #include "world/entity/projectile/Arrow.h"
// #include "world/level/BlockTickingQueue.h"
// #include "world/redstone//circuit/CircuitSystem.h"
// #include "world/redstone/circuit/components/ProducerComponent.h"

bool ButtonBlock::isButtonPressed(int data) const {
	return getBlockState(BlockState::ButtonPressedBit).getBool(data);
}

ButtonBlock::ButtonBlock(const std::string& nameId, int id, bool sensitive)
	: Block(nameId, id, Material::getMaterial(MaterialType::Decoration)), mSensitive(sensitive) {
	setTicking(false);
	setSolid(false);
	mProperties = BlockProperty::BreakOnPush;
}

void ButtonBlock::onPlace(BlockSource& region, const BlockPos& pos) const {
	onLoaded(region, pos);
}

void ButtonBlock::onRedstoneUpdate(BlockSource& region, const BlockPos& pos, int strength, bool isFirstTime) const {
}

void ButtonBlock::onLoaded(BlockSource& region, const BlockPos& pos) const {
	DataID dir = getBlockState(BlockState::FacingDirection).get<DataID>(region.getData(pos));
	FacingID facing = dir;

	//Reset the data on first placement or reload 
	FullBlock fb = region.getBlockAndData(pos);
	getBlockState(BlockState::FacingDirection).set(fb.data, dir);
	region.setBlockAndData(pos, fb, Block::UPDATE_ALL);

// 	if (!region.getLevel().isClientSide()) {
// 		auto producer = region.getDimension().getCircuitSystem().create<ProducerComponent>(pos, &region, Facing::OPPOSITE_FACING[facing]);
// 		if (producer) {
// 			producer->setStrength(Redstone::SIGNAL_NONE);
// 			producer->allowAttachments(true);
// 			producer->setAllowPowerUp(true);
// 		}
// 	}
}

int ButtonBlock::getVariant(int data) const {
	// No variants at this time, so ignore data
	return 0;
}

bool ButtonBlock::getCollisionShape(AABB& outAABB, BlockSource& region, const BlockPos& pos, Entity* entity) const {
	_getShape(outAABB, region.getData(pos), true);
	outAABB.move(pos);
	return true;
}

const AABB& ButtonBlock::getVisualShape(DataID data, AABB& bufferAABB, bool isClipping) const {
	_getShape(bufferAABB, data);
	return bufferAABB;
}

int ButtonBlock::getTickDelay() const {
	return mSensitive ? 30 : 20;
}

bool ButtonBlock::mayPlace(BlockSource& region, const BlockPos& pos, FacingID facing) const {
	return canAttachTo(region, pos, Facing::OPPOSITE_FACING[facing]);
}

bool ButtonBlock::mayPlace(BlockSource& region, const BlockPos& pos) const {
	for (FacingID facing = 0; facing < Facing::MAX; ++facing) {
		if (canAttachTo(region, pos, facing)) {
			return true;
		}
	}

	return false;
}

bool ButtonBlock::canAttachTo(BlockSource& region, const BlockPos& pos, FacingID facing) {
	BlockPos relative = pos.neighbor(facing);
	return region.canProvideSupport(relative, Facing::OPPOSITE_FACING[facing], BlockSupportType::Center);
}

bool ButtonBlock::isAttachedTo(BlockSource& region, const BlockPos& pos, BlockPos& outAttachedTo) const {
	FacingID dir = getBlockState(BlockState::FacingDirection).get<FacingID>(region.getData(pos));
	outAttachedTo = pos - Facing::DIRECTION[dir];
	return true;
}

int ButtonBlock::getPlacementDataValue(Entity& by, const BlockPos& pos, FacingID face, const Vec3& clickPos, int itemValue) const {
	int oldFlip = getBlockState(BlockState::ButtonPressedBit).get<int>(by.getRegion().getData(pos));
	return face + oldFlip;
}

void ButtonBlock::neighborChanged(BlockSource& region, const BlockPos& pos, const BlockPos& neighborPos) const {
	_checkCanSurvive(region, pos);
}

bool ButtonBlock::_checkCanSurvive(BlockSource &region, const BlockPos& pos) const {
	int data = region.getData(pos);
	int dir = getBlockState(BlockState::FacingDirection).get<int>(data);
	if (!mayPlace(region, pos, dir)) {
		spawnResources(region, pos, data, 1);
		region.removeBlock(pos);
		return false;
	}

	return true;
}


bool ButtonBlock::use(Player& player, const BlockPos& pos) const {
// 	BlockSource& region = player.getRegion();
// 
// 	FullBlock fullBlock = region.getBlockAndData(pos);
// 	if (!isButtonPressed(fullBlock.data)) {
// 		_buttonPressed(region, fullBlock, pos, &player);
// 		region.getTickQueue(pos)->add(region, pos, mID, getTickDelay());
// 	}

	return true;
}

void ButtonBlock::tick(BlockSource& region, const BlockPos& pos, Random& random) const {
	if (region.getLevel().isClientSide()) {
		return;
	}

	FullBlock fullBlock = region.getBlockAndData(pos);
	if (!isButtonPressed(fullBlock.data)) {
		return;
	}

	if (mSensitive) {
		_checkPressed(region, pos);
	}
	else {
		_buttonUnpressed(region, fullBlock, pos);
	}
}

void ButtonBlock::entityInside(BlockSource& region, const BlockPos& pos, Entity& entity) const {
	if (!region.getLevel().isClientSide()) {
		if (mSensitive && !isButtonPressed(region.getData(pos))) {
			_checkPressed(region, pos);
		}
	}
}

void ButtonBlock::_checkPressed(BlockSource &region, const BlockPos& pos) const {
	FullBlock fullBlock = region.getBlockAndData(pos);
	bool wasPressed = isButtonPressed(fullBlock.data);

	AABB shape;
	//	@Note: Due to the shape of an arrows AABB, if we're currently pressed, the AABB of the arrow and the pressed button could
	//	potentially not be colliding and causing the button to continuously press and depress. To solve this we want to check the
	//	whole shape of the button vs just the pressed shape because if there would be an arrow within the whole shape than we should
	//	continue to stay pressed.
	_getShape(shape, fullBlock.data, true);

	EntityList entities = region.getEntities(EntityType::Arrow, shape.move(pos));
	bool shouldBePressed = !entities.empty();

	if (shouldBePressed && !wasPressed) {
		_buttonPressed(region, fullBlock, pos, nullptr);
	}
	else if (!shouldBePressed && wasPressed) {
		_buttonUnpressed(region, fullBlock, pos);
	}

// 	if (shouldBePressed) {
// 		region.getTickQueue(pos)->add(region, pos, mID, getTickDelay());
// 	}
}

void ButtonBlock::_buttonPressed(BlockSource& region, FullBlock fb, const Vec3& pos, Entity* presser) const {
// 	getBlockState(BlockState::ButtonPressedBit).set(fb.data, true);
// 	region.setBlockAndData(pos, fb, Block::UPDATE_ALL, presser);
// 
// 	region.getLevel().broadcastDimensionEvent(region, LevelEvent::ActivateBlock, pos + Vec3(0.5f), 600);
// 
// 	if (!region.getLevel().isClientSide()) {
// 		region.getDimension().getCircuitSystem().setStrength(pos, Redstone::SIGNAL_MAX);
// 	}
}

void ButtonBlock::_buttonUnpressed(BlockSource& region, FullBlock fb, const Vec3& pos) const {
// 	getBlockState(BlockState::ButtonPressedBit).set(fb.data, false);
// 	region.setBlockAndData(pos, fb, Block::UPDATE_ALL);
// 
// 	region.getLevel().broadcastDimensionEvent(region, LevelEvent::ActivateBlock, pos + Vec3(0.5f), 500);
// 
// 	if (!region.getLevel().isClientSide()) {
// 		region.getDimension().getCircuitSystem().setStrength(pos, Redstone::SIGNAL_NONE);
// 	}
}

void ButtonBlock::_getShape(AABB& bufferAABB, DataID data, bool ignorePressedState) const {
	int dir = getBlockState(BlockState::FacingDirection).get<int>(data);
	bool pressed = isButtonPressed(data);

	const float depth = (pressed && !ignorePressedState) ? (1 / 16.0f) : (2 / 16.0f);
	const float halfHeight = 2 / 16.0f;
	const float halfWidth = 3 / 16.0f;

	switch (dir) {
	case Facing::EAST:
		bufferAABB.set(0, 0.5f - halfHeight, 0.5f - halfWidth, depth, 0.5f + halfHeight, 0.5f + halfWidth);
		break;
	case Facing::WEST:
		bufferAABB.set(1 - depth, 0.5f - halfHeight, 0.5f - halfWidth, 1, 0.5f + halfHeight, 0.5f + halfWidth);
		break;
	case Facing::SOUTH:
		bufferAABB.set(0.5f - halfWidth, 0.5f - halfHeight, 0, 0.5f + halfWidth, 0.5f + halfHeight, depth);
		break;
	case Facing::NORTH:
		bufferAABB.set(0.5f - halfWidth, 0.5f - halfHeight, 1 - depth, 0.5f + halfWidth, 0.5f + halfHeight, 1);
		break;
	case Facing::UP:
		bufferAABB.set(0.5f - halfWidth, 0, 0.5f - halfHeight, 0.5f + halfWidth, 0 + depth, 0.5f + halfHeight);
		break;
	case Facing::DOWN:
		bufferAABB.set(0.5f - halfWidth, 1 - depth, 0.5f - halfHeight, 0.5f + halfWidth, 1, 0.5f + halfHeight);
		break;
	}
}

ItemInstance ButtonBlock::asItemInstance(BlockSource& region, const BlockPos& pos, int blockData) const {
	return ItemInstance(mBlocks[mID], 1, Facing::EAST);
}

int ButtonBlock::getResourceCount(Random& random, int data, int bonusLootLevel) const {
	return 1;
}

int ButtonBlock::getResource(Random& random, int data, int bonusLootLevel) const {
	return mID;
}

DataID ButtonBlock::getSpawnResourcesAuxValue(DataID data) const {
	return Facing::EAST;
}

bool ButtonBlock::shouldConnectToRedstone(BlockSource& region, const BlockPos& pos, int direction) const {
	return true;
}

bool ButtonBlock::isInteractiveBlock() const {
	return true;
}
