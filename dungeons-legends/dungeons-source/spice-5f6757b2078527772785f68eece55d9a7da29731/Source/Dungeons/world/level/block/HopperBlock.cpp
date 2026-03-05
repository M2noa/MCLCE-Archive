/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#include "world/level/block/HopperBlock.h"
// #include "world/level/block/entity/HopperBlockEntity.h"
#include "world/Container.h"
#include "world/item/ItemInstance.h"
// #include "world/entity/item/ItemEntity.h"
#include "world/level/material/Material.h"
#include "client/renderer/texture/TextureAtlas.h"
#include "world/item/Item.h"
#include "world/level/dimension/Dimension.h"
#include "world/level/Level.h"
#include "world/level/BlockSource.h"
#include "world/entity/player/Player.h"
#include "world/Facing.h"
// #include "world/redstone/circuit/CircuitSystem.h"
// #include "world/redstone/circuit/components/ConsumerComponent.h"
// #include "world/level/Spawner.h"

int HopperBlock::getAttachedFace(int data) {
	int facingDir = Block::mHopper->getBlockState(BlockState::FacingDirection).get<int>((DataID)data);
	return facingDir;
}

bool HopperBlock::isTurnedOn(int data) {
	bool bToggleEnabled = Block::mHopper->getBlockState(BlockState::ToggleBit).getBool((DataID)data);
	return !bToggleEnabled;
}

// HopperBlockEntity* HopperBlock::getHopperBlockEntity(BlockSource& region, const BlockPos& pos) {
// 	return static_cast<HopperBlockEntity*>(region.getBlockEntity(pos));
// }

HopperBlock::HopperBlock(const std::string& nameId, int id)
	: EntityBlock(nameId, id, Material::getMaterial(MaterialType::Metal)) {
	setVisualShape(Vec3::ZERO, Vec3::ONE);
	setSolid(false);
	mBlockEntityType = BlockEntityType::Hopper;
	mProperties = BlockProperty::Hopper | BlockProperty::TopSolidBlocking;
	mRenderLayer = BlockRenderLayer::RENDERLAYER_ALPHATEST_SINGLE_SIDE;
	mTranslucency[id] = std::max(0.8f, mMaterial.getTranslucency());
}

void HopperBlock::onPlace(BlockSource& region, const BlockPos& pos) const {
	EntityBlock::onPlace(region, pos);

	_installCircuit(region, pos);
}

void HopperBlock::onLoaded(BlockSource& region, const BlockPos& pos) const {
	EntityBlock::onLoaded(region, pos);

	_installCircuit(region, pos);

// 	if (!region.getLevel().isClientSide()) {
// 		HopperBlockEntity* hopper = static_cast<HopperBlockEntity*>(region.getBlockEntity(pos));
// 		if (hopper != nullptr) {
// 			hopper->checkForSmeltEverythingAchievement(region);
// 		}
// 	}
}

void HopperBlock::onRemove(BlockSource& region, const BlockPos& pos) const {
// 	if (!region.getLevel().isClientSide()) {
// 		Container* container = static_cast<HopperBlockEntity*>(region.getBlockEntity(pos));
// 		if (container != nullptr) {
// 			Random& random = region.getLevel().getRandom();
// 
// 			for (int i = 0; i < container->getContainerSize(); i++) {
// 				ItemInstance *item = container->getItem(i);
// 				if (item != nullptr && !item->isNull() && item->getStackSize() > 0) {
// 					float xo = random.nextFloat() * 0.8f + 0.1f;
// 					float yo = random.nextFloat() * 0.8f + 0.1f;
// 					float zo = random.nextFloat() * 0.8f + 0.1f;
// 
// 					while (item->getStackSize() > 0) {
// 						int count = random.nextInt(21) + 10;
// 						if (count > item->getStackSize()) {
// 							count = item->getStackSize();
// 						}
// 
// 						item->setStackSize(item->getStackSize() - count);
// 
// 						auto itemEntity = region.getLevel().getSpawner().spawnItem(region, ItemInstance(item->getItem(), count, item->getAuxValue(), item->hasUserData() ? item->getUserData().get() : nullptr), nullptr, Vec3(pos.x + xo, pos.y + yo, pos.z + zo), 10);
// 
// 						float pow = 0.05f;
// 						float offx = random.nextGaussian() * pow;
// 						float offy = random.nextGaussian() * pow + 0.2f;
// 						float offz = random.nextGaussian() * pow;
// 						itemEntity->mPosDelta = Vec3(offx, offy, offz);
// 					}
// 				}
// 			}
// 		}
// 
// 		region.getDimension().getCircuitSystem().removeComponents(pos);
// 	}

	EntityBlock::onRemove(region, pos);
}

