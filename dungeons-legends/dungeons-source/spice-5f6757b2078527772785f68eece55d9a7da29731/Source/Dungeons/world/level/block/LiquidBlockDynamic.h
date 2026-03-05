/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

#include "world/level/block/LiquidBlock.h"

class LiquidBlockDynamic : public LiquidBlock {
public:
	LiquidBlockDynamic(const std::string& nameId, int id, const Material& material);

	void tick(BlockSource& region, const BlockPos& pos, Random& random) const override;

	void onPlace(BlockSource& region, const BlockPos& pos) const override;

private:
	static const int NUM_DIRECTIONS = 4;

	void _setStatic(BlockSource& region, const BlockPos& pos) const;
	bool _canSpreadTo(BlockSource& region, const BlockPos& pos) const;
	bool _isWaterBlocking(BlockSource& region, const BlockPos& pos) const;
	std::array<bool, 4> _getSpread(BlockSource& region, const BlockPos& pos) const;
	void _trySpreadTo(BlockSource& region, const BlockPos& pos, int neighbor) const;
	int _getSlopeDistance(BlockSource& region, const BlockPos& pos, int pass, int from) const;
	void _spread(BlockSource& region, const BlockPos& pos, int depth) const;
	int _getHighest(BlockSource& region, const BlockPos& pos, int current, int& outMaxCount) const;

};
