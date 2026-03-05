/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

#include "world/level/block/EntityBlock.h"

class EndPortalBlock : public EntityBlock {
public:
	EndPortalBlock(const std::string& nameId, int id);
	virtual bool addCollisionShapes(BlockSource& region, const BlockPos& pos, const AABB* intersectTestBox, std::vector<AABB>& inoutBoxes, Entity* entity) const override;
	virtual void entityInside(BlockSource& region, const BlockPos& pos, Entity& entity) const override;
	virtual void animateTick(BlockSource& region, const BlockPos& pos, Random& random) const override;
	int getResource(Random& random, int data, int bonusLootLevel = 0) const override;
	int getResourceCount(Random& random, int data, int bonusLootLevel = 0) const override;
	bool isWaterBlocking() const override; 
	const AABB& getAABB(BlockSource& region, const BlockPos& pos, AABB& bufferValue, int optionalData = 0, bool isClipping = false, int clipData = 0) const override;
	bool getCollisionShape(AABB& outAABB, BlockSource& region, const BlockPos& pos, Entity* entity) const override;
	void onFallOn(BlockSource& region, const BlockPos& pos, Entity* entity, float fallDistance) const override;
};
