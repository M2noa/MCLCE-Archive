#include "Dungeons.h"

#include "world/level/levelgen/feature/HellSpringFeature.h"
#include "world/level/block/Block.h"
#include "world/level/BlockSource.h"
#include "world/level/material/Material.h"

HellSpringFeature::HellSpringFeature(BlockID blockId, bool insideRock) :
	mBlockId(blockId)
	, mInsideRock(insideRock){
}

bool HellSpringFeature::place(BlockSource& region, const BlockPos& pos, Random& random) const {
	if (region.getBlockID(pos.above()) != Block::mNetherrack->mID) {
		return false;
	}

	if (!region.getMaterial(pos).isType(MaterialType::Air) &&
		region.getBlockID(pos) != Block::mNetherrack->mID) {
		return false;
	}

	int rockCount = 0;
	if (region.getBlockID(pos.west()) == Block::mNetherrack->mID) {
		rockCount++;
	}
	if (region.getBlockID(pos.east()) == Block::mNetherrack->mID) {
		rockCount++;
	}
	if (region.getBlockID(pos.north()) == Block::mNetherrack->mID) {
		rockCount++;
	}
	if (region.getBlockID(pos.south()) == Block::mNetherrack->mID) {
		rockCount++;
	}
	if (region.getBlockID(pos.below()) == Block::mNetherrack->mID) {
		rockCount++;
	}

	int holeCount = 0;
	if (region.isEmptyBlock(pos.west())) {
		holeCount++;
	}
	if (region.isEmptyBlock(pos.east())) {
		holeCount++;
	}
	if (region.isEmptyBlock(pos.north())) {
		holeCount++;
	}
	if (region.isEmptyBlock(pos.south())) {
		holeCount++;
	}
	if (region.isEmptyBlock(pos.below())) {
		holeCount++;
	}

	if ((!mInsideRock && rockCount == 4 && holeCount == 1) || rockCount == 5) {
		_setBlockAndData( region, pos, mBlockId);
	}

	return true;
}
