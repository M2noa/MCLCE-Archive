/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

#include "world/level/block/Block.h"

class SnowBlock : public Block {
public:
	SnowBlock(const std::string& nameId, int id);

	int getResource(Random& random, int data, int bonusLootLevel = 0) const override;
	int getResourceCount(Random& random, int data, int bonusLootLevel = 0) const override;

	bool getCollisionShape(AABB& outAABB, BlockSource& region, const BlockPos& pos, Entity* entity) const override;

	float calcGroundFriction(Mob& mob, BlockPos const& blockPos) const override;

	virtual bool waterSpreadCausesSpawn() const override {
		return false;
	}
};
