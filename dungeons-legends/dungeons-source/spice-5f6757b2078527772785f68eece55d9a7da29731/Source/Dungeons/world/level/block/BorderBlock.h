/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

#include "world/level/block/WallBlock.h"

class BorderBlock : public WallBlock {
public:
	BorderBlock(const std::string& nameId, int id);

	virtual std::string buildDescriptionName(DataID data) const override;
	virtual const AABB& getVisualShape(BlockSource& region, const BlockPos& pos, AABB& bufferAABB, bool isClipping) const override;
	virtual bool connectsTo(BlockSource& region, const BlockPos& thisPos, const BlockPos& otherPos) const override;
	virtual int getResourceCount(Random& random, int data, int bonusLootLevel = 0) const override;

	virtual void animateTick(BlockSource& region, const BlockPos& pos, Random& random) const override;

	virtual void onLoaded(BlockSource& region, const BlockPos& pos) const override;
	virtual void onPlace(BlockSource& region, const BlockPos& pos) const override;
	virtual void onRemove(BlockSource& region, const BlockPos& pos) const override;

	virtual bool use(Player& player, const BlockPos& pos) const override;
private:
};
