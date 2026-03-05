/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

#include "world/level/block/EntityBlock.h"

class EnchantingTableBlock : public EntityBlock {
public:
	EnchantingTableBlock(const std::string& nameId, int id);
	bool use(Player& player, const BlockPos& pos) const override;

	void animateTick(BlockSource& region, const BlockPos& pos, Random& random) const override;

	bool isCraftingBlock() const override;

protected:
	virtual bool canBeSilkTouched() const override;

};
