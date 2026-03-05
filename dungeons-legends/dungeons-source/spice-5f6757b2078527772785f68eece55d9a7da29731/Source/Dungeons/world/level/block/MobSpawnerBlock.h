/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

#include "world/level/block/EntityBlock.h"

class MobSpawnerBlock : public EntityBlock {
public:
	MobSpawnerBlock(const std::string& nameId, int id);

	int getResourceCount(Random& random, int data, int bonusLootLevel = 0) const override;

	int getExperienceDrop(Random& random) const override;

protected:
	bool canBeSilkTouched() const override;
};
