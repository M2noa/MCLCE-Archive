/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#include "world/level/block/WoodBlock.h"
#include "world/level/material/Material.h"
#include "world/item/ItemInstance.h"
#include "locale/I18n.h"

WoodBlock::WoodBlock(const std::string& nameId, int id)
	: Block(nameId, id, Material::getMaterial(MaterialType::Wood)) {
}

DataID WoodBlock::getSpawnResourcesAuxValue(DataID data) const {
	return data;
}

const std::array<std::string, 6> WoodBlock::WOOD_NAMES = {
	"oak", "spruce", "birch", "jungle", "acacia", "big_oak"
};

const Color WoodBlock::getWoodMaterialColor(WoodBlockType type) {
	static const std::array<Color, 6> sWoodColors =
	{ Color::fromARGB(0x8F7748) // Oak
	, Color::fromARGB(0x815631) // Spruce
	, Color::fromARGB(0xF7E9A3) // Birch
	, Color::fromARGB(0x976D4D) // Jungle
	, Color::fromARGB(0xD87F33) // Acacia
	, Color::fromARGB(0x664C33) // Big_Oak
	};

	return sWoodColors[enum_cast(type)];
}

bool WoodBlock::isValidWoodMaterial(int woodType) {
	if (woodType < enum_cast(WoodBlockType::Oak) || woodType >= enum_cast(WoodBlockType::_count)) {
		return false;
	}
	return true;
}

std::string WoodBlock::buildDescriptionName(DataID data) const {
	int type = getBlockState(BlockState::MappedType).get<int>(data);
	if(type < 0 || type >= 6) {
		type = 0;
	}

	return I18n::get(mDescriptionId + "." + WOOD_NAMES[type] + ".name");
}
