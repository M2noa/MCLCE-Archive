/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"
#include "world/level/block/SeaLanternBlock.h"
#include "world/item/ItemInstance.h"
#include "world/level/material/Material.h"

SeaLanternBlock::SeaLanternBlock(const std::string& nameId, int id)
	: Block(nameId, id, Material::getMaterial(MaterialType::Glass))	{
	mMapColor = Color(1.0f, 1.0f, 1.0f, 1.0f);
}

bool SeaLanternBlock::canBeSilkTouched() const {
	return true;
}

int SeaLanternBlock::getResourceCount(Random& random, int data, int bonusLootLevel) const {
	return 2 + random.nextInt(2);
}

int SeaLanternBlock::getResource(Random& random, int data, int bonusLootLevel) const {
	return Item::mPrismarineCrystals->getId();
}

DataID SeaLanternBlock::getSpawnResourcesAuxValue(DataID data) const {
	return data;
}

std::string SeaLanternBlock::buildDescriptionName(DataID data) const {
	return I18n::get(mDescriptionId + ".name");
}
