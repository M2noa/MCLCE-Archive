/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

#include "world/level/block/Block.h"
#include "world/level/material/Material.h"

class BlockSource;
class MaterialBlock;

class BushBlock : public Block {
public:
	static const DataID MAX_GROWTH;

	BushBlock(const std::string& nameId, int id, const Material& material);

	virtual void neighborChanged(BlockSource& region, const BlockPos& pos, const BlockPos& neighborPos) const override;

	virtual void tick(BlockSource& region, const BlockPos& pos, Random& random) const override;

	virtual bool canSurvive(BlockSource& region, const BlockPos& pos) const override;

	virtual const AABB& getAABB(BlockSource& region, const BlockPos& pos, AABB& bufferValue, int optionalData = 0, bool isClipping = false, int clipData = 0) const override;

	void growCrops(BlockSource& region, const BlockPos& pos) const;

protected:
	virtual bool mayPlaceOn(const Block& block) const override;

	virtual void checkAlive(BlockSource& region, const BlockPos& pos) const;
};
