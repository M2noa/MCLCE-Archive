/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

#include "world/level/block/Block.h"

class FallingBlock;
class Color;
class Material;

class HeavyBlock : public Block {
public:
	HeavyBlock(const std::string& nameId, int id, const Material& material );

	void onPlace(BlockSource& region, const BlockPos& pos) const override;

	void triggerFallCheck(BlockSource& region, const BlockPos& pos) const;
	void neighborChanged(BlockSource& region, const BlockPos& pos, const BlockPos& neighborPos) const override;

	void tick(BlockSource& region, const BlockPos& pos, Random& random) const override;

	int getTickDelay() const;

	void animateTick(BlockSource& region, const BlockPos& pos, Random& random) const override;

	virtual Color getDustColor(DataID data) const = 0;

	virtual bool falling() const;
	virtual void onLand(BlockSource& region, const BlockPos& pos) const;

	virtual bool isFree(BlockSource& region, const BlockPos& pos) const;

protected:
	
	void checkSlide(BlockSource& region, const BlockPos& pos) const;
	virtual void startFalling(BlockSource& region, const BlockPos& pos, const Block* oldBlock, bool creative) const;

private:
	BlockPos _findBottomSlidingBlock(BlockSource& region, const BlockPos& pos) const;

	void _scheduleCheck(BlockSource& region, const BlockPos& pos, const Block* oldBlock, int delay = -1) const;
	void _scheduleCheckIfSliding(BlockSource& region, const BlockPos& pos, const Block* oldBlock) const;
	void _tickBlocksAround2D(BlockSource& region, const BlockPos& pos, const Block* oldBlock) const;
};
