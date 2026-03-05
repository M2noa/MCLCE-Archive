/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

#include "world/level/block/Block.h"

enum class WoodBlockType;

class FenceGateBlock : public Block {
public:
	FenceGateBlock(const std::string& nameId, int id, WoodBlockType woodType);

	virtual void onLoaded(BlockSource& region, const BlockPos& pos) const override;
	virtual void onPlace(BlockSource& region, const BlockPos& pos) const override;
	virtual void onRedstoneUpdate(BlockSource& region, const BlockPos& pos, int strength, bool isFirstTime) const override;

	bool mayPlace(BlockSource& region, const BlockPos& pos) const override;

	const AABB& getAABB(BlockSource& region, const BlockPos& pos, AABB& bufferValue, int optionalData = 0, bool isClipping = false, int clipData = 0) const override;
	void neighborChanged(BlockSource& region, const BlockPos& pos, const BlockPos& neighborPos) const override;

	int getPlacementDataValue(Entity& by, const BlockPos& pos, FacingID face, const Vec3& clickPos, int itemValue) const override;

	bool use(Player& player, const BlockPos& pos) const override;

	bool isInteractiveBlock() const override;

private:
	void _checkIsInWall(BlockSource& region, const BlockPos& pos) const;
	void _onOpenChanged(BlockSource& region, const BlockPos& pos) const;

	bool _isOpen(int data) const;
	bool _isInWall(int data) const;
};
