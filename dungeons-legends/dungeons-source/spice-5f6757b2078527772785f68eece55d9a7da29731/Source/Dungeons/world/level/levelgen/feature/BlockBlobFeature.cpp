#include "Dungeons.h"

#include "world/level/levelgen/feature/BlockBlobFeature.h"
#include "world/level/BlockSource.h"
#include "util/Random.h"
#include "world/level/block/Block.h"

BlockBlobFeature::BlockBlobFeature(BlockID block, int radius) :
	mBlock(block)
	, mStartRadius(radius) {

}

bool BlockBlobFeature::place(BlockSource& region, const BlockPos& pos, Random& random) const {
	BlockPos blockPos(pos);
	while (blockPos.y > 3) {
		if (!region.isEmptyBlock(blockPos.below())) {
			BlockID subBlock = region.getBlockID(blockPos.below());
			if (subBlock == Block::mGrass->mID || subBlock == Block::mDirt->mID || subBlock == Block::mStone->mID) {
				break;
			}
		}
		blockPos = blockPos.below();
	}

	if (blockPos.y <= 3) {
		return false;
	}

	int c = 0;

	while (mStartRadius >= 0 && c < 3) {
		int xr = mStartRadius + random.nextInt(2);
		int yr = mStartRadius + random.nextInt(2);
		int zr = mStartRadius + random.nextInt(2);
		float tr = (xr + yr + zr) * .333f + .5f;

		for (auto x : range_incl(blockPos.x - xr, blockPos.x + xr)) {
			for (auto y : range_incl(blockPos.y - yr, blockPos.y + yr)) {
				for (auto z : range_incl(blockPos.z - zr, blockPos.z + zr)) {
					BlockPos blobPos(x, y, z);
					if (blobPos.distSqr(blockPos) <= tr * tr) {
						_setBlockAndData( region, blobPos, mBlock);
					}
				}
			}
		}

		int offsetX = -(mStartRadius + 1) + random.nextInt(2 + mStartRadius * 2);
		int offsetY = 0 - random.nextInt(2);
		int offsetZ = -(mStartRadius + 1) + random.nextInt(2 + mStartRadius * 2);
		blockPos = blockPos.offset(offsetX, offsetY, offsetZ);
		c++;
	}
	return true;
}
