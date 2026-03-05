/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

#include "Block.h"

class SoulSandBlock : public Block {
public:
	SoulSandBlock(const std::string& nameId, int id);
	const AABB& getAABB(BlockSource& region, const BlockPos& pos, AABB& bufferValue, int optionalData = 0, bool isClipping = false, int clipData = 0) const override;
	void entityInside(BlockSource& region, const BlockPos& pos, Entity& entity) const override;
	virtual bool checkIsPathable(Entity& entity, const BlockPos& lastPathPos, const BlockPos& pathPos) const override;
};
