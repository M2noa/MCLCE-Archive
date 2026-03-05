/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#include "world/level/block/EntityBlock.h"
#include "world/level/block/entity/BlockEntity.h"
#include "world/level/Level.h"
#include "world/level/material/Material.h"
#include "world/level/BlockSource.h"

void EntityBlock::triggerEvent(BlockSource& region, const BlockPos& pos, int b0, int b1) const{
	Block::triggerEvent(region, pos, b0, b1);
	BlockEntity* blockEntity = region.getBlockEntity(pos.x, pos.y, pos.z);
	if (blockEntity != nullptr) {
		blockEntity->triggerEvent(b0, b1);
	}
}

EntityBlock::EntityBlock(const std::string& nameId, int id, const Material& material)
	: Block(nameId, id, material) {
}

Unique<BlockEntity> EntityBlock::newBlockEntity(const BlockPos& pos, const BlockID blockID) const {
	return BlockEntityFactory::createBlockEntity(getBlockEntityType(), pos, blockID);
}

void EntityBlock::neighborChanged(BlockSource& region, const BlockPos& pos, const BlockPos& neighborPos) const {
	BlockEntity* blockEntity = region.getBlockEntity(pos);
	if (blockEntity) {
		blockEntity->onNeighborChanged(region, neighborPos);
	}
}
