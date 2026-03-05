/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

#include "world/level/block/Block.h"

class Material;
class BlockEntity;

class EntityBlock : public Block {
public:
	Unique<BlockEntity> newBlockEntity(const BlockPos& pos, const BlockID blockID) const;

	void neighborChanged(BlockSource& region, const BlockPos& pos, const BlockPos& neighborPos) const override;

	void triggerEvent(BlockSource& region, const BlockPos& pos, int b0, int b1) const override;

protected:
	EntityBlock(const std::string& nameId, int id, const Material& material);
};
