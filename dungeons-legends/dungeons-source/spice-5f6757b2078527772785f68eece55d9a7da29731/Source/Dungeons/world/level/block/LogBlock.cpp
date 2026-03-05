/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#include "world/level/block/LogBlock.h"
#include "world/level/material/Material.h"
#include "world/Facing.h"
#include "world/level/block/LeafBlock.h"

LogBlock::LogBlock(const std::string& nameId, int id)
	: RotatedPillarBlock(nameId, id, Material::getMaterial(MaterialType::Wood)){
}

int LogBlock::getResourceCount(Random& random, int data, int bonusLootLevel) const {
	return 1;
}

void LogBlock::onRemove(BlockSource& region, const BlockPos& pos) const {
	LeafBlock::runDecay(region, pos, LeafBlock::REQUIRED_WOOD_RANGE);
	Block::onRemove(region, pos);
}

DataID LogBlock::getSpawnResourcesAuxValue(DataID data) const {
	int type = getBlockState(BlockState::MappedType).get<DataID>(data);

	DataID newData = 0;
	getBlockState(BlockState::MappedType).set(newData, type);

	return newData;
}
