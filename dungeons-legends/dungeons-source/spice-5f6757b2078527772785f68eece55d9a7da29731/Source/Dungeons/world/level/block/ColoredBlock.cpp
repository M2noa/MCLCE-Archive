/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#include "world/level/BlockSource.h"
#include "world/level/block/ColoredBlock.h"
#include "world/item/ItemInstance.h"
#include "locale/I18n.h"

ColoredBlock::ColoredBlock(const std::string& nameId, int id, const Material& material) 
	: Block(nameId, id, material) {
}

DataID ColoredBlock::getSpawnResourcesAuxValue(DataID data) const {
	return data;
}

int ColoredBlock::getBlockDataForItemAuxValue(int data) {
	return getItemAuxValueForBlockData(data);
}

int ColoredBlock::getItemAuxValueForBlockData(int data) {
	int auxVal = Block::mStainedClay->getBlockState(BlockState::MappedType).get<int>(data);
	return 0xF - auxVal;
}

std::string ColoredBlock::buildDescriptionName(DataID data) const {
	static const std::string BLOCK_NAMES[16]{
		"white", "orange", "magenta", "lightBlue", "yellow", "lime",
		"pink", "gray", "silver", "cyan", "purple", "blue", "brown",
		"green", "red", "black"
	};

	int mappedColor = getBlockState(BlockState::MappedType).get<int>(data);
	if (mappedColor < 0 || mappedColor >= 16) {
		mappedColor = 0;
	}

	return I18n::get(mDescriptionId + "." + BLOCK_NAMES[mappedColor] + ".name");
}

Color ColoredBlock::getMapColor(BlockSource& region, const BlockPos& pos) const {
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
