/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#include "world/level/block/WoodSlabBlock.h"

#include "world/level/block/WoodBlock.h"
#include "world/level/material/Material.h"
#include "world/level/BlockSource.h"
#include "world/item/ItemInstance.h"
#include "world/entity/player/Player.h"
#include "world/level/Level.h"
#include "world/Facing.h"

// WoodSlabBlock::Item::Item(const std::string & name, int id) :
// 	AuxDataBlockItem(name, id, Block::mWoodenSlab) {
// }
// 
// bool WoodSlabBlock::Item::useOn(ItemInstance& instance, Entity& entity, int x, int y, int z, FacingID face, float clickX, float clickY, float clickZ) {
// 	if(instance.isEmptyStack()) {
// 		return false;
// 	}
// 	//if (!player->mayBuild(x, y, z)) return false;
// 
// 	auto& region = entity.getRegion();
// 
// 	const BlockPos pos(x, y, z);
// 	auto current = region.getBlockAndData(pos);
// 	int type = Block::mWoodenSlab->getBlockState(BlockState::MappedType).get<int>(instance.getAuxValue());
// 	int currentType = Block::mWoodenSlab->getBlockState(BlockState::MappedType).get<int>(current.data);
// 
// 	bool isUpper = !isBottomSlab(current.data);
// 
// 	bool sameWood = (type == currentType);
// 	Block::mWoodenSlab->getBlockState(BlockState::MappedType).set(current.data, type);
// 
// 	//combine
// 	if(((face == Facing::UP && !isUpper) || (face == Facing::DOWN && isUpper)) && current.id == Block::mWoodenSlab->mID && sameWood) {
// 		AABB tmpAABB;
// 		bool unobstructed = region.isUnobstructedByEntities(Block::mDoubleWoodenSlab->getAABB(region, pos, tmpAABB));
// 
// 		if (unobstructed && region.setBlockAndData(pos, {Block::mDoubleWoodenSlab->mID, current.data}, Block::UPDATE_ALL, &entity)) {
// 			region.getLevel().broadcastSoundEvent(region, LevelSoundEvent::ItemUseOn, Vec3(x + 0.5f, y + 0.5f, z + 0.5f), Block::mDoubleWoodenSlab->getId());
// 			entity.useItem(instance);
// 		}
// 
// 		return true;
// 	} else {
// 		//place single
// 		return BlockItem::useOn(instance, entity, x, y, z, face, clickX, clickY, clickZ);
// 	}
// }

WoodSlabBlock::WoodSlabBlock(const std::string& nameId, int id, bool fullSize) :
	SlabBlock(nameId, id, fullSize, Material::getMaterial(MaterialType::Wood)) {
	mSlabType = SlabBlock::SlabType::WoodSlab;
}

int WoodSlabBlock::getVariant(int data) const {
	return getBlockState(BlockState::MappedType).get<int>(data);
}

int WoodSlabBlock::getResource(Random& random, int data, int bonusLootLevel) const {
	return Block::mWoodenSlab->mID;
}

DataID WoodSlabBlock::getSpawnResourcesAuxValue(DataID data) const {
	return getBlockState(BlockState::MappedType).get<int>(data);
}

ItemInstance WoodSlabBlock::asItemInstance(BlockSource& region, const BlockPos& pos, int blockData) const {
	int type = getBlockState(BlockState::MappedType).get<int>(blockData);

	DataID data = 0;
	getBlockState(BlockState::MappedType).set(data, type);

	return ItemInstance(Block::mWoodenSlab, 1, data);
}

bool WoodSlabBlock::isValidAuxValue(int auxValue) const {
	return WoodBlock::isValidWoodMaterial(auxValue);
}

std::string WoodSlabBlock::buildDescriptionName(DataID data) const {
	int type = getBlockState(BlockState::MappedType).get<int>(data);
	if(type < 0 || type >= 7) {
		type = 0;
	}

	static const std::array<std::string, 7> WOOD_NAMES = {
		"oak", "spruce", "birch", "jungle", "acacia", "big_oak"
	};

	return I18n::get(mDescriptionId + "." + WOOD_NAMES[type] + ".name");
}
