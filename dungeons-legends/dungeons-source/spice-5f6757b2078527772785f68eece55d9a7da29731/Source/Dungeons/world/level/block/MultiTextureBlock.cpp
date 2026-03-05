/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#include "world/level/block/MultiTextureBlock.h"
#include "world/level/material/Material.h"
#include "world/item/ItemInstance.h"
#include "locale/I18n.h"

MultiTextureBlock::MultiTextureBlock(const std::string& nameId, int id, const Material& material) 
	: Block(nameId, id, material) {
}


std::string MultiTextureBlock::buildDescriptionName(DataID data) const {
	std::string ret = mDescriptionId;
	int mappedType = getBlockState(BlockState::MappedType).get<int>(data);

	if (mappedType == enum_cast(Type::Default)) {
		ret += ".default";
	} else if (mappedType == enum_cast(Type::Chiseled)) {	// mossy
		ret += ".chiseled";
	} else if (mappedType == enum_cast(Type::Smooth)) {
		ret += ".smooth";
	} else if (mappedType == enum_cast(Type::Cracked)) {
		ret += ".cracked";
	} else if (mappedType == enum_cast(Type::Mossy)) {
		ret += ".mossy";
	}

	return I18n::get(ret + ".name");
}

DataID MultiTextureBlock::getSpawnResourcesAuxValue(DataID data) const {
	return data;
}
