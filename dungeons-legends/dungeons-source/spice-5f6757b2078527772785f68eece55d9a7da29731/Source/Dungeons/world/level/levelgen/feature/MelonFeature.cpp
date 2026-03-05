#include "Dungeons.h"

#include "world/level/levelgen/feature/MelonFeature.h"
#include "util/Random.h"
#include "world/level/BlockSource.h"
#include "world/level/block/Block.h"

bool MelonFeature::place(BlockSource& region, const BlockPos& pos, Random& random) const {
	for (auto i : range(64)) {
		UNUSED_VARIABLE(i);
		const int randZ = random.nextGaussianInt(8);
		const int randY = random.nextGaussianInt(4);
		const int randX = random.nextGaussianInt(8);
		BlockPos melonPos = pos.offset(randX, randY, randZ);

 		if (Block::mMelon->mayPlace(region, melonPos) && region.getBlockID(melonPos.below()) == Block::mGrass->mID) {
			_setBlockAndData( region, melonPos, Block::mMelon->mID);
		}
	}

	return true;
}
