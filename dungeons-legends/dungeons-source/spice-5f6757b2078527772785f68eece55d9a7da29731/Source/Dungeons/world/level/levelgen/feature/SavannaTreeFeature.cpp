#include "Dungeons.h"

#include "world/level/levelgen/feature/SavannaTreeFeature.h"
#include "world/level/BlockSource.h"
#include "world/level/block/Block.h"
#include "world/level/block/NewLeafBlock.h"
#include "world/level/block/NewLogBlock.h"
#include "world/Direction.h"
#include "world/level/Level.h"

SavannaTreeFeature::SavannaTreeFeature(Entity* placer) :
	TreeFeature(placer, NewLogBlock::ACACIA_TRUNK, NewLeafBlock::ACACIA_LEAF) {

}

bool SavannaTreeFeature::place(BlockSource& region, const BlockPos& pos, Random& random) const {
	const int rand0 = random.nextInt(3);
	const int rand1 = random.nextInt(3);
	int treeHeight = rand0 + rand1 + 5;
	if (!_prepareSpawn(region, pos, treeHeight) || pos.y >= region.getMaxHeight() - treeHeight - 1) {
		return false;
	}

	_placeBlock(region, pos.below(), FullBlock(Block::mDirt->mID));

	int leanDirection = random.nextInt(4);
	int leanHeight = treeHeight - random.nextInt(4) - 1;
	int leanSteps = 3 - random.nextInt(3);

	BlockPos blockPos(pos);
	int ey = 0;

	for (auto yo : range(treeHeight)) {
		int yy = pos.y + yo;
		if (yo >= leanHeight && leanSteps > 0) {
			blockPos += BlockPos(Direction::STEP_X[leanDirection], 0, Direction::STEP_Z[leanDirection]);
			leanSteps--;
		}
		blockPos.y = yy;
		if (_isFree(region.getMaterial(blockPos))) {
			_placeBlock(region, blockPos, FullBlock(Block::mLog2->mID, NewLogBlock::ACACIA_TRUNK));
			ey = yy;
		}
	}

	blockPos.y = ey;
	for (auto ox : range_incl(-1, 1)) {
		for (auto oz : range_incl(-1, 1)) {
			_placeLeafAt(region, blockPos.offset(ox, 1, oz));
		}
	}
	_placeLeafAt(region, blockPos + BlockPos(2, 1, 0));
	_placeLeafAt(region, blockPos + BlockPos(-2, 1, 0));
	_placeLeafAt(region, blockPos + BlockPos(0, 1, 2));
	_placeLeafAt(region, blockPos + BlockPos(0, 1, -2));

	for (auto ox : range_incl(-3, 3)) {
		for (auto oz : range_incl(-3, 3)) {
			if (std::abs(ox) == 3 && std::abs(oz) == 3) {
				continue;
			}
			_placeLeafAt(region, blockPos.offset(ox, 0, oz));
		}
	}

	blockPos.x = pos.x;
	blockPos.z = pos.z;

	int branchDirection = random.nextInt(4);
	if (branchDirection != leanDirection) {
		int branchPos = leanHeight - random.nextInt(2) - 1;
		int branchSteps = 1 + random.nextInt(3);

		ey = 0;

		for (int yo = branchPos; yo < treeHeight && branchSteps > 0; yo++, branchSteps--) {
			if (yo < 1) {
				continue;
			}

			int yy = pos.y + yo;
			blockPos += BlockPos(Direction::STEP_X[branchDirection], 0, Direction::STEP_Z[branchDirection]);
			blockPos.y = yy;
			if (_isFree(region.getMaterial(blockPos))) {
				_placeBlock(region, blockPos, FullBlock(Block::mLog2->mID, NewLogBlock::ACACIA_TRUNK));
				ey = yy;
			}
		}

		if (ey > 0) {
			blockPos.y = ey;
			for (auto ox : range_incl(-1, 1)) {
				for (auto oz : range_incl(-1, 1)) {
					_placeLeafAt(region, blockPos.offset(ox, 1, oz));
				}
			}

			for (auto ox : range_incl(-2, 2)) {
				for (auto oz : range_incl(-2, 2)) {
					if (std::abs(ox) == 2 && std::abs(oz) == 2) {
						continue;
					}
					_placeLeafAt(region, blockPos.offset(ox, 0, oz));
				}
			}
		}
	}

	return true;
}

void SavannaTreeFeature::_placeLeafAt(BlockSource& region, const BlockPos& pos) const {
	if (_isFree(region.getMaterial(pos))) {
		_setBlockAndData(region, pos, FullBlock(Block::mLeaves2->mID, NewLeafBlock::ACACIA_LEAF));
	}
}
