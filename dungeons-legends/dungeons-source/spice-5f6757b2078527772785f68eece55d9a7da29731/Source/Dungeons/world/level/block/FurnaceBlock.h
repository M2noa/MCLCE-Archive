/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

#include "world/level/block/EntityBlock.h"
#include "util/Random.h"

class Mob;
class Player;
class BlockSource;

class FurnaceBlock : public EntityBlock {
public:
	FurnaceBlock(const std::string& nameId, int id, bool lit);

	FacingID getMappedFace(FacingID face, int data) const override;

	void animateTick(BlockSource& region, const BlockPos& pos, Random& random) const override;

	bool use(Player& player, const BlockPos& pos) const override;
	int getResource(Random& random, int data, int bonusLootLevel = 0) const override;

	static void setLit(bool lit, BlockSource& region, const BlockPos& pos);

	int getPlacementDataValue(Entity& by, const BlockPos& pos, FacingID face, const Vec3& clickPos, int itemValue) const override;

	void onPlace(BlockSource& region, const BlockPos& pos) const override;
	void onRemove(BlockSource& region, const BlockPos& pos) const override;

	bool isContainerBlock() const override;
	bool isCraftingBlock() const override;

	bool hasComparatorSignal() const override;
	int getComparatorSignal(BlockSource& region, const BlockPos& pos, FacingID dir, int data) const override;

private:
	const bool mLit;
	static bool mNoDrop;

};
