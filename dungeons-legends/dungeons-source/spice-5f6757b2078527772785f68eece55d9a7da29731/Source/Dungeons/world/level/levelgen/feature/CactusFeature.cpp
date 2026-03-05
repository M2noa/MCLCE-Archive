#include "Dungeons.h"

#include "world/level/levelgen/feature/CactusFeature.h"
#include "world/level/block/CactusBlock.h"
#include "world/level/BlockSource.h"

bool CactusFeature::place(BlockSource& region, const BlockPos& pos, Random& random) const {
	for (int i = 0; i < 10; i++) {

		const int randZ = random.nextGaussianInt(8);
		const int randY = random.nextGaussianInt(4);
		const int randX = random.nextGaussianInt(8);
		BlockPos blockPos = pos.offset(randX, randY, randZ);

		if (region.isEmptyBlock(blockPos)) {
			int randh = random.nextInt(3) + 1;
			int h = 1 + random.nextInt(randh);

			for (int yy = 0; yy < h; yy++) {
				if (Block::mCactus->canSurvive(region, blockPos.above(yy))) {
					_setBlockAndData( region, {blockPos.x, blockPos.y + yy, blockPos.z}, Block::mCactus->mID);
				}
			}
		}
	}

	return true;
}
