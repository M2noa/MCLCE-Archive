/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

#include "Block.h"

class BlockPos;
class Level;
class AABB;
 
class Item;
class Set;
class Random;

class RedStoneWireBlock : public Block {
public:
	RedStoneWireBlock(const std::string& nameId, int id);

	virtual const AABB& getAABB(BlockSource& region, const BlockPos& pos, AABB& bufferValue, int optionalData = 0, bool isClipping = false, int clipData = 0) const override;

	virtual bool canSurvive(BlockSource& region, const BlockPos& pos) const override;
	virtual bool mayPlace(BlockSource& region, const BlockPos& pos) const override;

public: 
	virtual void onLoaded(BlockSource& region, const BlockPos& pos) const override;
	virtual void onPlace(BlockSource& region, const BlockPos& pos) const override;
	virtual void onRedstoneUpdate(BlockSource& region, const BlockPos& pos, int strength, bool isFirstTime) const override;
	virtual void neighborChanged(BlockSource& region, const BlockPos& pos, const BlockPos& neighborPos) const override;
	virtual int getColor(BlockSource& region, const BlockPos& pos, DataID auxData) const override;
	virtual int getColor(int data) const override;

	virtual int getResource(Random& random, int data, int bonusLootLevel = 0) const override;
	virtual ItemInstance asItemInstance(BlockSource& region, const BlockPos& pos, int blockData) const override;
	virtual ItemInstance getSilkTouchItemInstance(DataID data) const override;

	virtual void animateTick(BlockSource& region, const BlockPos& pos, Random& random) const override;
	bool shouldConnectToRedstone(BlockSource& region, const BlockPos& pos, int direction) const override;
	static bool shouldConnectTo(BlockSource &region, const BlockPos& pos, int direction);
};
