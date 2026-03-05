/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#include "world/level/block/DoorBlock.h"
#include "world/level/material/Material.h"
#include "world/level/block/LevelEvent.h"
#include "world/level/Level.h"
#include "world/level/dimension/Dimension.h"
#include "world/item/Item.h"
#include "world/entity/player/Player.h"
// #include "world/entity/ai/village/Villages.h"
#include "world/Facing.h"
#include "world/level/BlockSource.h"
// #include "world/redstone/circuit/CircuitSystem.h"
// #include "world/redstone/circuit/components/ConsumerComponent.h"
#include "world/redstone/Redstone.h"

DoorBlock::DoorBlock(const std::string& nameId, int id, const Material& material, DoorType type)
	: Block(nameId, id, material),
	mType(type) {

	float r = 0.5f;
	float h = 1.0f;
	setVisualShape(Vec3(0.5f - r, 0, 0.5f - r), Vec3(0.5f + r, h, 0.5f + r));

	setSolid(false);
	mRenderLayer = RENDERLAYER_ALPHATEST_SINGLE_SIDE;
	mProperties = BlockProperty::Door | BlockProperty::BreakOnPush;

	setTicking(true);
	mTranslucency[id] = std::max(0.8f, mMaterial.getTranslucency());
}

bool DoorBlock::isUpper(DataID data) const {
	return getBlockState(BlockState::UpperBlockBit).getBool(data);
}

bool DoorBlock::hasRightHinge(BlockSource& region, const BlockPos& pos) const {
	DataID lowerData = 0;
	DataID upperData = 0;
	getDoorData(region, pos, lowerData, upperData);

	return getBlockState(BlockState::DoorHingeBit).getBool(upperData);
}

int DoorBlock::getVariant(int data) const {
	return mType;
}

const AABB& DoorBlock::getVisualShape(BlockSource& region, const BlockPos& pos, AABB& bufferAABB, bool isClipping) const {
	float r = (3.0f / 16.0f) - 0.005f;
	bufferAABB.set(0, 0, 0, 1, 2, 1);
	int dir = getDir(region, pos);
	bool open = isOpen(region, pos);
	bool rightHinge = hasRightHinge(region, pos);
	if (dir == 0) {
		if (open) {
			if (!rightHinge) {
				bufferAABB.set(0, 0, 0, 1, 1, r);
			}
			else {
				bufferAABB.set(0, 0, 1 - r, 1, 1, 1);
			}
		}
		else {
			bufferAABB.set(0, 0, 0, r, 1, 1);
		}
	}
	else if (dir == 1) {
		if (open) {
			if (!rightHinge) {
				bufferAABB.set(1 - r, 0, 0, 1, 1, 1);
			}
			else {
				bufferAABB.set(0, 0, 0, r, 1, 1);
			}
		}
		else {
			bufferAABB.set(0, 0, 0, 1, 1, r);
		}
	}
	else if (dir == 2) {
		if (open) {
			if (!rightHinge) {
				bufferAABB.set(0, 0, 1 - r, 1, 1, 1);
			}
			else {
				bufferAABB.set(0, 0, 0, 1, 1, r);
			}
		}
		else {
			bufferAABB.set(1 - r, 0, 0, 1, 1, 1);
		}
	}
	else if (dir == 3) {
		if (open) {
			if (!rightHinge) {
				bufferAABB.set(0, 0, 0, r, 1, 1);
			}
			else {
				bufferAABB.set(1 - r, 0, 0, 1, 1, 1);
			}
		}
		else {
			bufferAABB.set(0, 0, 1 - r, 1, 1, 1);
		}
	}

	bufferAABB.max.y -= SIZE_OFFSET;

	return bufferAABB;
}

bool DoorBlock::use(Player& player, const BlockPos& pos) const {
	if (mMaterial.isType(MaterialType::Metal)) {
		return true;
	}

	auto& region = player.getRegion();

	bool open = isOpen(region, pos);
	setOpen(region, pos, !open, &player);

	return true;
}

int DoorBlock::getDir(BlockSource& region, const BlockPos& pos) const {
	DataID dataLower = 0;
	DataID dataUpper = 0;
	getDoorData(region, pos, dataLower, dataUpper);

	return getBlockState(BlockState::Direction).get<int>(dataLower);
}

bool DoorBlock::isOpen(BlockSource& region, const BlockPos& pos) const {
	DataID dataLower = 0;
	DataID dataUpper = 0;
	getDoorData(region, pos, dataLower, dataUpper);

	return getBlockState(BlockState::OpenBit).getBool(dataLower);
}

