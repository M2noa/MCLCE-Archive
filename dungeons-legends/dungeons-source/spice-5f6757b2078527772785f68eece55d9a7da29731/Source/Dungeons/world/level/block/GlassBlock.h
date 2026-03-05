/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

#include "world/level/block/HalfTransparentBlock.h"

class Material;

class GlassBlock : public HalfTransparentBlock {
public:
	GlassBlock(const std::string& nameId, int id, const Material& material, bool allowSame);

	int getResourceCount(Random& random, int data, int bonusLootLevel = 0) const override;

	virtual bool canProvideSupport(BlockSource& region, const BlockPos& pos, FacingID face, BlockSupportType type) const override;

	virtual ItemInstance asItemInstance(BlockSource& region, const BlockPos& pos, int blockData) const override;
	virtual std::string buildDescriptionName(DataID data) const override;
	virtual ItemInstance getSilkTouchItemInstance(DataID data) const override;

};
