/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#include "world/level/block/DispenserBlock.h"
// #include "world/level/block/entity/DispenserBlockEntity.h"
#include "world/level/material/Material.h"
#include "world/level/BlockSource.h"
// #include "world/level/BlockTickingQueue.h"
#include "world/level/dimension/Dimension.h"
#include "world/Container.h"
#include "world/level/Level.h"
#include "world/entity/player/Player.h"
// #include "world/entity/item/ItemEntity.h"
#include "world/item/Item.h"
// #include "world/redstone/circuit/components/ConsumerComponent.h"
// #include "world/redstone/circuit/CircuitSystem.h"
#include "world/redstone/Redstone.h"
// #include "world/level/Spawner.h"
#include "world/Facing.h"

DispenserBlock::DispenserBlock(const std::string& nameId, int id)
	: EntityBlock(nameId, id, Material::getMaterial(MaterialType::Stone)) {
	mBlockEntityType = BlockEntityType::Dispenser;
}

int DispenserBlock::getVariant(int data) const {
	int dir = getBlockState(BlockState::FacingDirection).get<int>(data);

	if (dir == Facing::UP || dir == Facing::DOWN) {
		return 1;
	}
	else {
		return 0;
	}
}

FacingID DispenserBlock::getMappedFace(FacingID face, int data) const {
	int dir = getBlockState(BlockState::FacingDirection).get<int>(data);

	if (face == dir) {
		if (dir == Facing::UP || dir == Facing::DOWN) {
			return Facing::EAST;
		}
		else {
			return Facing::SOUTH;
		}
	}

	if (dir == Facing::UP || dir == Facing::DOWN) {
		return Facing::UP;
	}
	else if (face == Facing::UP || face == Facing::DOWN) {
		return Facing::UP;
	}
	return Facing::NORTH;
}

int DispenserBlock::getPlacementDataValue(Entity& by, const BlockPos& pos, FacingID face, const Vec3& clickPos, int itemValue) const {
	return getPlacementFacingAll(by, pos, 0.0f);
}

void DispenserBlock::onPlace(BlockSource& region, const BlockPos& pos) const {
	EntityBlock::onPlace(region, pos);

	onLoaded(region, pos);
}

void DispenserBlock::onLoaded(BlockSource& region, const BlockPos& pos) const {
// 	if (!region.getLevel().isClientSide()) {
// 		auto &circuit = region.getDimension().getCircuitSystem();
// 		auto consumer = circuit.create<ConsumerComponent>(pos, &region, Facing::DOWN);
// 		if (consumer) {
// 			consumer->setPropagatePower(true);
// 			consumer->setAcceptHalfPulse(true);
// 		}
// 	}
}

void DispenserBlock::onRemove(BlockSource& region, const BlockPos& pos) const {
// 	if (!region.getLevel().isClientSide()) {
// 		Container *container = static_cast<DispenserBlockEntity*>(region.getBlockEntity(pos));
// 		if (container != nullptr) {
// 			Random& random = region.getLevel().getRandom();
// 
// 			for (int i = 0; i < container->getContainerSize(); i++) {
// 				ItemInstance *item = container->getItem(i);
// 				if (item != nullptr && !item->isNull() && item->getStackSize() > 0) {
// 					Vec3 randomOffset = (random.nextVec3() * 0.8f) + 0.1f;
// 
// 					while (item->getStackSize() > 0) {
// 						int count = random.nextInt(21) + 10;
// 						if (count > item->getStackSize()) {
// 							count = item->getStackSize();
// 						}
// 
// 						item->setStackSize(item->getStackSize() - count);
// 
// 						auto itemEntity = region.getLevel().getSpawner().spawnItem(region, ItemInstance(item->getItem(), count, item->getAuxValue(), item->hasUserData() ? item->getUserData().get() : nullptr), nullptr, Vec3(pos) + randomOffset, 10);
// 						float pow = 0.05f;
// 						itemEntity->mPosDelta.x = static_cast<float>(random.nextGaussian()) * pow;
// 						itemEntity->mPosDelta.y = static_cast<float>(random.nextGaussian()) * pow + 0.2f;
// 						itemEntity->mPosDelta.z = static_cast<float>(random.nextGaussian()) * pow;
// 					}
// 				}
// 			}
// 		}
// 
// 		region.getDimension().getCircuitSystem().removeComponents(pos);
// 	}

	EntityBlock::onRemove(region, pos);
}

