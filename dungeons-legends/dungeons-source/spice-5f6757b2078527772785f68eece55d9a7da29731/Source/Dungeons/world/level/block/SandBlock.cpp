/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#include "world/level/block/SandBlock.h"
#include "world/level/material/Material.h"
#include "world/item/ItemInstance.h"
#include "locale/I18n.h"
#include "world/level/BlockSource.h"

SandBlock::SandBlock(const std::string& nameId, int id)
	: HeavyBlock(nameId, id, Material::getMaterial(MaterialType::Sand)) {
	mBrightnessGamma = 0.55f;
}

int SandBlock::getVariant(int data) const {
	if (data == enum_cast(SandType::Normal)) {
		return 0;
	}
	else {
		return 1;
	}
}

DataID SandBlock::getSpawnResourcesAuxValue(DataID data) const {
	return data;
}

std::string SandBlock::buildDescriptionName(DataID data) const {
	int type = getBlockState(BlockState::MappedType).get<int>(data);
	if (type == enum_cast(SandType::Normal)) {
		return I18n::get(mDescriptionId + ".default.name");
	} else {
		return I18n::get(mDescriptionId + ".red.name");
	}
}

Color SandBlock::getDustColor(DataID data) const {
	int type = getBlockState(BlockState::MappedType).get<int>(data);
	switch (type) {
	case enum_cast(SandType::Normal):
		return Color::fromARGB(0xffdbd3a0);
	case enum_cast(SandType::Red):
		return Color::fromARGB(0xffa95821);
	default:
		DEBUG_FAIL("Invalid data");
		return Color::NIL;
	}
}

Color SandBlock::getMapColor(BlockSource& region, const BlockPos& pos) const  {
	int type = getBlockState(BlockState::MappedType).get<int>(region.getData(pos));
	if (type == enum_cast(SandType::Red)) {
		return Color::fromARGB(0xD87F33);
	}
	return Color::fromARGB(0xF7E9A3);
}
