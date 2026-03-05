/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#include "world/level/block/FlowerPotBlock.h"
#include "world/entity/player/Player.h"
#include "world/Facing.h"
// #include "world/level/block/entity/FlowerPotBlockEntity.h"
#include "world/level/BlockSource.h"
#include "world/level/block/TallGrass.h"
#include "world/item/Item.h"
#include "world/level/Level.h"
#include "world/level/material/Material.h"

FlowerPotBlock::FlowerPotBlock(const std::string& nameId, int id) :
	EntityBlock(nameId, id, Material::getMaterial(MaterialType::Decoration) ) {
	//Changing the default shape of the pot.
	float size = 6.f / 16.f;
	float half = size / 2;
	setVisualShape( Vec3(0.5f - half, 0, 0.5f - half), Vec3(0.5f + half, size, 0.5f + half));

	setSolid(false);
	Block::mTranslucency[id] = 0.5f;
	mProperties = BlockProperty::BreakOnPush;
	mRenderLayer = BlockRenderLayer::RENDERLAYER_ALPHATEST;
	mBlockEntityType = BlockEntityType::FlowerPot;
	mTranslucency[id] = std::max(0.8f, mMaterial.getTranslucency());
}

bool FlowerPotBlock::isCubeShaped(void) {
	return false;
}

bool FlowerPotBlock::isSolidRender(void) const {
	return false;
}

bool FlowerPotBlock::use(Player& player, const BlockPos& pos) const {
	if (player.getLevel().isClientSide()) {
		return true;
	}

	auto item = player.getSelectedItem();

	//If player doesn't have an item, he can't use the pot.
	if (item == nullptr) {
		return false;
	}

	//The only plantable items are blocks.
	const Block* block = item->getBlock();
	if (block == nullptr) {
		return false;
	}

	auto &region = player.getRegion();
	//Check to make sure the Pot is able to receive a plant.
// 	auto pot_entity = getFlowerPotEntity(region, pos);
// 	if (pot_entity == nullptr || pot_entity->getPlantItem() != nullptr) {
// 		return false;
// 	}
// 
// 	//Check to see if the block is a supported type
// 	DataID data = static_cast<DataID>(item->getAuxValue());
// 	
// 	int plantType = 0;
// 	if (block && block->getBlockState(BlockState::MappedType).isInitialized()) {
// 		plantType = block->getBlockState(BlockState::MappedType).get<int>(data);
// 	}
// 
// 	if (!isSupportedBlock(block, plantType)) {
// 		return false;
// 	}
// 
// 	pot_entity->setPlantItem(block, plantType);
// 	pot_entity->setChanged();

	//This line updates the region to know it needs to redraw.
	//I used to pass data here, but when data was 0 for certain objects it wouldn't visually update until next the next
	// update call.
	//This data value just says "Something is different", and then it'll draw the updated object in the pot.
	//The correct data value for the block will be pulled from the entity during the tessellation.
	region.setBlockAndData(pos, {Block::mFlowerPot->mID, (DataID)1}, UPDATE_ALL, &player);

	//Java version awards player with the planting flower achievement at this point.
	//Update it here when the system gets ported!

	if (!player.mAbilities.mInstabuild) {
		item->remove(1);
	}

	return true;
}

void FlowerPotBlock::neighborChanged(BlockSource& region, const BlockPos& pos, const BlockPos& neighborPos) const {
	if(!canSurvive(region, pos)) {
// 		spawnResources(region, pos, getFlowerPotEntity(region, pos)->getItemData());
		region.removeBlock(pos);
	}
}

bool FlowerPotBlock::mayPlace(BlockSource& region, const BlockPos& pos) const {
	return mayPlace(region, pos, 0);
}

bool FlowerPotBlock::mayPlace(BlockSource& region, const BlockPos& pos, FacingID face) const {
	return Block::mayPlace(region, pos) && canSurvive(region, pos);
}

bool FlowerPotBlock::canSurvive(BlockSource& region, const BlockPos& pos) const {
	return region.canProvideSupport(pos.below(), Facing::UP, BlockSupportType::Center);
}

bool FlowerPotBlock::isSupportedBlock(const Block* block, int plantType) const {
	if (block->isType(Block::mYellowFlower)
		|| block->isType(Block::mRedFlower)
		|| block->isType(Block::mCactus)
		|| block->isType(Block::mBrownMushroom)
		|| block->isType(Block::mRedMushroom)
		|| block->isType(Block::mSapling)
		|| block->isType(Block::mDeadBush)) {
		return true;
	}
	else if (block->isType(Block::mTallgrass)) {
		return plantType == enum_cast(TallGrassType::Fern);
	}

	return false;
}

FlowerPotBlockEntity* FlowerPotBlock::getFlowerPotEntity(BlockSource& region, const BlockPos& pos) const {
// 	BlockEntity* blockEntity = region.getBlockEntity(pos);
// 	if (blockEntity != nullptr && blockEntity->getType() == BlockEntityType::FlowerPot) {
// 		return static_cast<FlowerPotBlockEntity*>(blockEntity);
// 	}

	return nullptr;
}

bool FlowerPotBlock::playerWillDestroy(Player& player, const BlockPos& pos, int data) const{
// 	if (player.mAbilities.mInstabuild) {
// 		FlowerPotBlockEntity* fpe = getFlowerPotEntity(player.getRegion(), pos);
// 		if (fpe != nullptr) {
// 			fpe->setPlantItem(nullptr, 0);
// 		}
// 	}

	return EntityBlock::playerWillDestroy(player, pos, data);
}

void FlowerPotBlock::onRemove(BlockSource& region, const BlockPos& pos) const{
// 	FlowerPotBlockEntity* fpe = getFlowerPotEntity(region, pos);
// 	if (fpe != nullptr) {
// 		const Block* plant = fpe->getPlantItem();
// 		if (plant != nullptr) {
// 			popResource(region, pos, ItemInstance(plant->mID, 1, fpe->getItemData()));
// 		}
// 	}

	EntityBlock::onRemove(region, pos);
}

int FlowerPotBlock::getResource(Random& random, int data, int bonusLootLevel) const{
	return Item::mFlowerPot->getId();
}

ItemInstance FlowerPotBlock::asItemInstance(BlockSource& region, const BlockPos& pos, int blockData) const {
	return ItemInstance(Item::mFlowerPot);
}

bool FlowerPotBlock::canBeSilkTouched() const {
	return false;
}
