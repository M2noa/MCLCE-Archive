/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/

#pragma once

#include "world/level/block/EntityBlock.h"

class StructureBlock : public EntityBlock {
public:
	StructureBlock(const std::string& nameId, int id);

	bool use(Player& player, const BlockPos& pos) const override;

	virtual int getResourceCount(Random& random, int data, int bonusLootLevel = 0) const override;

	virtual void onRedstoneUpdate(BlockSource& region, const BlockPos& pos, int strength, bool isFirstTime) const override;

	void onLoaded(BlockSource& region, const BlockPos& pos) const override;

	ItemInstance asItemInstance(BlockSource& region, const BlockPos& pos, int blockData) const override;

private:
};
