/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

#include "world/level/block/Block.h"

class TntBlock : public Block {
public:
	TntBlock(const std::string& nameId, int id);

	virtual int getResourceCount(Random& random, int data, int bonusLootLevel = 0) const override;

	virtual void onLoaded(BlockSource& region, const BlockPos& pos) const override;
	virtual void onExploded(BlockSource& region, const BlockPos& pos, Entity* entitySource) const override;
	virtual void onRedstoneUpdate(BlockSource& region, const BlockPos& pos, int strength, bool isFirstTime) const override;
	virtual void onPlace(BlockSource& region, const BlockPos& pos) const override;

	virtual void destroy(BlockSource& region, const BlockPos& pos, int data, Entity* entitySource) const override;

	virtual bool use(Player& player, const BlockPos& pos) const override;

	virtual bool canProvideSupport(BlockSource& region, const BlockPos& pos, FacingID face, BlockSupportType type) const override;

	bool dispense(BlockSource& region, Container& container, int slot, const Vec3& pos, FacingID face) const override;

private:
	bool shouldExplode(const DataID& data) const;
};
