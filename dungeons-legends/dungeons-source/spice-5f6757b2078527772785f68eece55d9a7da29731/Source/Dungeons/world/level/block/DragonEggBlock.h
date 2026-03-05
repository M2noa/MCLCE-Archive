/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/

#pragma once

#include "world/level/block/HeavyBlock.h"

class DragonEggBlock : public HeavyBlock {
public:
	DragonEggBlock(const std::string& nameId, int id);

	Color getDustColor(DataID data) const override;

	virtual bool attack(Player* player, const BlockPos& pos) const override;

	virtual bool use(Player& player, const BlockPos& pos) const override;

private:
	void _teleport(BlockSource& region, Random &random, const BlockPos& pos) const;
};
