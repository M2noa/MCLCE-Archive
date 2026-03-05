/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#include "world/level/block/BrewingStandBlock.h"
// #include "world/level/block/entity/BrewingStandBlockEntity.h"
#include "world/level/material/Material.h"
// #include "world/entity/item/ItemEntity.h"
#include "world/entity/player/Player.h"
#include "world/item/ItemInstance.h"
#include "world/level/BlockSource.h"
#include "world/level/Level.h"
#include "world/item/Item.h"
// #include "world/entity/ParticleType.h"

// #include "world/level/Spawner.h"

bool BrewingStandBlock::mNoDrop = false;

BrewingStandBlock::BrewingStandBlock(const std::string& nameId, int id)
	: EntityBlock(nameId, id, Material::getMaterial(MaterialType::Metal)) {
	setSolid(false);
	setPushesOutItems(true);
	mProperties = BlockProperty::Unspecified;
	mRenderLayer = BlockRenderLayer::RENDERLAYER_ALPHATEST;
	mBlockEntityType = BlockEntityType::BrewingStand;
	setVisualShape(Vec3::ZERO, Vec3::ONE);
	mTranslucency[id] = std::max(0.8f, mMaterial.getTranslucency());
}

int BrewingStandBlock::getResource(Random& random, int data, int bonusLootLevel) const {
	return Item::mBrewing_stand->getId();
}

ItemInstance BrewingStandBlock::asItemInstance(BlockSource& region, const BlockPos& pos, int blockData) const {
	return ItemInstance(Item::mBrewing_stand);
}

std::string BrewingStandBlock::buildDescriptionName(DataID data) const {
	return I18n::get("item.brewing_stand.name");
}

void BrewingStandBlock::addAABBs(BlockSource& region, const BlockPos& pos, const AABB* intersectTestBox, std::vector<AABB>& inoutBoxes) const {
	AABB shape(Vec3(7.0f / 16.0f, 0, 7.0f / 16.0f), Vec3(9.0f / 16.0f, 14.0f / 16.0f, 9.0f / 16.0f));
	Block::addAABB(shape.move(pos), intersectTestBox, inoutBoxes);
	shape.set(Vec3::ZERO, Vec3(1, 2.0f / 16.0f, 1));
	Block::addAABB(shape.move(pos), intersectTestBox, inoutBoxes);
}

bool BrewingStandBlock::addCollisionShapes( BlockSource& region, const BlockPos& pos, const AABB* intersectTestBox, std::vector<AABB>& inoutBoxes, Entity* entity ) const {
	addAABBs(region, pos, intersectTestBox, inoutBoxes);
	return true;
}

void BrewingStandBlock::animateTick(BlockSource& region, const BlockPos& pos, Random& random) const {
	const float x = pos.x + 0.4f + random.nextFloat() * .2f;
	const float y = pos.y + 0.7f + random.nextFloat() * .3f;
	const float z = pos.z + 0.4f + random.nextFloat() * .2f;

	// TODO: rherlitz
// 	region.getLevel().addParticle(ParticleType::Smoke, Vec3(x, y, z), Vec3::ZERO);
}

bool BrewingStandBlock::use(Player& player, const BlockPos& pos) const {
	if(player.getLevel().isClientSide() ) {
		return true;
	}

// 	BrewingStandBlockEntity* stand = static_cast<BrewingStandBlockEntity*>(player.getRegion().getBlockEntity(pos));

// 	if(stand != nullptr) {
// 		player.openBrewingStand(stand->getContainerSize(), stand->getPosition());
// 	}

	return true;
}

void BrewingStandBlock::onRemove(BlockSource& region, const BlockPos& pos) const {
// 	auto& level = region.getLevel();
// 	if(!mNoDrop && !level.isClientSide()) {
// 		BrewingStandBlockEntity* container = (BrewingStandBlockEntity*)region.getBlockEntity(pos);
// 		if(container != nullptr) {
// 			for(int i = 0; i < container->getContainerSize(); i++) {
// 				ItemInstance* item = container->getItem(i);
// 				if(item) {
// 					Random random;
// 					const float xo = random.nextFloat() * 0.8f + 0.1f;
// 					const float yo = random.nextFloat() * 0.8f + 0.1f;
// 					const float zo = random.nextFloat() * 0.8f + 0.1f;
// 
// 					if(!item->isEmptyStack()) {
// 						auto stackSize = item->getStackSize();
// 						ItemEntity *newItem = region.getLevel().getSpawner().spawnItem(region, ItemInstance(item->getItem(), stackSize, item->getAuxValue(), item->hasUserData() ? item->getUserData().get() : nullptr), nullptr, Vec3(pos.x + xo, pos.y + yo, pos.z + zo), 10);
// 						const float pow = 0.05f;
// 						newItem->mPosDelta.x = random.nextGaussian() * pow;
// 						newItem->mPosDelta.y = random.nextGaussian() * pow + 0.2f;
// 						newItem->mPosDelta.z = random.nextGaussian() * pow;
// 						item->remove(stackSize);
// 					}
// 				}
// 			}
// 		}
// 	}

	EntityBlock::onRemove(region, pos);
}

bool BrewingStandBlock::isContainerBlock() const {
	return true;
}

bool BrewingStandBlock::isCraftingBlock() const {
	return true;
}

bool BrewingStandBlock::hasComparatorSignal() const {
	return true;
}

int BrewingStandBlock::getComparatorSignal(BlockSource& region, const BlockPos& pos, FacingID dir, int data) const {
// 	auto container = (BrewingStandBlockEntity*)region.getBlockEntity(pos);
// 	if (container != nullptr) {
// 		return container->getRedstoneSignalFromContainer();
// 	}

	return EntityBlock::getComparatorSignal(region, pos, dir, data);
}

bool BrewingStandBlock::canBeSilkTouched() const {
	return false;
}
