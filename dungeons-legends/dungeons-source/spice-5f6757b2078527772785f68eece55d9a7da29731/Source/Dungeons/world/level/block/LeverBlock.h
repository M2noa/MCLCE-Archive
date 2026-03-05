/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

#include "world/level/block/Block.h"

class AABB;
class Player;

class LeverBlock : public Block {
public:
	LeverBlock(const std::string& nameId, int id);

	virtual const AABB& getAABB(BlockSource& region, const BlockPos& pos, AABB& bufferValue, int optionalData = 0, bool isClipping = false, int clipData = 0) const override;
	virtual const AABB& getVisualShape(BlockSource& region, const BlockPos& pos, AABB& bufferAABB, bool isClipping = false) const override;
	virtual const AABB& getVisualShape(DataID data, AABB& bufferAABB, bool isClipping = false) const override;

	virtual bool isInteractiveBlock() const override {
		return true;
	}

	virtual bool mayPlace(BlockSource& region, const BlockPos& pos, FacingID face) const override;
	virtual bool mayPlace(BlockSource& region, const BlockPos& pos) const override;

	virtual int getPlacementDataValue(Entity& by, const BlockPos& pos, FacingID face, const Vec3& clickPos, int itemValue) const override;

	static int getLeverFacing(int facing);

	virtual void neighborChanged(BlockSource& region, const BlockPos& pos, const BlockPos& neighborPos) const override;

	virtual void updateShape(BlockSource *level, const BlockPos& pos);

	virtual bool use(Player& player, const BlockPos& pos) const override;

	virtual void onLoaded(BlockSource& region, const BlockPos& pos) const override;
	virtual void onPlace(BlockSource& region, const BlockPos& pos) const override;
	virtual void onRemove(BlockSource& region, const BlockPos& pos) const override;

	// Redstone Logic
	int getSignal(BlockSource& region, const BlockPos& pos, int dir) const;

	virtual ItemInstance asItemInstance(BlockSource& region, const BlockPos& pos, int blockData) const override;

	virtual int getResourceCount(Random& random, int data, int bonusLootLevel = 0) const override;
	virtual int getResource(Random& random, int data, int bonusLootLevel = 0) const override;

	virtual bool shouldConnectToRedstone(BlockSource& region, const BlockPos& pos, int direction) const override;
	virtual bool isAttachedTo(BlockSource& region, const BlockPos& pos, BlockPos& outAttachedTo) const override;
private:
	void _getShape(BlockSource& region, const BlockPos& pos, AABB& bufferValue) const;
	void _getShape(DataID data, AABB& bufferValue) const;
	bool _checkCanSurvive(BlockSource &region, const BlockPos& pos) const;
	int _getDirectionFromData(int data) const;
	FacingID _getFacingFromData(int data) const;
	FacingID _getOppositeDirectionFromData(int data);
};