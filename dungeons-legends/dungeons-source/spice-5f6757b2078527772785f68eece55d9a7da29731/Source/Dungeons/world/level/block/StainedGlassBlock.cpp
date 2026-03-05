/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/

#include "Dungeons.h"

#include "world/level/block/StainedGlassBlock.h"
#include "world/item/ItemInstance.h"
#include "world/level/material/Material.h"
#include "locale/I18n.h"

StainedGlassBlock::StainedGlassBlock(const std::string& nameId, int id, const Material& material, bool allowSame)
	: HalfTransparentBlock(nameId, id, material, allowSame) {
	mRenderLayer = RENDERLAYER_BLEND;
}

int StainedGlassBlock::getResourceCount(Random& random, int data, int bonusLootLevel) const {
	return 0;
}

DataID StainedGlassBlock::getSpawnResourcesAuxValue(DataID data) const {
	return data;
}

int StainedGlassBlock::getBlockDataForItemAuxValue(int data) {
	return getItemAuxValueForBlockData(data);
}

int StainedGlassBlock::getItemAuxValueForBlockData(int data) {
	return ~data & 0xf;
}

std::string StainedGlassBlock::buildDescriptionName(DataID data) const {
	static const std::string BLOCK_NAMES[16] {
		"white", "orange", "magenta", "lightBlue", "yellow", "lime",
		"pink", "gray", "silver", "cyan", "purple", "blue", "brown",
		"green", "red", "black"
	};

	int mappedType = getBlockState(BlockState::MappedType).get<int>(data);
	if (mappedType < 0 || mappedType >= 16) {
		mappedType = 0;
	}

	return I18n::get(mDescriptionId + "." + BLOCK_NAMES[mappedType] + ".name");
}
