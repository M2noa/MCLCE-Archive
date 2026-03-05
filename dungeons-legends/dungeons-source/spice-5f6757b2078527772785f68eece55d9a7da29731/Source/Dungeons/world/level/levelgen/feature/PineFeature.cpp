#include "Dungeons.h"

#include "world/level/levelgen/feature/PineFeature.h"
#include "world/level/BlockSource.h"
#include "world/level/Level.h"
#include "world/level/block/Block.h"
#include "world/level/block/OldLogBlock.h"
#include "world/level/block/OldLeafBlock.h"
#include "world/level/LevelConstants.h"

PineFeature::PineFeature(Entity* placer)
	: TreeFeature(placer, enum_cast(OldLogBlock::LogType::Spruce), enum_cast(OldLeafBlock::LeafType::Spruce)) {
}

bool PineFeature::place(BlockSource& region, const BlockPos& pos, Random& random) const {

	// pines can be quite tall
	int treeHeight = random.nextInt(5) + 7;
	int trunkHeight = treeHeight - random.nextInt(2) - 3;
	int topHeight = treeHeight - trunkHeight;
	int topRadius = 1 + random.nextInt(topHeight + 1);

	bool free = true;
	// may not be outside of pos.y boundaries
	if (pos.y < 1 || pos.y + treeHeight + 1 > region.getMaxHeight()) {
		return false;
	}

	// make sure there is enough space
	for (int yy = pos.y; yy <= pos.y + 1 + treeHeight && free; yy++) {
		int r = ((yy - pos.y) < trunkHeight) ? 0 : topRadius;

		for (int xx = pos.x - r; xx <= pos.x + r && free; xx++) {
			for (int zz = pos.z - r; zz <= pos.z + r && free; zz++) {
				if (yy >= 0 && yy < region.getMaxHeight()) {
					BlockID tt = region.getBlockID(BlockPos(xx, yy, zz));
					free = (tt != 0 && tt != Block::mLeaves->mID) ? false : free;
				}
				else {
					free = false;
				}
			}
		}
	}

	if (!free) {
		return false;
	}

	// must stand on ground
	BlockID belowBlock = region.getBlockID(pos.below());
	if ((belowBlock != Block::mGrass->mID && belowBlock != Block::mDirt->mID) || pos.y >= region.getMaxHeight() - treeHeight - 1) {
		return false;
	}

	_placeBlock(region, pos.below(), FullBlock(Block::mDirt->mID));

	// place leaf top
	int currentRadius = 0;

	for (auto yy : reverse_range(pos.y + trunkHeight, pos.y + treeHeight + 1)) {
		for (int xx = pos.x - currentRadius; xx <= pos.x + currentRadius; xx++) {
			int xo = xx - pos.x;

			for (int zz = pos.z - currentRadius; zz <= pos.z + currentRadius; zz++) {
				int zo = zz - pos.z;
				BlockPos blockPos(xx, yy, zz);
				if (std::abs(xo) == currentRadius && std::abs(zo) == currentRadius && currentRadius > 0) {
					continue;
				}
				if (!Block::mSolid[region.getBlockID(blockPos)]) {
					_placeBlock(region, blockPos, FullBlock(Block::mLeaves->mID, enum_cast(OldLeafBlock::LeafType::Spruce)));
				}
			}
		}

		if (currentRadius >= 1 && yy == (pos.y + trunkHeight + 1)) {
			currentRadius -= 1;
		}
		else if (currentRadius < topRadius) {
			currentRadius += 1;
		}
	}

	for (int hh = 0; hh < treeHeight - 1; hh++) {
		BlockID t = region.getBlockID(pos.above(hh));
		if (t == 0 || t == Block::mLeaves->mID) {
			_placeBlock(region, pos.above(hh), FullBlock(Block::mLog->mID, enum_cast(OldLogBlock::LogType::Spruce)));
		}
	}

	return true;
}
