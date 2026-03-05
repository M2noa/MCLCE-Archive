#include "Dungeons.h"

#include "world/level/levelgen/feature/FlowerFeature.h"
#include "util/Random.h"
#include "world/level/BlockSource.h"
#include "world/level/block/BushBlock.h"

FlowerFeature::FlowerFeature(BlockID block) :
	mBlock(block) {
}

bool FlowerFeature::place(BlockSource& region, const BlockPos& pos, Random& random) const {
	//only used by mushrooms
	return placeFlower(region, pos, mBlock, random);
}

bool FlowerFeature::placeFlower(BlockSource& region, const BlockPos& pos, FullBlock flower, Random& random) const {
	for (auto i : range(64)) {
		UNUSED_VARIABLE(i);
		const int randZ = random.nextGaussianInt(8);
		const int randY = random.nextGaussianInt(4);
		const int randX = random.nextGaussianInt(8);
		BlockPos blockPos = pos.offset(randX, randY, randZ);
		if (region.isEmptyBlock(blockPos)) {
			if (((const BushBlock*)Block::mBlocks[flower.id])->canSurvive(region, blockPos)) {
				_setBlockAndData( region, blockPos, flower);
			}
		}
	}

	return true;
}
