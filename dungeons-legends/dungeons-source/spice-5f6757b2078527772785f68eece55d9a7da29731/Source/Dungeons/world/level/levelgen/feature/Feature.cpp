#include "Dungeons.h"

#include "world/level/levelgen/feature/Feature.h"
#include "world/level/block/Block.h"
#include "world/level/BlockSource.h"

Feature::Feature(Entity* placer)
	: mPlacer(placer)
{
}

void Feature::_setManuallyPlaced(Entity* placer) { 
	mPlacer = placer; 
}

bool Feature::_getManuallyPlaced() const {
	return mPlacer != nullptr;
}

bool Feature::_placeBlock(BlockSource& region, const BlockPos& pos, const FullBlock& blockData) const {
	return region.setBlockAndData( pos, blockData, Block::UPDATE_ALL, mPlacer);
}

bool Feature::_setBlockAndData(BlockSource& region, const BlockPos& pos, const FullBlock& blockData) const {
	return region.setBlockAndData( pos, blockData, Block::UPDATE_ALL, mPlacer);
}
