/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#include "world/level/block/ChestBlock.h"
#include "world/level/block/entity/BlockEntity.h"
// #include "world/level/block/entity/ChestBlockEntity.h"
#include "world/level/Level.h"
#include "world/level/dimension/Dimension.h"
#include "world/level/material/Material.h"
#include "world/Facing.h"
// #include "world/entity/item/ItemEntity.h"
#include "world/entity/player/Player.h"
#include "world/level/BlockSource.h"
#include "world/item/Item.h"
#include "world/redstone/Redstone.h"
#include "../SourceDeps/Core/Debug/Log.h"
// #include "world/redstone/circuit/CircuitSystem.h"
// #include "world/redstone/circuit/components/ProducerComponent.h"
// #include "world/level/Spawner.h"

ChestBlock::ChestBlock(const std::string& nameId, int id, ChestBlock::ChestType type, MaterialType materialType)
	: EntityBlock(nameId, id, Material::getMaterial(materialType))
	, mType(type) {

	const float m = 0.025f;
	setVisualShape( Vec3(m, 0, m), Vec3(1 - m, 1 - m - m, 1 - m));

	setSolid(false);
	setPushesOutItems(true);

	mProperties = BlockProperty::Unspecified;
	mBlockEntityType = BlockEntityType::Chest;
}

Block& ChestBlock::init() {
	mTranslucency[mID] = 0.5f;
	return *this;
}

int ChestBlock::getPlacementDataValue(Entity& by, const BlockPos& pos, FacingID face, const Vec3& clickPos, int itemValue) const {
	int dir = (Math::floor(by.getRotation().y * 4 / (360) + 0.5f)) & 3;

	DataID data = 0;

	if (dir == 0) {
		getBlockState(BlockState::FacingDirection).set(data, Facing::NORTH);
	} else if (dir == 1) {
		getBlockState(BlockState::FacingDirection).set(data, Facing::EAST);
	} else if (dir == 2) {
		getBlockState(BlockState::FacingDirection).set(data, Facing::SOUTH);
	} else if (dir == 3) {
		getBlockState(BlockState::FacingDirection).set(data, Facing::WEST);
	}

	return data;
}

void ChestBlock::updateSignalStrength(BlockSource& region, const BlockPos& pos, int strength) const {
// 	if (!region.getLevel().isClientSide() && mType == TYPE_TRAP) {
// 		strength = Math::clamp(strength, Redstone::SIGNAL_NONE, Redstone::SIGNAL_MAX);
// 		 
// 		region.getDimension().getCircuitSystem().setStrength(pos, strength);
// 		
// 		//need to get power for both chest
// 		auto* chest = static_cast<ChestBlockEntity*>(region.getBlockEntity(pos));
// 		if (chest && chest->isLargeChest()) {
// 			region.getDimension().getCircuitSystem().setStrength(chest->getPairedChestPosition(), strength);
// 		}
// 	}
}

bool ChestBlock::hasComparatorSignal() const {
	return true;
}

int ChestBlock::getComparatorSignal(BlockSource& region, const BlockPos& pos, FacingID dir, int data) const {
// 	auto container = (ChestBlockEntity*)region.getBlockEntity(pos);
// 	if (container != nullptr) {
// 		return container->getRedstoneSignalFromContainer();
// 	}

	return EntityBlock::getComparatorSignal(region, pos, dir, data);
}

FacingID ChestBlock::getMappedFace(FacingID face, int data) const {
	if (face == Facing::UP || face == Facing::DOWN) {
		return Facing::UP;
	}

	int facing = getBlockState(BlockState::FacingDirection).get<int>(data);
	return (face == facing) ? Facing::SOUTH : Facing::EAST;
}

void ChestBlock::onLoaded(BlockSource& region, const BlockPos& pos) const {
// 	if (mType == TYPE_TRAP) {
// 		if (!region.getLevel().isClientSide()) {
// 			auto producer = region.getDimension().getCircuitSystem().create<ProducerComponent>(pos, &region, Facing::DOWN);
// 			if (producer) {
// 				producer->setStrength(Redstone::SIGNAL_NONE);
// 				producer->allowAttachments(true);
// 				producer->setAllowPowerUp(true);
// 				producer->setStopPower(true);
// 			}
// 		}
// 	}

	auto chest = region.getBlockAndData(pos);
	int facing = chest.data;
	if (facing != Facing::SOUTH && facing != Facing::WEST && facing != Facing::NORTH && facing != Facing::EAST) {
		// Chest data contains the facing: If we have an invalid facing, set to default (2 = Facing::NORTH)
		chest.data = Facing::NORTH;
		region.setBlockAndData(pos, chest, Block::UPDATE_ALL);
	}
}

