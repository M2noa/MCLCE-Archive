/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#include "world/level/block/InvisibleBlock.h"
#include "world/level/material/Material.h"

InvisibleBlock::InvisibleBlock(const std::string& nameId, int id, const Material& material) 
	: Block(nameId, id, material) {
	setSolid(false);
	mTranslucency[id] = std::max(0.8f, mMaterial.getTranslucency());
}

HitResult InvisibleBlock::clip(BlockSource& region, const BlockPos& pos, const Vec3& A, const Vec3& B, bool isClipping, int clipData) const {
	return HitResult(B);
}
