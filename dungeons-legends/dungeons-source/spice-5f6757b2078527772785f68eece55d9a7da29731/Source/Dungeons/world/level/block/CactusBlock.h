/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

#include "world/level/block/Block.h"

class Random;

class CactusBlock : public Block {
public:
	CactusBlock(const std::string& nameId, int id);

	void tick(BlockSource& region, const BlockPos& pos, Random& random) const override;

	const AABB& getAABB(BlockSource& region, const BlockPos& pos, AABB& bufferValue, int optionalData = 0, bool isClipping = false, int clipData = 0) const override;

	virtual void onGraphicsModeChanged(bool fancy_, bool preferPolyTessellation, bool transparentLeaves) override;

	bool mayPlace(BlockSource& region, const BlockPos& pos) const override;

	void neighborChanged(BlockSource& region, const BlockPos& pos, const BlockPos& neighborPos) const override;

	bool canSurvive(BlockSource& region, const BlockPos& pos) const override;

	void entityInside(BlockSource& region, const BlockPos& pos, Entity& entity) const override;

protected:
	virtual bool canBeSilkTouched() const override;

};
