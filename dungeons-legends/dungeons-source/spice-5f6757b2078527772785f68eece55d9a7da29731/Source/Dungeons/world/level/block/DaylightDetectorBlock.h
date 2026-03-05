/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

#include "world/level/block/EntityBlock.h"

class BlockSource;

class DaylightDetectorBlock : public EntityBlock {

public:
	DaylightDetectorBlock(const std::string& nameId, int id, bool isInverted);

    virtual void updateShape(BlockSource& level, const BlockPos& pos);

	virtual void onLoaded(BlockSource& region, const BlockPos& pos) const override;
    virtual void onPlace(BlockSource& region, const BlockPos& pos) const override;
	virtual void onRemove(BlockSource& region, const BlockPos& pos) const override;

    virtual void updateSignalStrength(BlockSource& level, const BlockPos& pos) const;
	virtual bool use(Player& player, const BlockPos& pos) const override;

	virtual bool shouldConnectToRedstone(BlockSource& region, const BlockPos& pos, int direction) const override;

	virtual bool isInteractiveBlock() const override {
		return true;
	}
	
	virtual int getVariant(int data) const override;

	virtual int getResource(Random& random, int data, int bonusLootLevel = 0) const override;
	virtual ItemInstance asItemInstance(BlockSource& region, const BlockPos& pos, int blockData) const override;
	virtual ItemInstance getSilkTouchItemInstance(DataID data) const override;

private:
	bool mIsInverted;

};