void HopperBlock::onRedstoneUpdate(BlockSource& region, const BlockPos& pos, int strength, bool isFirstTime) const {
	FullBlock fb = region.getBlockAndData(pos);

	bool shouldBeOn = strength == 0;
	bool isOn = isTurnedOn(fb.data);

	if (shouldBeOn != isOn) {
// 		auto& circuit = region.getDimension().getCircuitSystem();
// 		circuit.lockGraph(true);
		getBlockState(BlockState::ToggleBit).set(fb.data, shouldBeOn ? false : true);
		region.setBlockAndData(pos, fb, UPDATE_NONE);
// 		circuit.lockGraph(false);
	}
}

bool HopperBlock::use(Player& player, const BlockPos& pos) const {
	BlockSource& region = player.getRegion();

	if (region.getLevel().isClientSide()) {
		return true;
	}

// 	BlockEntity* blockEntity = player.getRegion().getBlockEntity(pos);
// 	if (blockEntity != nullptr && blockEntity->isType(BlockEntityType::Hopper)) {
// 		player.openHopper(static_cast<HopperBlockEntity*>(blockEntity)->getContainerSize(), blockEntity->getPosition());
// 	}

	return true;
}

bool HopperBlock::isInteractiveBlock() const {
	return true;
}

bool HopperBlock::addCollisionShapes(BlockSource& region, const BlockPos& pos, const AABB* intersectTestBox, std::vector<AABB>& inoutBoxes, Entity* entity) const {
	addAABBs(region, pos, intersectTestBox, inoutBoxes);
	return true;
}

void HopperBlock::addAABBs(BlockSource& region, const BlockPos& pos, const AABB* intersectTestBox, std::vector<AABB>& inoutBoxes) const {
	AABB shape(Vec3::ZERO, Vec3(1, 10.0f / 16.0f, 1));
	Block::addAABB(shape.move(pos), intersectTestBox, inoutBoxes);
	float thickness = 2.0f / 16.0f;
	shape.set(Vec3::ZERO, Vec3(thickness, 1, 1));
	Block::addAABB(shape.move(pos), intersectTestBox, inoutBoxes);
	shape.set(Vec3::ZERO, Vec3(1, 1, thickness));
	Block::addAABB(shape.move(pos), intersectTestBox, inoutBoxes);
	shape.set(Vec3(1 - thickness, 0, 0), Vec3::ONE);
	Block::addAABB(shape.move(pos), intersectTestBox, inoutBoxes);
	shape.set(Vec3(0, 0, 1 - thickness), Vec3::ONE);
	Block::addAABB(shape.move(pos), intersectTestBox, inoutBoxes);
}

bool HopperBlock::canProvideSupport(BlockSource& region, const BlockPos& pos, FacingID face, BlockSupportType type) const {
	return face == Facing::UP;
}

int HopperBlock::getPlacementDataValue(Entity& by, const BlockPos& pos, FacingID face, const Vec3& clickPos, int itemValue) const {
	int attached = Facing::OPPOSITE_FACING[face];
	if (attached == Facing::UP) {
		attached = Facing::DOWN;
	}

	DataID data = 0;
	getBlockState(BlockState::FacingDirection).set(data, attached);

	return data;
}

bool HopperBlock::hasComparatorSignal() const {
	return true;
}

int HopperBlock::getComparatorSignal(BlockSource& region, const BlockPos& pos, FacingID dir, int data) const {
// 	auto container = static_cast<HopperBlockEntity*>(region.getBlockEntity(pos));
// 	if (container != nullptr) {
// 		return container->getRedstoneSignalFromContainer();
// 	}

	return Block::getComparatorSignal(region, pos, dir, data);
}

int HopperBlock::getResource(Random& random, int data, int bonusLootLevel) const {
	return Item::mHopper->getId();
}

ItemInstance HopperBlock::asItemInstance(BlockSource& region, const BlockPos& pos, int blockData) const {
	return ItemInstance(Item::mHopper, 1, blockData);
}

bool HopperBlock::canBeSilkTouched() const {
	return false;
}

void HopperBlock::_installCircuit(BlockSource& region, const BlockPos& pos) const {
// 	auto consumer = region.getDimension().getCircuitSystem().create<ConsumerComponent>(pos, &region, Facing::DOWN);
// 	if (consumer != nullptr) {
// 		consumer->setAllowPowerUp(true);
// 	}
}
