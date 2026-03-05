/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#include "world/level/block/GlassBlock.h"
#include "world/level/material/Material.h"
#include "world/item/ItemInstance.h"

GlassBlock::GlassBlock(const std::string& nameId, int id, const Material& material, bool allowSame)
	: HalfTransparentBlock(nameId, id, material, allowSame) {
	mRenderLayer = RENDERLAYER_ALPHATEST_SINGLE_SIDE;
	setPushesOutItems(true);
}

int GlassBlock::getResourceCount(Random& random, int data, int bonusLootLevel) const {
	return 0;
}

bool GlassBlock::canProvideSupport(BlockSource& region, const BlockPos& pos, FacingID face, BlockSupportType type) const {
	return true;
}

std::string GlassBlock::buildDescriptionName(DataID data) const {
	// TEMPORARY HACK UNTIL STAINED GLASS COMES IN
	if (mID == mStainedGlass->mID) {
		return mGlass->buildDescriptionName(data);
	}
	return HalfTransparentBlock::buildDescriptionName(data);
}

ItemInstance GlassBlock::asItemInstance(BlockSource& region, const BlockPos& pos, int blockData) const {
	// TEMPORARY HACK UNTIL STAINED GLASS COMES IN
	if (mID == mStainedGlass->mID) {
		return ItemInstance(mGlass);
	}
	return HalfTransparentBlock::asItemInstance(region, pos, blockData);
}

ItemInstance GlassBlock::getSilkTouchItemInstance(DataID data) const {
	// TEMPORARY HACK UNTIL STAINED GLASS COMES IN
	if (mID == mStainedGlass->mID) {
		return ItemInstance(mGlass);
	}
	return HalfTransparentBlock::getSilkTouchItemInstance(data);
}
