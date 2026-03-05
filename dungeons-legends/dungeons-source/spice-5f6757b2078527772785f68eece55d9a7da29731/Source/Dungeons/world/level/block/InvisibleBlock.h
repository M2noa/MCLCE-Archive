/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

#include "world/level/block/Block.h"

class InvisibleBlock : public Block {
public:
	InvisibleBlock(const std::string& nameId, int id, const Material& material);

	HitResult clip(BlockSource& region, const BlockPos& pos, const Vec3& A, const Vec3& B, bool isClipping = false, int clipData = 0) const override;
};
