/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#include "world/level/block/StoneBlock.h"
#include "world/level/material/Material.h"
#include "world/item/ItemInstance.h"
#include "locale/I18n.h"

StoneBlock::StoneBlock(const std::string& nameId, int id)
	: Block(nameId, id, Material::getMaterial(MaterialType::Stone)){
	
	//TODO it doesn't look bad on andesite and granite, they are striped in nature
	//setAllFacesIsotropic();
}

int StoneBlock::getResource(Random& random, int data, int bonusLootLevel) const {
	int type = getBlockState(BlockState::MappedType).get<int>(data);
	if (type == enum_cast(StoneType::Stone)) {
		return Block::mCobblestone->mID;
	}

	return Block::mStone->mID;
}

DataID StoneBlock::getSpawnResourcesAuxValue(DataID data) const {
	return data;
}

std::string StoneBlock::buildDescriptionName(DataID data) const {
	int type = getBlockState(BlockState::MappedType).get<int>(data);
	static const std::string BLOCK_NAMES[NUM_TYPES] = {
		"stone", "granite", "graniteSmooth", "diorite", "dioriteSmooth", "andesite", "andesiteSmooth"
	};

	if (type < 0 || type >= NUM_TYPES) {
		type = 0;
	}

	return I18n::get(mDescriptionId + "." + BLOCK_NAMES[type] + ".name");
}
