/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

#include "world/level/block/BaseRailBlock.h"

class DetectorRailBlock : public BaseRailBlock {

public:
	DetectorRailBlock(const std::string& nameId, int id);

	int getTickDelay() const;

	virtual int getVariant(int data) const override;

	void onLoaded(BlockSource& region, const BlockPos& pos) const override;
	void onRedstoneUpdate(BlockSource& region, const BlockPos& pos, int strength, bool isFirstTime) const override;
	void tick(BlockSource& region, const BlockPos& pos, Random& random) const override;
	void entityInside(BlockSource& region, const BlockPos& pos, Entity& entity) const override;
	bool shouldConnectToRedstone(BlockSource& region, const BlockPos& pos, int direction) const override;

	bool hasComparatorSignal() const override;
	int getComparatorSignal(BlockSource& region, const BlockPos& pos, FacingID dir, int data) const override;

private:
	void checkPressed(BlockSource& source, const BlockPos& pos, int state) const;
	void handlePressed(BlockSource& region, const BlockPos& pos, int state, bool shouldBePressed) const;

	AABB getSearchBB(BlockPos pos) const;
};

