/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

#include "world/level/block/Block.h"

class BlockSource;
class Player;
class Random;

class IceBlock : public Block {
public:
	IceBlock(const std::string& nameId, int id, bool packed);

	bool playerWillDestroy(Player& player, const BlockPos& pos, int data) const override;

	int getResourceCount(Random& random, int data, int bonusLootLevel = 0) const override;

	void tick(BlockSource& region, const BlockPos& pos, Random& random) const override;
};
