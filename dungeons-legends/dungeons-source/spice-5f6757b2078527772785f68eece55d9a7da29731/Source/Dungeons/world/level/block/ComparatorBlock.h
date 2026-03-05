/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

#include "world/level/block/EntityBlock.h"

class ComparatorCapacitor;

class ComparatorBlock : public EntityBlock {
public:
	ComparatorBlock(const std::string& nameId, int id, bool on);
	
	int getVariant(int data) const override;

	bool shouldRenderFace(BlockSource& region, const BlockPos& pos, FacingID face, const AABB& shape) const override;

	bool use(Player& player, const BlockPos& pos) const override;

	bool isInteractiveBlock() const override;

	void onRedstoneUpdate(BlockSource& region, const BlockPos& pos, int strength, bool isFirstTime) const override;

	void onPlace(BlockSource& region, const BlockPos& pos) const override;
	void onLoaded(BlockSource& region, const BlockPos& pos) const override;

	bool mayPlace(BlockSource& region, const BlockPos& pos) const override;
	bool canSurvive(BlockSource& region, const BlockPos& pos) const override;

	int getPlacementDataValue(Entity& by, const BlockPos& pos, FacingID face, const Vec3& clickPos, int itemValue) const override;

	void triggerEvent(BlockSource& region, const BlockPos& pos, int b0, int b1) const override;

	int getSignal(BlockSource& region, const BlockPos& pos, int dir) const ;
	bool isSignalSource() const override;

	void neighborChanged(BlockSource& region, const BlockPos& pos, const BlockPos& neighborPos) const override;

	int getResource(Random& random, int data, int bonusLootLevel = 0) const override;
	ItemInstance asItemInstance(BlockSource& region, const BlockPos& pos, int blockData) const override;

	bool canBeSilkTouched() const override;

	bool isSubtractMode(BlockSource& region, const BlockPos& pos) const;

	int getDirectSignal(BlockSource& region, const BlockPos& pos, int dir) const override;

	bool shouldConnectToRedstone(BlockSource& region, const BlockPos& pos, int direction) const override;
private:
	void _installCircuit(BlockSource &region, const BlockPos& pos, bool bLoading) const;

	void _refreshOutputState(BlockSource& region, const BlockPos& pos, int strength) const;

	static const int BIT_OUTPUT_SUBTRACT = 0x4;
	static const int BIT_IS_LIT = 0x8;
	bool mOn;
};
