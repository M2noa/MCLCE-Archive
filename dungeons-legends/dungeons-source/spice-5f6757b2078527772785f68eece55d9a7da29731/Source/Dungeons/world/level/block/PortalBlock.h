/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

#include "world/level/block/HalfTransparentBlock.h"

class PortalBlock : public HalfTransparentBlock {
public:
	static const int AXIS_UNKNOWN = 0;
	static const int AXIS_X = 1;
	static const int AXIS_Z = 2;

	static const int MIN_WIDTH = 2;
	static const int MAX_WIDTH = 21;
	static const int MIN_HEIGHT = 3;
	static const int MAX_HEIGHT = 21;

	static int getAxis(int data);

	PortalBlock(const std::string& nameId, int id);

	bool mayPick(BlockSource& region, int data, bool liquid) const override;
	void onRemove(BlockSource& region, const BlockPos& pos) const override;

	void tick(BlockSource& region, const BlockPos& pos, Random& random) const override;
	void animateTick(BlockSource& region, const BlockPos& pos, Random& random) const override;

	const AABB& getVisualShape(BlockSource& region, const BlockPos& pos, AABB& bufferAABB, bool isClipping = false) const override;

	const AABB& getAABB(BlockSource& region, const BlockPos& pos, AABB& bufferValue, int optionalData = 0, bool isClipping = false, int clipData = 0) const override;
	void entityInside(BlockSource& region, const BlockPos& pos, Entity& entity) const override;

	void neighborChanged(BlockSource& region, const BlockPos& pos, const BlockPos& neighborPos) const override;

	int getResourceCount(Random& random, int data, int bonusLootLevel = 0) const override;

	bool trySpawnPortal(BlockSource& region, const BlockPos& pos) const;

	ItemInstance asItemInstance(BlockSource& region, const BlockPos& pos, int blockData) const override;

	bool isWaterBlocking() const override;
};