void DoorBlock::setOpen(BlockSource& region, const BlockPos& pos, bool shouldOpen, Mob* eventSource) const {
	DataID lowerData = 0;
	DataID upperData = 0;
	getDoorData(region, pos, lowerData, upperData);

	bool isDoorOpen = isOpen(region, pos);
	if (isDoorOpen == shouldOpen) {
		return;
	}

	BlockPos lowerPos;
	BlockPos upperPos;
	getDoorPosition(region, pos, lowerPos, upperPos);

	getBlockState(BlockState::OpenBit).set(lowerData, !isDoorOpen);

	// bits: dir, dir, open/closed, isUpper
	region.setBlockAndData(lowerPos, mID, lowerData, Block::UPDATE_ALL, eventSource);
	// because we only use 4 bits, the isRightHinge is stored in the first bit of the upper door
	// bits: isRightHinge, 0, 0, isUpper
	//level.fireBlockChanged(upperPos, FullBlock(mID, C_IS_UPPER_MASK | ((compositeData & C_RIGHT_HINGE_MASK) >> 4)), mID, Block::UPDATE_ALL, eventSource);
	region.fireBlockChanged(upperPos, FullBlock(mID, upperData), mID, Block::UPDATE_ALL, eventSource);

	//	Because the top block doesn't actually change any data, we need to fire block updates in all directions but down.
	region.updateNeighborsAtExceptFromFacing(upperPos, upperPos, Facing::DOWN);

// 	region.getLevel().broadcastDimensionEvent(region, LevelEvent::SoundOpenDoor, pos + Vec3(0.5f, 0.5f, 0.5f), 0);

	// Get entire door's aabb
	AABB bufferBB;
	BlockPos secondPos;
	getSecondPart(region, pos, secondPos);

	auto firstBB = getAABB(region, pos, bufferBB);
	auto secondBB = getAABB(region, secondPos, bufferBB);
	auto combinedBB = firstBB.merge(secondBB);

	// Turn one way collision on for entities that get stuck in the door so they don't violently resolve out
// 	const EntityList& entitiesInDoor = region.getEntities(nullptr, combinedBB);
// 	for (Entity* entity : entitiesInDoor)
// 		entity->onOnewayCollision(combinedBB);
}

bool DoorBlock::playerWillDestroy(Player& player, const BlockPos& pos, int data) const {
	BlockPos secondPos;
	getSecondPart(player.getRegion(), pos, secondPos);
	player.getRegion().setBlockAndData(secondPos, FullBlock::AIR, Block::UPDATE_ALL);	// will be replaced by air

	return Block::playerWillDestroy(player, pos, data);
}

void DoorBlock::neighborChanged(BlockSource& region, const BlockPos& pos, const BlockPos& neighborPos) const {
	auto data = region.getData(pos);

	if (!isUpper(data)) {
		if (region.getBlockID(pos.above()) != mID) {
			region.removeBlock(pos);
// 			region.getLevel().broadcastDimensionEvent(region, LevelEvent::ParticlesDestroyBlock, pos, mID + (data << Block::TILE_NUM_SHIFT));
		}

		if (!region.canProvideSupport(pos.below(), Facing::UP, BlockSupportType::Edge)) {
			spawnResources(region, pos, data, 1.0f);
			region.removeBlock(pos);
			if (region.getBlockID(pos.above()) == mID) {
				region.removeBlock(pos.above());
			}
		}
	}
	else {
		if (region.getBlockID(pos.below()) != mID) {
			region.setBlockAndData(pos, FullBlock::AIR, Block::UPDATE_ALL);
// 			region.getLevel().broadcastDimensionEvent(region, LevelEvent::ParticlesDestroyBlock, pos, mID + (data << Block::TILE_NUM_SHIFT));
		}
	}
}

void DoorBlock::onRedstoneUpdate(BlockSource& region, const BlockPos& pos, int strength, bool isFirstTime) const {

	if (!region.getLevel().isClientSide() && !isFirstTime) {
// 		auto &circuit = region.getDimension().getCircuitSystem();
		bool isDoorOpen = isOpen(region, pos);
		bool strengthUpper = false;
		bool strengthBelow = false;

		if (isUpper(region.getData(pos))) {
			strengthUpper = (strength > 0);
// 			strengthBelow = (circuit.getStrength(pos.below()) > 0);
		}
		else {
// 			strengthUpper = (circuit.getStrength(pos.above()) > 0);
			strengthBelow = (strength > 0);
		}

		bool newIsDoorOpen = strengthUpper || strengthBelow;
		if (isDoorOpen != newIsDoorOpen) {
			setOpen(region, pos, newIsDoorOpen, nullptr);
		}
	}

}

static int getItemID(const Block& block) {
	if (block.isType(Block::mWoodenDoor))
		return Item::mDoor_wood->getId();
	if (block.isType(Block::mIronDoor))
		return Item::mDoor_iron->getId();
	if (block.isType(Block::mWoodenDoorAcacia))
		return Item::mDoor_acacia->getId();
	if (block.isType(Block::mWoodenDoorBirch))
		return Item::mDoor_birch->getId();
	if (block.isType(Block::mWoodenDoorDarkOak))
		return Item::mDoor_darkoak->getId();
	if (block.isType(Block::mWoodenDoorJungle))
		return Item::mDoor_jungle->getId();
	if (block.isType(Block::mWoodenDoorSpruce))
		return Item::mDoor_spruce->getId();
	return 0;
}

