/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#include "world/level/block/LiquidBlockStatic.h"
#include "world/level/material/Material.h"
#include "world/level/BlockSource.h"
// #include "world/level/BlockTickingQueue.h"

LiquidBlockStatic::LiquidBlockStatic(const std::string& nameId, int id, BlockID dynamicID, const Material& material)
	: LiquidBlock(nameId, id, material)
	, mDynamicID(dynamicID) {
	setTicking(material.isType(MaterialType::Lava));
}

void LiquidBlockStatic::neighborChanged(BlockSource& region, const BlockPos& pos, const BlockPos& neighborPos) const{
	LiquidBlock::neighborChanged(region, pos, neighborPos);
	if (region.getBlockID(pos) == mID) {
		_setDynamic(region, pos);
	}
}

void LiquidBlockStatic::tick(BlockSource& region, const BlockPos& pos, Random& random) const{
	if (mMaterial.isType(MaterialType::Lava)) {
		trySpreadFire(region, pos, random);
	}
}

bool LiquidBlockStatic::_isFlammable(BlockSource& region, const BlockPos& pos){
	return region.getMaterial(pos).isFlammable();
}

void LiquidBlockStatic::_setDynamic(BlockSource& region, const BlockPos& pos) const {
	region.setBlockAndData(pos, (BlockID)(mID - 1), region.getData(pos), Block::UPDATE_CLIENTS);
// 	region.getTickQueue(pos)->add(region, pos, mDynamicID, getTickDelay(region));
}
