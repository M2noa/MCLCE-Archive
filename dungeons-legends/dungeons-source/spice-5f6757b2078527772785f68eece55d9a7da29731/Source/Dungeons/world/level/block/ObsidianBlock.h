/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

#include "world/level/block/Block.h"

class ObsidianBlock : public Block {
public:
	ObsidianBlock(const std::string& nameId, int id, bool isGlowing);

	int getResourceCount(Random& random, int data, int bonusLootLevel = 0) const override;
	int getResource(Random& random, int data, int bonusLootLevel = 0) const override;

	void animateTick(BlockSource& region, const BlockPos& pos, Random& random) const override;

	void poofParticles(BlockSource& region, const BlockPos& pos) const;

private:
	bool mIsGlowing;
};
