/********************************************************
*   (c) Mojang. All rights reserved                       *
*   (c) Microsoft. All rights reserved.                   *
*********************************************************/

#pragma once

#include "world/level/block/ChestBlock.h"

class EnderChestBlock : public ChestBlock {
public:
	EnderChestBlock(const std::string& nameId, int id);

	virtual void animateTick(BlockSource& region, const BlockPos& pos, Random& random) const override;

	virtual int getResource(Random& random, int data, int bonusLootLevel = 0) const override;
	virtual int getResourceCount(Random& random, int data, int bonusLootLevel = 0) const override;

protected:
	virtual bool canBeSilkTouched() const override;

private:
};
