#include "Dungeons.h"

#include "world/level/levelgen/feature/IceSpikeFeature.h"
#include "world/level/BlockSource.h"
#include "world/level/block/Block.h"
#include "world/level/material/Material.h"
#include "util/Random.h"

bool IceSpikeFeature::place(BlockSource& region, const BlockPos& pos, Random& random) const {
	BlockPos blockPos(pos);
	while (region.isEmptyBlock(blockPos) && blockPos.y > 2) {
		blockPos = blockPos.below();
	}

	if (region.getBlockID(blockPos) != Block::mSnow->mID) {
		return false;
	}

	blockPos = blockPos.above(random.nextInt(4));

	int height = random.nextInt(4) + 7;
	int width = height / 4 + random.nextInt(2);
	if (width > 1 && random.nextInt(60) == 0) {
		blockPos = blockPos.above(10 + random.nextInt(30));
	}

	for (auto yOff : range(height)) {
		float scale = (1.0f - (float)yOff / height) * width;
		int newWidth = (int)std::ceil(scale);

		for (auto xo : range_incl(-newWidth, newWidth)) {
			float dx = std::abs(xo) - .25f;

			for (auto zo : range_incl(-newWidth, newWidth)) {
				float dz = std::abs(zo) - .25f;
				if ((xo != 0 || zo != 0) && (dx * dx + dz * dz) > scale * scale) {
					continue;
				}
				if (xo == -newWidth || xo == newWidth || zo == -newWidth || zo == newWidth) {
					if (random.nextFloat() > .75f) {
						continue;
					}
				}

				BlockPos offsetPos = blockPos.offset(xo, yOff, zo);
				BlockID t = region.getBlockID(offsetPos);
				if (_isValidPlaceBlock(t)) {
					_setBlockAndData( region, offsetPos, Block::mPackedIce->mID);
				}
				if (yOff != 0 && newWidth > 1) {
					offsetPos = blockPos.offset(xo, -yOff, zo);
					t = region.getBlockID(offsetPos);
					if (_isValidPlaceBlock(t)) {
						_setBlockAndData( region, offsetPos, Block::mPackedIce->mID);
					}
				}
			}
		}
	}

	int pillarWidth = width - 1;
	if (pillarWidth < 0) {
		pillarWidth = 0;
	}
	else if (pillarWidth > 1) {
		pillarWidth = 1;
	}

	for (auto xo : range_incl(-pillarWidth, pillarWidth)) {
		for (auto zo : range_incl(-pillarWidth, pillarWidth)) {
			BlockPos iceBlock = blockPos.offset(xo, -1, zo);
			int runLength = (std::abs(xo) == 1 && std::abs(zo) == 1) ? random.nextInt(5) : 50;

			while (iceBlock.y > 50) {
				BlockID t = region.getBlockID(iceBlock);
				if (_isValidPlaceBlock(t) || t == Block::mPackedIce->mID) {
					_setBlockAndData( region, iceBlock, Block::mPackedIce->mID);
				}
				else {
					break;
				}
				iceBlock = iceBlock.below();
				runLength--;
				if (runLength <= 0) {
					iceBlock = iceBlock.below(random.nextInt(5) + 1);
					runLength = random.nextInt(5);
				}
			}
		}
	}

	return true;
}

bool IceSpikeFeature::_isValidPlaceBlock(BlockID block) const {
	return block == Block::mAir->mID || block == Block::mDirt->mID || block == Block::mSnow->mID || block == Block::mIce->mID;
}
