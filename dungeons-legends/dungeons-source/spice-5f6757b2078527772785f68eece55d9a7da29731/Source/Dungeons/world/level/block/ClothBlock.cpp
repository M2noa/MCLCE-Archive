/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#include "ClothBlock.h"
#include "world/level/BlockSource.h"
#include "world/level/material/Material.h"

ClothBlock::ClothBlock(const std::string& nameId, int id)
	: Block(nameId, id, Material::getMaterial(MaterialType::Cloth)) {
}

ClothBlock::ClothBlock(const std::string& nameId, int id, int data)
	: Block(nameId, id, Material::getMaterial(MaterialType::Cloth)) {
}

int ClothBlock::getBlockDataForItemAuxValue(int auxValue) {
	int data = Block::mWool->getBlockState(BlockState::MappedType).get<int>(auxValue);
	return 0xF - data;
}

DataID ClothBlock::getSpawnResourcesAuxValue(DataID data) const {
	return data;
}

Color ClothBlock::getMapColor(BlockSource& region, const BlockPos& pos) const {
	static const Color mapColors[16] =
	{ Color::fromARGB(0xffffff) // WHITE
	, Color::fromARGB(0xD87F33) // ORANGE
	, Color::fromARGB(0xB24CD8) // MAGENTA
	, Color::fromARGB(0x6699D8) // LIGHT_BLUE
	, Color::fromARGB(0xE5E533) // YELLOW
	, Color::fromARGB(0x7FCC19) // LIME
	, Color::fromARGB(0xF27FA5) // PINK
	, Color::fromARGB(0x4C4C4C) // GRAY
	, Color::fromARGB(0x999999) // SILVER
	, Color::fromARGB(0x4C7F99) // CYAN
	, Color::fromARGB(0x7F3FB2) // PURPLE
	, Color::fromARGB(0x334CB2) // BLUE
	, Color::fromARGB(0x664C33) // BROWN
	, Color::fromARGB(0x667F33) // GREEN
	, Color::fromARGB(0x993333) // RED
	, Color::fromARGB(0x191919) // BLACK
	};

	DataID data = region.getData(pos);
	int mappedColor = getBlockState(BlockState::MappedType).get<int>(data);
	if (mappedColor < 0 || mappedColor >= 16) {
		return Block::getMapColor(region, pos);
	}

	return mapColors[mappedColor];
}
