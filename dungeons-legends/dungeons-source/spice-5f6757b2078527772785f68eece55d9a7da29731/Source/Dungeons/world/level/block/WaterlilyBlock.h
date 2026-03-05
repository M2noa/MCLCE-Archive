/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

#include "world/level/block/BushBlock.h"

class WaterlilyBlock : public BushBlock {
public:
	WaterlilyBlock(const std::string& nameId, int id);

	int getColor(int auxData) const override;
	int getColor(BlockSource& region, const BlockPos& pos) const override;

	bool mayPlaceOn(const Block& block) const override;

	bool canSurvive(BlockSource& region, const BlockPos& pos) const override;

	const AABB& getAABB(BlockSource& region, const BlockPos& pos, AABB& bufferValue, int optionalData = 0, bool isClipping = false, int clipData = 0) const override;

	void entityInside(BlockSource& region, const BlockPos& pos, Entity& entity) const override;

private:
	static const int GUI_COLOR = 0xffffff;	// 0x71c35c;
	static const int WORLD_COLOR = 0x208030;
};
