/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#include "world/level/block/HalfTransparentBlock.h"

HalfTransparentBlock::HalfTransparentBlock(const std::string& nameId, int id, const Material& material, bool allowSame)
	: Block(nameId, id, material) {
	mAllowSame = allowSame;
	setSolid(false);
	mRenderLayer = RENDERLAYER_BLEND;
}
