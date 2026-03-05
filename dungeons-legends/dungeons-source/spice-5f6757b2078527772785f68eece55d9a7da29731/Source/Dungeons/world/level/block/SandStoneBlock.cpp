/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#include "world/level/block/SandStoneBlock.h"
#include "world/level/material/Material.h"
#include "world/level/Level.h"
#include "world/item/ItemInstance.h"
#include "world/Facing.h"
#include "locale/I18n.h"

SandStoneBlock::SandStoneBlock(const std::string& nameId, int id)
	: MultiTextureBlock(nameId, id, Material::getMaterial(MaterialType::Stone)) {
	mBrightnessGamma = 0.7f;

}



std::string SandStoneBlock::buildDescriptionName(DataID data) const {
	std::string ret = mDescriptionId;

	auto type = getBlockState(BlockState::MappedType).get<int>(data);
	switch (type) {
	case enum_cast(SandstoneType::Default):
		ret += ".default";
		break;
	case enum_cast(SandstoneType::Heiroglyphs):
		ret += ".chiseled";
		break;
	case enum_cast(SandstoneType::Smoothside):
		ret += ".smooth";
		break;
	}

	return I18n::get(ret + ".name");
}
