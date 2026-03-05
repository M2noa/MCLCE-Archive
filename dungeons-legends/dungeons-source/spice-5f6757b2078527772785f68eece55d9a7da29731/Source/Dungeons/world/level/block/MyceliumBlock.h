/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

#include "world/level/block/Block.h"

class MyceliumBlock : public Block {
public:
	static const Brightness MIN_BRIGHTNESS;

	MyceliumBlock(const std::string& nameId, int id);

	void tick(BlockSource& region, const BlockPos& pos, Random& random) const override;
	void animateTick(BlockSource& region, const BlockPos& pos, Random& random) const override;

	int getResource(Random& random, int data, int bonusLootLevel = 0) const override;
};
