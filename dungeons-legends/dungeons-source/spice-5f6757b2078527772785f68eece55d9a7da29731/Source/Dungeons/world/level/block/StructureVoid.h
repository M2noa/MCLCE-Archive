/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/

#pragma once

#include "world/level/block/Block.h"

class StructureVoid : public Block {
public:
	enum Type {
		STRUCTURE_VOID = 0,
		STRUCTURE_AIR = 1
	};

	StructureVoid(const std::string& nameId, int id);

	bool isObstructingChests(BlockSource& region, const BlockPos& pos) const override;
	virtual bool isWaterBlocking() const override;

	bool addCollisionShapes(BlockSource& region, const BlockPos& pos, const AABB* intersectTestBox, std::vector<AABB>& inoutBoxes, Entity* entity) const override;
	bool getCollisionShape(AABB& outAABB, BlockSource& region, const BlockPos& pos, Entity* entity) const override;

	bool checkIsPathable(Entity& entity, const BlockPos& lastPathPos, const BlockPos& pathPos) const override;

	void entityInside(BlockSource& region, const BlockPos& pos, Entity& entity) const override;

	bool canHaveExtraData() const override;

protected:
	bool canBeSilkTouched() const override;
};
