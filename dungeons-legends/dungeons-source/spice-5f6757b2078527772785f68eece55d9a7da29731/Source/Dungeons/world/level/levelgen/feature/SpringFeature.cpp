#include "Dungeons.h"

#include "world/level/levelgen/feature/SpringFeature.h"
#include "world/level/BlockSource.h"
#include "world/level/block/Block.h"
#include "util/Random.h"

SpringFeature::SpringFeature(BlockID block) :
	mBlock(block) {

}

bool SpringFeature::place(BlockSource& region, const BlockPos& pos, Random& random) const {
	if (region.getBlockID(pos.above()) != Block::mStone->mID) {
		return false;
	}
	if (region.getBlockID(pos.below()) != Block::mStone->mID) {
		return false;
	}

	if (region.getBlockID(pos) != 0 && region.getBlockID(pos) != Block::mStone->mID) {
		return false;
	}

	int rockCount = 0;
	if (region.getBlockID(pos.west()) == Block::mStone->mID) {
		rockCount++;
	}
	if (region.getBlockID(pos.east()) == Block::mStone->mID) {
		rockCount++;
	}
	if (region.getBlockID(pos.north()) == Block::mStone->mID) {
		rockCount++;
	}
	if (region.getBlockID(pos.south()) == Block::mStone->mID) {
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

	if (rockCount == 3 && holeCount == 1 && _setBlockAndData( region, pos, mBlock)) {
		Block::mBlocks[mBlock]->tick(region, pos, random);
	}

	return true;
}
