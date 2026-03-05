/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#include "world/level/block/QuartzBlockBlock.h"
#include "world/Facing.h"
#include "world/level/material/Material.h"
#include "world/level/Level.h"
#include "world/item/ItemInstance.h"
#include "locale/I18n.h"

QuartzBlockBlock::QuartzBlockBlock(const std::string& nameId, int id)
	: RotatedPillarBlock(nameId, id, Material::getMaterial(MaterialType::Stone)){
}

QuartzBlockBlock::~QuartzBlockBlock() {
}

int QuartzBlockBlock::getPlacementDataValue(Entity& by, const BlockPos& pos, FacingID face, const Vec3& clickPos, int itemValue) const {
	int type = getBlockState(BlockState::MappedType).get<int>(itemValue);

	// Do not save rotation for regular quartz block
	if(type == enum_cast(Type::Default)) {
		return Block::getPlacementDataValue(by, pos, face, clickPos, itemValue);
	} else {
		return RotatedPillarBlock::getPlacementDataValue(by, pos, face, clickPos, itemValue);
	}
}

std::string QuartzBlockBlock::buildDescriptionName(DataID data) const {
	auto type = getBlockState(BlockState::MappedType).get<Type>(data);
	std::string description = mDescriptionId;

	switch (type) {
	case Type::Chiseled:
		description += ".chiseled";
		break;
	case Type::Lines:
		description += ".lines";
		break;
	default:
		description += ".default";
		break;
	}

	return I18n::get(description + ".name");
}

DataID QuartzBlockBlock::getSpawnResourcesAuxValue(DataID data) const{
	return getBlockState(BlockState::MappedType).get<DataID>(data);
}
