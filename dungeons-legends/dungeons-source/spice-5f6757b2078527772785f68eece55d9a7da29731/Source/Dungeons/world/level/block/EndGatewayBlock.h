/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

#include "world/level/block/EntityBlock.h"

class EndGatewayBlock : public EntityBlock {
public:
	EndGatewayBlock(const std::string& nameId, int id);
	int getResourceCount(Random& random, int data, int bonusLootLevel = 0) const override;
	virtual bool addCollisionShapes(BlockSource& region, const BlockPos& pos, const AABB* intersectTestBox, std::vector<AABB>& inoutBoxes, Entity* entity) const override;
	virtual void animateTick(BlockSource& region, const BlockPos& pos, Random& random) const override;

	virtual bool shouldRenderFace(BlockSource& region, const BlockPos& pos, FacingID face, const AABB& shape) const override;
	virtual bool isWaterBlocking() const override;

	virtual ItemInstance asItemInstance(BlockSource& region, const BlockPos& pos, int blockData) const override;
};
