/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

#include "world/level/block/Block.h"

class TransparentBlock : public Block {
public:
	TransparentBlock(const std::string& nameId, int id, const Material& material, bool allowSame);

	virtual bool shouldRenderFace(BlockSource& region, const BlockPos& pos, FacingID face, const AABB& shape) const override;

protected:
	bool mAllowSame;
};
