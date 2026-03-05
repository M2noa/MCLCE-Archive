/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

#include "world/level/block/Block.h"

class WebBlock : public Block {
public:
	WebBlock(const std::string& nameId, int id);

	void entityInside(BlockSource& region, const BlockPos& pos, Entity& entity) const override;

	const AABB& getAABB(BlockSource& region, const BlockPos& pos, AABB& bufferValue, int optionalData = 0, bool isClipping = false, int clipData = 0) const override;

	int getResource(Random& random, int data, int bonusLootLevel = 0) const override;
};
