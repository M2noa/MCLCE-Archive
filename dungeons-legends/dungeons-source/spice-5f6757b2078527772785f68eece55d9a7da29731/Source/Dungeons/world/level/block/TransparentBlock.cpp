/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#include "world/level/block/TransparentBlock.h"
#include "world/level/BlockSource.h"

TransparentBlock::TransparentBlock(const std::string& nameId, int id, const Material& material, bool allowSame)
	: Block(nameId, id, material) {
	mAllowSame = allowSame;
	setSolid(false);
}

bool TransparentBlock::shouldRenderFace(BlockSource& region, const BlockPos& pos, FacingID face, const AABB& shape) const {
	BlockID id = region.getBlockID(pos);
	if (!mAllowSame && id == mID) {
		return false;
	}

	return Block::shouldRenderFace(region, pos, face, shape);
}