bool DispenserBlock::use(Player& player, const BlockPos& pos) const {
	if (player.getLevel().isClientSide()) {
		return true;
	}

// 	DispenserBlockEntity *dispenser = static_cast<DispenserBlockEntity*>(player.getRegion().getBlockEntity(pos));
// 
// 	// Sanity check to make sure this is the dispenser type we think it is. There was a bug where these didn't match due to the way the level was saved.
// 	// it's unfortunately unclear how that state was caused (still existing bug? bad build? old bug?) but at least we can handle it gracefully
// 	if(dispenser != nullptr && mBlockEntityType != dispenser->getType()) {
// 		LOGW("Type mismatch, block ID type %i didn't match BlockEntity type %i, destroying block.", mBlockEntityType, dispenser->getType());
// 		player.getRegion().removeBlock(pos);
// 		return true;
// 	}
// 
// 	if (dispenser != nullptr) {
// 		player.openDispenser(dispenser->getContainerSize(), pos, mBlockEntityType == BlockEntityType::Dispenser);
// 	}

	return true;
}

bool DispenserBlock::isInteractiveBlock() const {
	return true;
}

int DispenserBlock::getResource(Random& random, int data, int bonusLootLevel) const {
	return Block::mDispenser->mID;
}

DataID DispenserBlock::getSpawnResourcesAuxValue(DataID data) const {
	return Facing::SOUTH;
}

ItemInstance DispenserBlock::asItemInstance(BlockSource& region, const BlockPos& pos, int blockData) const {
	return ItemInstance(Block::mDispenser, 1, Facing::SOUTH);
}

ItemInstance DispenserBlock::getSilkTouchItemInstance(DataID data) const {
	return ItemInstance(Block::mDispenser, 1, Facing::SOUTH);
}

void DispenserBlock::onRedstoneUpdate(BlockSource& region, const BlockPos& pos, int strength, bool isFirstTime) const {
	if (region.getLevel().isClientSide()) {
		return;
	}

	FullBlock fb = region.getBlockAndData(pos);
	int data = fb.data;

	bool signal = strength > Redstone::SIGNAL_NONE;
	bool isTriggered = getBlockState(BlockState::TriggeredBit).getBool(data);

	if (signal && !isTriggered) {
// 		region.getTickQueue(pos)->add(region, pos, mID, getTickDelay());
		getBlockState(BlockState::TriggeredBit).set(fb.data, true);
		region.setBlockAndDataNoUpdate(pos.x, pos.y, pos.z, fb);
	} else if (!signal && isTriggered) {
		getBlockState(BlockState::TriggeredBit).set(fb.data, false);
		region.setBlockAndData(pos, fb, UPDATE_NONE);
	}
}

bool DispenserBlock::hasComparatorSignal() const {
	return true;
}

int DispenserBlock::getComparatorSignal(BlockSource& region, const BlockPos& pos, FacingID dir, int data) const {
// 	DispenserBlockEntity* container = static_cast<DispenserBlockEntity*>(region.getBlockEntity(pos));
// 	if (container != nullptr) {
// 		return container->getRedstoneSignalFromContainer();
// 	}

	return EntityBlock::getComparatorSignal(region, pos, dir, data);
}

void DispenserBlock::tick(BlockSource& region, const BlockPos& pos, Random& random) const {
	if (!region.getLevel().isClientSide()) {
		dispenseFrom(region, pos);
	}
}

int DispenserBlock::getTickDelay() const {
	return 4;
}

void DispenserBlock::dispenseFrom(BlockSource &region, const BlockPos& pos) const {
// 	DispenserBlockEntity *dispenser = static_cast<DispenserBlockEntity*>(region.getBlockEntity(pos));
// 	if (dispenser == nullptr) {
// 		return;
// 	}
// 
// 	int slot = dispenser->getRandomSlot();
// 	if (slot < 0) {
// 		region.getLevel().broadcastLevelEvent(LevelEvent::SoundClickFail, Vec3(pos), 1200);
// 	} else {
// 		ItemInstance* itemInstance = dispenser->getItem(slot);
// 		if (itemInstance != nullptr) {
// 			bool isDispensed = false;
// 			Vec3 dispensePosition = getDispensePosition(region, pos);
// 			FacingID face = getFacing(region.getData(pos));
// 
// 			if (itemInstance->isBlock()) {
// 				const Block* block = itemInstance->getBlock();
// 				isDispensed = block->dispense(region, *dispenser, slot, dispensePosition, face);
// 			} else {
// 				Item* item = itemInstance->getItem();
// 				isDispensed = item->dispense(region, *dispenser, slot, dispensePosition, face);
// 			}
// 
// 			if (!isDispensed) {
// 				ejectItem(region, dispensePosition, face, itemInstance, *dispenser, slot);
// 			}
// 
// 			region.getLevel().broadcastLevelEvent(LevelEvent::ParticlesShoot, dispensePosition, Facing::STEP_X[face] + 1 + (Facing::STEP_Z[face] + 1) * 3);
// 
// 			dispenser->setChanged();
// 			dispenser->setContainerChanged(slot);
// 		}
// 	}
}

