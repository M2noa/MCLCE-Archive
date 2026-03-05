#include "Dungeons.h"

#include "world/level/levelgen/feature/WaterlilyFeature.h"
#include "world/level/BlockSource.h"
#include "world/level/block/Block.h"
#include "util/Random.h"

bool WaterlilyFeature::place(BlockSource& region, const BlockPos& pos, Random& random) const {
	for (auto i : range(10)) {
		UNUSED_VARIABLE(i);
		const int randZ = random.nextGaussianInt(8);
		const int randY = random.nextGaussianInt(4);
		const int randX = random.nextGaussianInt(8);
		BlockPos lilyPos = pos.offset(randX, randY, randZ);

		if (region.isEmptyBlock(lilyPos) && Block::mWaterlily->mayPlace(region, lilyPos)) {
			_setBlockAndData( region, lilyPos, Block::mWaterlily->mID);
		}
	}

	return true;
}
