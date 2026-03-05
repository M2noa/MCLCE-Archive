/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

#include "world/level/block/LiquidBlock.h"

class LiquidBlockStatic : public LiquidBlock {
public:
	LiquidBlockStatic(const std::string& nameId, int id, BlockID dynamicID, const Material& material);

	void neighborChanged(BlockSource& region, const BlockPos& pos, const BlockPos& neighborPos) const override;

	void tick(BlockSource& region, const BlockPos& pos, Random& random) const override;

private:
	bool _isFlammable(BlockSource& region, const BlockPos& pos);

	void _setDynamic(BlockSource& region, const BlockPos& pos) const;

	BlockID mDynamicID;
};
