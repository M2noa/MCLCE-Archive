/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#include "world/level/block/StoneSlabBlock.h"
#include "world/level/material/Material.h"
#include "world/item/ItemInstance.h"
#include "world/Facing.h"
#include "locale/I18n.h"

const std::string StoneSlabBlock::SLAB_NAMES[] = {
	"stone", "sand", "wood", "cobble", "brick", "smoothStoneBrick", "quartz", "nether_brick"
};
const int StoneSlabBlock::SLAB_NAMES_COUNT = sizeof(StoneSlabBlock::SLAB_NAMES) / sizeof(std::string);

StoneSlabBlock::StoneSlabBlock(const std::string& nameId, int id, bool fullSize)
	:   SlabBlock(nameId, id, fullSize, Material::getMaterial(MaterialType::Stone)) {
	mSlabType = SlabBlock::SlabType::StoneSlab;
}

int StoneSlabBlock::getVariant(int data) const {
	return getBlockState(BlockState::MappedType).get<int>(data);
}

std::string StoneSlabBlock::buildDescriptionName(DataID data) const {
	int type = getBlockState(BlockState::MappedType).get<int>(data);
	if (type < 0 || type > SLAB_NAMES_COUNT) {
		type = 0;
	}

	return I18n::get(mDescriptionId + "." + SLAB_NAMES[type] + ".name");
}

bool StoneSlabBlock::isValidAuxValue(int auxValue) const {
	if (auxValue < enum_cast(StoneSlabType::Stone) || auxValue >= enum_cast(StoneSlabType::_count)) {
		return false;
	}
	return true;
}

int StoneSlabBlock::getResource(Random& random, int data, int bonusLootLevel) const {
	//	This is needed to convert "softly" the old fake wood slabs to real wood slabs.
	int type = getBlockState(BlockState::MappedType).get<int>(data);
	if (type == enum_cast(StoneSlabType::Wood)) {
		return Block::mWoodenSlab->mID;
	} else {
		return Block::mStoneSlab->mID;
	}
}

DataID StoneSlabBlock::getSpawnResourcesAuxValue(DataID data) const {
	DataID type = getBlockState(BlockState::MappedType).get<int>(data);
	return type == enum_cast(StoneSlabType::Wood) ? 0 : type;
}

ItemInstance StoneSlabBlock::asItemInstance(BlockSource& region, const BlockPos& pos, int blockData) const {
	int type = getBlockState(BlockState::MappedType).get<int>(blockData);

	DataID data = 0;
	getBlockState(BlockState::MappedType).set(data, type);

	return ItemInstance(Block::mStoneSlab, 1, data);
}

////////////////////////////// StoneSlabBlock2 //////////////////////////////

const std::string StoneSlabBlock2::SLAB_NAMES[] = {
	"red_sandstone", "purpur"
};
const int StoneSlabBlock2::SLAB_NAMES_COUNT = sizeof(StoneSlabBlock2::SLAB_NAMES) / sizeof(std::string);

StoneSlabBlock2::StoneSlabBlock2(const std::string& nameId, int id, bool fullSize)
	: SlabBlock(nameId, id, fullSize, Material::getMaterial(MaterialType::Stone)) {
	mSlabType = SlabBlock::SlabType::StoneSlab;
}

int StoneSlabBlock2::getVariant(int data) const {
	int type = getBlockState(BlockState::MappedType).get<int>(data);
	return type == enum_cast(StoneSlabType::RedSandstone) ? 0 : 1;
}


std::string StoneSlabBlock2::buildDescriptionName(DataID data) const {
	int type = getBlockState(BlockState::MappedType).get<int>(data);

	if (type < 0 || type > SLAB_NAMES_COUNT) {
		type = 0;
	}

	return I18n::get(mDescriptionId + "." + SLAB_NAMES[type] + ".name");
}

int StoneSlabBlock2::getResource(Random& random, int data, int bonusLootLevel) const {
	return Block::mStoneSlab2->mID;
}

DataID StoneSlabBlock2::getSpawnResourcesAuxValue(DataID data) const {
	DataID type = getBlockState(BlockState::MappedType).get<DataID>(data);
	return type;
}

ItemInstance StoneSlabBlock2::asItemInstance(BlockSource& region, const BlockPos& pos, int blockData) const {
	int type = getBlockState(BlockState::MappedType).get<int>(blockData);

	DataID data = 0;
	getBlockState(BlockState::MappedType).set(data, type);

	return ItemInstance(Block::mStoneSlab2, 1, data);
}
