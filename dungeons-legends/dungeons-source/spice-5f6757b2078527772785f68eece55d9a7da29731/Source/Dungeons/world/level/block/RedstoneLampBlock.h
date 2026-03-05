/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

#include "world/level/block/Block.h"

class BlockSource;
class Item;
class ItemInstance;

class RedstoneLampBlock : public Block {
public:
	RedstoneLampBlock(const std::string& nameId, int id, bool isLit);

	virtual void onLoaded(BlockSource& region, const BlockPos& pos) const override;
	virtual void onPlace(BlockSource& region, const BlockPos& pos) const override;

	void tick(BlockSource& region, const BlockPos& pos, Random& random) const override;
	void onRedstoneUpdate(BlockSource& region, const BlockPos& pos, int strength, bool isFirstTime) const override;

	virtual int getResource(Random& random, int data, int bonusLootLevel = 0) const override;
	virtual ItemInstance asItemInstance(BlockSource& region, const BlockPos& pos, int blockData) const override;
	virtual ItemInstance getSilkTouchItemInstance(DataID data) const override;
	bool shouldConnectToRedstone(BlockSource& region, const BlockPos& pos, int direction) const override;
private:

	const bool mIsLit;
};
