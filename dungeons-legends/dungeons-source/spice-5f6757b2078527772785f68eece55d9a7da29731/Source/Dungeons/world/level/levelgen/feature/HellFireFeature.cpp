#include "Dungeons.h"

#include "world/level/levelgen/feature/HellFireFeature.h"
#include "world/level/BlockSource.h"
#include "world/level/block/Block.h"
#include "world/level/block/FireBlock.h"
#include "util/Random.h"

bool HellFireFeature::place(BlockSource& region, const BlockPos& pos, Random& random) const {
	for (int i = 0; i < 64; i++) {
		const int randZ = random.nextGaussianInt(8);
		const int randY = random.nextGaussianInt(4);
		const int randX = random.nextGaussianInt(8);
		BlockPos firePos = pos.offset(randX, randY, randZ);

		if (!region.isEmptyBlock(firePos)) {
			continue;
		}
		if (region.getBlockID(firePos.below()) != Block::mNetherrack->mID) {
			continue;
		}
		_setBlockAndData( region, firePos, Block::mFire->mID );
	}

	return true;
}