int DoorBlock::getResource(Random& random, int data, int bonusLootLevel) const {
	return getItemID(*this);
}

ItemInstance DoorBlock::asItemInstance(BlockSource& region, const BlockPos& pos, int blockData) const {
	int itemId = getItemID(*this);
	return ItemInstance(itemId, 1, 0);
}

ItemInstance DoorBlock::getSilkTouchItemInstance(DataID data) const {
	int itemId = getItemID(*this);
	return ItemInstance(itemId, 1, 0);
}

bool DoorBlock::mayPlace(BlockSource& region, const BlockPos& pos) const {
	if (pos.y >= region.getMaxHeight() - 1) {
		return false;
	}

	return region.canProvideSupport(pos.below(), Facing::UP, BlockSupportType::Edge) && Block::mayPlace(region, pos) && Block::mayPlace(region, pos.above());
}

void DoorBlock::onLoaded(BlockSource& region, const BlockPos& pos) const {
// 	if (!region.getLevel().isClientSide()) {
// 		auto &circuit = region.getDimension().getCircuitSystem();
// 		auto consumer = circuit.create<ConsumerComponent>(pos, &region, Facing::DOWN);
// 		if (consumer) {
// 			bool isDoorOpen = isOpen(region, pos);
// 			bool isAttachedDoorOpen = false;
// 
// 			if (isUpper(region.getData(pos))) {
// 				isAttachedDoorOpen = (circuit.getStrength(pos.below()) > 0);
// 			}
// 			else {
// 				isAttachedDoorOpen = (circuit.getStrength(pos.above()) > 0);
// 			}
// 
// 			consumer->setStrength((isDoorOpen || isAttachedDoorOpen) ? Redstone::SIGNAL_MAX : Redstone::SIGNAL_MIN);
// 			consumer->setAcceptHalfPulse(true);
// 		}
// 	}
}

void DoorBlock::onPlace(BlockSource& region, const BlockPos& pos) const {
	Block::onPlace(region, pos);
	onLoaded(region, pos);
}

bool DoorBlock::getSecondPart(BlockSource& region, const BlockPos& pos, BlockPos& out) const {
	auto data = region.getData(pos);
	out = isUpper(data) ? pos.below() : pos.above();

	return true;
}

bool DoorBlock::shouldFlipTexture(BlockSource& region, const BlockPos& pos, FacingID face) const {
	int dir = getDir(region, pos);
	bool isDoorOpen = isOpen(region, pos);
	bool retVal = false;
	if (isDoorOpen) {
		if (dir == 0 && face == Facing::NORTH) {
			retVal = true;
		}
		else if (dir == 1 && face == Facing::EAST) {
			retVal = true;
		}
		else if (dir == 2 && face == Facing::SOUTH) {
			retVal = true;
		}
		else if (dir == 3 && face == Facing::WEST) {
			retVal = true;
		}
	}
	else {
		if (dir == 0 && face == Facing::EAST) {
			retVal = true;
		}
		else if (dir == 1 && face == Facing::SOUTH) {
			retVal = true;
		}
		else if (dir == 2 && face == Facing::WEST) {
			retVal = true;
		}
		else if (dir == 3 && face == Facing::NORTH) {
			retVal = true;
		}

		if (hasRightHinge(region, pos)) {
			retVal = !retVal;
		}

	}

	return retVal;
}

bool DoorBlock::isInteractiveBlock() const {
	return true;
}

bool DoorBlock::canBeSilkTouched() const {
	return false;
}

void DoorBlock::getDoorData(BlockSource& region, const BlockPos& pos, DataID& lowerData, DataID& upperData) const {
	lowerData = region.getData(pos);
	if (getBlockState(BlockState::UpperBlockBit).getBool(lowerData)) {
		upperData = lowerData;
		lowerData = region.getData(pos.below());
	}
	else {
		upperData = region.getData(pos.above());
	}
}

void DoorBlock::getDoorPosition(BlockSource& region, const BlockPos& pos, BlockPos& lowerPos, BlockPos& upperPos) const {
	if (isUpper(region.getData(pos))) {
		lowerPos = pos.below();
		upperPos = pos;
	}
	else {
		lowerPos = pos;
		upperPos = pos.above();
	}
}

bool DoorBlock::checkIsPathable(Entity& entity, const BlockPos& lastPathPos, const BlockPos& pathPos) const {
	return isOpen(entity.getRegion(), pathPos);
}

void DoorBlock::tick(BlockSource& region, const BlockPos& pos, Random& random) const {
	Block::tick(region, pos, random);

	// Assuming doors are always 2 blocks, only submit a query if the block above
	// us is also a door. In this way we will not submit 2 queries for each door.
// 	if (region.getBlockID(pos.below()) != mID && mMaterial.isType(MaterialType::Wood)) {
// 		region.getLevel().getVillages()->submitFindVillageQuery(pos);
// 	}
}

