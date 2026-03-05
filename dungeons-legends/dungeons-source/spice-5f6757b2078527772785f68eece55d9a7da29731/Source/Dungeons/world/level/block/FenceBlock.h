/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

#include "world/level/block/Block.h"

class FenceBlock : public Block {
public:
	FenceBlock(const std::string& nameId, int id, const Material& material);

	const AABB& getVisualShape(BlockSource& region, const BlockPos& pos, AABB& bufferAABB, bool isClipping = false) const override;

	bool connectsTo(BlockSource& region, const BlockPos& thisPos, const BlockPos& otherPos) const;
	const AABB& getAABB(BlockSource& region, const BlockPos& pos, AABB& bufferValue, int optionalData = 0, bool isClipping = false, int clipData = 0) const override;
	bool addCollisionShapes(BlockSource& region, const BlockPos& pos, const AABB* intersectTestBox, std::vector<AABB>& inoutBoxes, Entity* entity) const override;

	virtual bool canProvideSupport(BlockSource& region, const BlockPos& pos, FacingID face, BlockSupportType type) const override;

	DataID getSpawnResourcesAuxValue(DataID data) const override;

	bool use(Player& player, const BlockPos& pos) const override;

	std::string buildDescriptionName(DataID data) const override;

	bool isFenceBlock() const override {
		return true;
	}

protected:
	virtual bool canBeSilkTouched() const override;
};