bool ChestBlock::shouldConnectToRedstone(BlockSource& region, const BlockPos& pos, int direction) const
{
	return (mType == TYPE_TRAP);
}

void ChestBlock::onPlace(BlockSource& region, const BlockPos& pos) const {
	EntityBlock::onPlace(region, pos);
	onLoaded(region, pos);
}

void ChestBlock::onRemove(BlockSource& region, const BlockPos& pos) const {
// 	ChestBlockEntity* te = (ChestBlockEntity*) region.getBlockEntity(pos);
// 
// 	if (te != nullptr && te->getType() == BlockEntityType::Chest) {
// 		te->unpair();
// 		auto& level = region.getLevel();
// 		if (!level.isClientSide()) {
// 			for (int i = 0; i < te->getContainerSize(); i++) {
// 				ItemInstance* item = te->getItem(i);
// 				if (item != nullptr) {
// 					Random random;
// 					const float xo = random.nextFloat() * 0.8f + 0.1f;
// 					const float yo = random.nextFloat() * 0.8f + 0.1f;
// 					const float zo = random.nextFloat() * 0.8f + 0.1f;
// 
// 					while (!item->isEmptyStack()) {
// 						int count = random.nextInt(21) + 10;
// 						if (count > item->getStackSize()) {
// 							count = item->getStackSize();
// 						}
// 						auto itemEntity = region.getLevel().getSpawner().spawnItem(region, ItemInstance(item->getItem(), count, item->getAuxValue(), item->hasUserData() ? item->getUserData().get() : nullptr), nullptr, Vec3(pos.x + xo, pos.y + yo, pos.z + zo), 10);
// 						float pow = 0.05f;
// 						itemEntity->mPosDelta.x = random.nextGaussian() * pow;
// 						itemEntity->mPosDelta.y = random.nextGaussian() * pow + 0.2f;
// 						itemEntity->mPosDelta.z = random.nextGaussian() * pow;
// 						item->remove(count);
// 					}
// 				}
// 			}
// 		}
// 	}

	EntityBlock::onRemove(region, pos);
}

ChestBlockEntity* _teIfExisting(BlockSource& region, const BlockPos& pos) {
	return (ChestBlockEntity*)region.getBlockEntity(pos);
}

ChestBlockEntity& _te(BlockSource& region, const BlockPos& pos) {
	auto* chest = _teIfExisting(region, pos);
	DEBUG_ASSERT(chest, "Chest missing?");
	return *chest;
}

bool ChestBlock::use(Player& player, const BlockPos& pos) const {
	auto* chest = _teIfExisting(player.getRegion(), pos);
	if (!chest) {
		LOGW("chest tile without entity!!!");
		return false;
	}

	BlockSource& region = player.getRegion();

// 	//check if this chest can open at all
// 	if (player.getLevel().isClientSide() || !chest->canOpen(region)) {
// 		return true;
// 	}
// 
// 	chest->openBy(player);
// 
// 	if (mType == TYPE_TRAP) {
// 		int count = chest->getOpenCount();
// 		if (chest->isLargeChest()) {
// 			auto* chestOther = static_cast<ChestBlockEntity*>(region.getBlockEntity(chest->getPairedChestPosition()));
// 			if (chestOther != nullptr) {
// 				count += chestOther->getOpenCount();
// 			}
// 		}
// 		updateSignalStrength(region, pos, count);
// 	}

	return true;
}

bool ChestBlock::getSecondPart(BlockSource& region, const BlockPos& pos, BlockPos& out) const {
// 	auto* chest = _teIfExisting(region, pos);
// 	if (chest && !chest->isMainSubchest() && chest->isLargeChest() && mType != TYPE_TRAP) {
// 		out = chest->getPairedChestPosition();
// 		return true;
// 	} else{
		return false;
// 	}
}

bool ChestBlock::isContainerBlock() const {
	return true;
}

bool ChestBlock::canBeSilkTouched() const {
	return false;
}

void ChestBlock::onMove(BlockSource& region, const BlockPos& from, const BlockPos& to) const {
// 	auto* chest = _teIfExisting(region, from);
	// Forward message to entity who will do the actual logic
// 	if(chest) {
// 		chest->onMove(region, from, to);
// 	}
}

bool ChestBlock::detachesOnPistonMove(BlockSource& region, const BlockPos& pos) const {
	return true;
}
