/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

#include "world/level/block/Block.h"

class OreBlock : public Block {
public:
	OreBlock(const std::string& nameId, int id);

	int getResource(Random& random, int data, int bonusLootLevel = 0) const override;
	int getResourceCount(Random& random, int data, int bonusLootLevel = 0) const override;

	int getExperienceDrop(Random& random) const override;

protected:
	DataID getSpawnResourcesAuxValue(DataID data) const override;
};
