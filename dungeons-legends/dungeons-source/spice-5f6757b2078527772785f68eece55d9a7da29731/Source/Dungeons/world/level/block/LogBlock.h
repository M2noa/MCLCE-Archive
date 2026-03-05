/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

#include "world/level/block/RotatedPillarBlock.h"

class LogBlock : public RotatedPillarBlock {
public:
	LogBlock(const std::string& nameId, int id);

	int getResourceCount(Random& random, int data, int bonusLootLevel = 0) const override;

	void onRemove(BlockSource& region, const BlockPos& pos) const override;

protected:
	DataID getSpawnResourcesAuxValue(DataID data) const override;
};
