/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"
#include "world/level/block/PrismarineBlock.h"
#include "world/item/ItemInstance.h"
#include "world/level/material/Material.h"

PrismarineBlock::PrismarineBlock(const std::string& nameId, int id)
	: Block(nameId, id, Material::getMaterial(MaterialType::Stone)) {
	mMapColor = Color(0.0f, 1.0f, 1.0f, 1.0f);
}

int PrismarineBlock::getResource(Random& random, int data, int bonusLootLevel) const {
	return Block::mPrismarine->mID;
}

DataID PrismarineBlock::getSpawnResourcesAuxValue(DataID data) const {
	return data;
}

std::string PrismarineBlock::buildDescriptionName(DataID data) const {
	static const std::string BLOCK_NAMES[NUM_TYPES] = {
		"rough", "dark", "bricks"
	};

	if (data < 0 || data >= NUM_TYPES) {
		data = 0;
	}

	return I18n::get(mDescriptionId + "." + BLOCK_NAMES[data] + ".name");
}
