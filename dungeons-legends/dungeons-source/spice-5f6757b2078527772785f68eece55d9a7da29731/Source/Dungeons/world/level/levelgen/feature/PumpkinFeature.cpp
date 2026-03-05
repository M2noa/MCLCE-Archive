#include "Dungeons.h"

#include "world/level/levelgen/feature/PumpkinFeature.h"
#include "util/Random.h"
#include "world/level/BlockSource.h"
#include "world/level/block/Block.h"

bool PumpkinFeature::place(BlockSource& region, const BlockPos& pos, Random& random) const {
	for (auto i : range(64)) {
		UNUSED_VARIABLE(i);
		const int randX = random.nextGaussianInt(8);
		const int randZ = random.nextGaussianInt(8);
		const int randY = random.nextGaussianInt(4);
		BlockPos pumpkinPos = pos.offset(randX, randY, randZ);
		if (region.isEmptyBlock(pumpkinPos) && region.getBlockID(pumpkinPos.below()) == Block::mGrass->mID) {
			if (Block::mPumpkin->mayPlace(region, pumpkinPos)) {
				_setBlockAndData( region, pumpkinPos, {Block::mPumpkin->mID, (DataID)random.nextInt(4)});
			}
		}
	}

	return true;
}
