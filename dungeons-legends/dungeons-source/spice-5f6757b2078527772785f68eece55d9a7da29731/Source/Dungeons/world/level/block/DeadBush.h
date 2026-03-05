/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

#include "world/level/block/BushBlock.h"

class DeadBush : public BushBlock {
public:
	DeadBush(const std::string& nameId, int id);

	bool mayPlaceOn(const Block& block) const override;

	int getResource(Random& random, int data, int bonusLootLevel = 0) const override;
	int getResourceCount(Random& random, int data, int bonusLootLevel = 0) const override;

	void playerDestroy(Player* player, const BlockPos& pos, int data) const override;
};
