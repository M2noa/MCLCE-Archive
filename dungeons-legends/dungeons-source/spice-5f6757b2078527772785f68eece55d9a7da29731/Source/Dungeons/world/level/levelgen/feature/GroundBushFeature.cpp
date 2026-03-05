#include "Dungeons.h"

#include "world/level/levelgen/feature/GroundBushFeature.h"
#include "util/Random.h"
#include "world/level/BlockSource.h"
#include "world/level/block/Block.h"
#include "world/level/material/Material.h"

GroundBushFeature::GroundBushFeature(int trunkType, int leafType) :
	TreeFeature(nullptr, trunkType, leafType) {

}

bool GroundBushFeature::place(BlockSource& region, const BlockPos& pos, Random& random) const {
	BlockPos blockPos(pos);
	while (_isFree(region.getMaterial(blockPos)) && blockPos.y > 0) {
		blockPos = blockPos.below();
	}

	const BlockID block = region.getBlockID(blockPos);
	if (block == Block::mDirt->mID || block == Block::mGrass->mID) {
		_setBlockAndData(region, blockPos, FullBlock(Block::mLog->mID, _getLeafType()));

		for (auto yy : range_incl(blockPos.y, blockPos.y + 2)) {
			int yo = yy - blockPos.y;
			int offs = 2 - yo;

			for (auto xx : range_incl(blockPos.x - offs, blockPos.x + offs)) {
				int xo = xx - (blockPos.x);

				for (auto zz : range_incl(blockPos.z - offs, blockPos.z + offs)) {
					int zo = zz - (blockPos.z);
					if (std::abs(xo) == offs && std::abs(zo) == offs && random.nextInt(2) == 0) {
						continue;
					}

					BlockPos leafBlock(xx, yy, zz);
					const Block* t = Block::mBlocks[region.getBlockID(leafBlock)];
					if (t == nullptr || (t != nullptr && !t->isSolid())) {
						_setBlockAndData(region, leafBlock, FullBlock(Block::mLeaves->mID, _getLeafType()));
					}
				}
			}
		}
	}

	return true;
}