void DispenserBlock::ejectItem(BlockSource& region, const Vec3& pos, FacingID face, ItemInstance* item, Container& container, int slot) const {
	ejectItem(region, pos, face, item);

	container.removeItem(slot, 1);
// 	region.getLevel().broadcastLevelEvent(LevelEvent::SoundClick, pos, 1000);
}

void DispenserBlock::ejectItem(BlockSource& region, const Vec3& pos, FacingID face, ItemInstance* item) {
// 	Vec3 offset = Vec3::ZERO;
// 	Vec3 direction = Vec3((float)Facing::STEP_X[face], (float)Facing::STEP_Y[face], (float)Facing::STEP_Z[face]);
// 
// 	if (face == Facing::UP || face == Facing::DOWN) {
// 		offset.y -= 0.25f / 2;
// 	}
// 	else {
// 		offset.y -= 2.5f / 16;
// 	}
// 
// 	ItemInstance newItem(*item);
// 	newItem.setStackSize(1);
// 
// 	ItemEntity *ejectedItem = region.getLevel().getSpawner().spawnItem(region, newItem, nullptr, pos + offset, SharedConstants::TicksPerSecond * 2);
// 
// 	Random& random = region.getLevel().getRandom();
// 	float pow = random.nextFloat() * 0.1f + 0.2f;
// 
// 	ejectedItem->mPosDelta = Vec3(direction.x * pow, 0.2f, direction.z * pow);
// 	ejectedItem->mPosDelta.x += random.nextGaussian() * 0.0075f * 6;
// 	ejectedItem->mPosDelta.y += random.nextGaussian() * 0.0075f * 6;
// 	ejectedItem->mPosDelta.z += random.nextGaussian() * 0.0075f * 6;
}

Vec3 DispenserBlock::getDispensePosition(BlockSource& region, const Vec3& pos) const {
	const FacingID facing = getFacing(region.getData(pos));

	const float originX = pos.x + 0.5f + 0.7f * Facing::getStepX(facing);
	const float originY = pos.y + 0.3f + 0.7f * Facing::getStepY(facing);
	const float originZ = pos.z + 0.5f + 0.7f * Facing::getStepZ(facing);

	return Vec3(originX, originY, originZ);
}

FacingID DispenserBlock::getFacing(int data) const {
	return Facing::DIRECTIONS[getBlockState(BlockState::FacingDirection).get<int>(data) % 6];
}

void DispenserBlock::recalcLockDir(BlockSource& region, const BlockPos& pos) {
	if (region.getLevel().isClientSide()) {
		return;
	}

	FullBlock fb = region.getBlockAndData(pos);

	bool solidBlockingNorth = region.isSolidBlockingBlock(pos.north());
	bool solidBlockingSouth = region.isSolidBlockingBlock(pos.south());
	bool solidBlockingEast = region.isSolidBlockingBlock(pos.east());
	bool solidBlockingWest = region.isSolidBlockingBlock(pos.west());

	int lockDir = fb.data;
	if (solidBlockingNorth && !solidBlockingSouth) {
		lockDir = 3;
	}
	if (solidBlockingSouth && !solidBlockingNorth) {
		lockDir = 2;
	}
	if (solidBlockingWest && !solidBlockingEast) {
		lockDir = 5;
	}
	if (solidBlockingEast && !solidBlockingWest) {
		lockDir = 4;
	}

	getBlockState(BlockState::FacingDirection).set(fb.data, lockDir);
	region.setBlockAndData(pos, fb, Block::UPDATE_CLIENTS);
}
