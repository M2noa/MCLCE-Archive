/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

#include "world/level/block/Block.h"

class FireBlock : public Block {
public:
	static const int FLAME_INSTANT = 60;
	static const int FLAME_EASY = 30;
	static const int FLAME_MEDIUM = 15;
	static const int FLAME_HARD = 5;

	static const int BURN_INSTANT = 100;
	static const int BURN_EASY = 60;
	static const int BURN_MEDIUM = 20;
	static const int BURN_HARD = 5;
	static const int BURN_NEVER = 0;

	FireBlock(const std::string& nameId, int id);

	const AABB& getAABB(BlockSource& region, const BlockPos& pos, AABB& bufferValue, int optionalData = 0, bool isClipping = false, int clipData = 0) const override;

	int getResourceCount(Random& random, int data, int bonusLootLevel = 0) const override;

	int getTickDelay() const;

	void tick(BlockSource& region, const BlockPos& pos, Random& random) const override;
	void animateTick(BlockSource& region, const BlockPos& pos, Random& random) const override;

	bool mayPick() const override;

	bool canBurn(BlockSource& region, const BlockPos& pos) const;

	int getFlammability(BlockSource& region, const BlockPos& pos, int odds) const;

	bool mayPlace(BlockSource& region, const BlockPos& pos) const override;

	void neighborChanged(BlockSource& region, const BlockPos& pos, const BlockPos& neighborPos) const override;

	void onPlace(BlockSource& region, const BlockPos& pos) const override;

	void ignite(BlockSource& region, const BlockPos& pos);

protected:
	bool canBeSilkTouched() const override;

private:
	void setFlammable(BlockID id, int flame, int burn);

	void checkBurn(BlockSource& region, const BlockPos& pos, int chance, Random& random, int age) const;

	bool isValidFireLocation(BlockSource& region, const BlockPos& pos) const;

	int getFireOdds(BlockSource& region, const BlockPos& pos) const;

	bool tryIgnite(BlockSource& region, const BlockPos& pos);

	AABB mAabb;
	int mFlameOdds[256];
	int mBurnOdds[256];
};
