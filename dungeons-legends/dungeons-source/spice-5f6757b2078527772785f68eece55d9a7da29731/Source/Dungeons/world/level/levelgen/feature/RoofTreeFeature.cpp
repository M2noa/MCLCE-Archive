#include "Dungeons.h"

#include "world/level/levelgen/feature/RoofTreeFeature.h"
#include "world/level/block/NewLogBlock.h"
#include "world/level/block/NewLeafBlock.h"
#include "world/Direction.h"
#include "world/level/block/VineBlock.h"
#include "world/level/material/Material.h"
#include "world/level/BlockSource.h"
#include "world/level/Level.h"

RoofTreeFeature::RoofTreeFeature(Entity* placer) :
	TreeFeature(placer, NewLogBlock::BIG_OAK_TRUNK, NewLeafBlock::BIG_OAK_LEAF) {

}

bool RoofTreeFeature::place(BlockSource& region, const BlockPos& pos, Random& random) const {
	const int rand0 = random.nextInt(3);
	const int rand1 = random.nextInt(2);
	int treeHeight = rand0 + rand1 + 6;

	if (!_prepareSpawn(region, pos, treeHeight) || pos.y >= region.getMaxHeight() - treeHeight - 1) {
		return false;
	}

	FullBlock blockData(Block::mDirt->mID);
	_placeBlock(region, pos + BlockPos(0, -1, 0), blockData);
	_placeBlock(region, pos + BlockPos(1, -1, 0), blockData);
	_placeBlock(region, pos + BlockPos(1, -1, 1), blockData);
	_placeBlock(region, pos + BlockPos(0, -1, 1), blockData);

	int leanDirection = random.nextInt(4);
	int leanHeight = treeHeight - random.nextInt(4);
	int leanSteps = 2 - random.nextInt(3);

	BlockPos blockPos(pos);
	int ey = 0;

	bool viney = random.nextInt(6) == 0;

	for (auto hh : range(treeHeight)) {
		int yy = pos.y + hh;
		if (hh >= leanHeight && leanSteps > 0) {
			blockPos += BlockPos(Direction::STEP_X[leanDirection], 0, Direction::STEP_Z[leanDirection]);
			leanSteps--;
		}
		blockPos.y = yy;
		if (_isFree(region.getMaterial(blockPos))) {
			BlockPos above = pos.above(hh);
			_placeVines(region, above, random, viney, VineBlock::VINE_EAST, VineBlock::VINE_SOUTH);
			_placeVines(region, above.east(), random, viney, VineBlock::VINE_WEST, VineBlock::VINE_SOUTH);
			_placeVines(region, above.south().east(), random, viney, VineBlock::VINE_WEST, VineBlock::VINE_NORTH);
			_placeVines(region, above.south(), random, viney, VineBlock::VINE_EAST, VineBlock::VINE_NORTH);
			ey = yy;
		}
	}

	blockPos.y = ey;
	_placeLeafs(region, blockPos, pos, random);
	return true;
}

void RoofTreeFeature::_placeVines(BlockSource& region, const BlockPos& pos, Random& random, bool shouldPlace, int dirx, int dirz) const {
	if (_isFree(region.getMaterial(pos))) {
		_setBlockAndData(region, pos, FullBlock(Block::mLog2->mID, _getTrunkType()));
		if (shouldPlace) {
			BlockPos posX = dirx == VineBlock::VINE_EAST ? pos.west() : pos.east();
			BlockPos poxZ = dirz == VineBlock::VINE_NORTH ? pos.south() : pos.north();
			_placeVine(region, posX, random, dirx);
			_placeVine(region, poxZ, random, dirz);
		}
	}
}

void RoofTreeFeature::_placeVine(BlockSource& region, const BlockPos& pos, Random& random, int direction) const {
	if (random.nextInt(7) > 0 && region.isEmptyBlock(pos)) {
		_setBlockAndData(region, pos, FullBlock(Block::mVine->mID, direction));
	}
}

void RoofTreeFeature::_placeLeafs(BlockSource& region, const BlockPos& leafPos, const BlockPos& pos, Random& random) const {
	for (auto ox : range_incl(-2, 0)) {
		for (auto oz : range_incl(-2, 0)) {
			int yo = -1;
			_placeLeafAt(region, leafPos + BlockPos(ox, yo, oz));
			_placeLeafAt(region, leafPos + BlockPos(1 - ox, yo, oz));
			_placeLeafAt(region, leafPos + BlockPos(ox, yo, 1 - oz));
			_placeLeafAt(region, leafPos + BlockPos(1 - ox, yo, 1 - oz));
			if ((ox <= -2 && oz <= -1) || (ox == -1 && oz == -2)) {
				continue;
			}
			yo = 1;
			_placeLeafAt(region, leafPos + BlockPos(ox, yo, oz));
			_placeLeafAt(region, leafPos + BlockPos(1 - ox, yo, oz));
			_placeLeafAt(region, leafPos + BlockPos(ox, yo, 1 - oz));
			_placeLeafAt(region, leafPos + BlockPos(1 - ox, yo, 1 - oz));
		}
	}

	if (random.nextBoolean()) {
		_placeLeafAt(region, leafPos + BlockPos(0, 2, 0));
		_placeLeafAt(region, leafPos + BlockPos(1, 2, 0));
		_placeLeafAt(region, leafPos + BlockPos(1, 2, 1));
		_placeLeafAt(region, leafPos + BlockPos(0, 2, 1));
	}

	for (auto ox : range_incl(-3, 4)) {
		for (auto oz : range_incl(-3, 4)) {
			if ((ox == -3 && oz == -3) || (ox == -3 && oz == 4) || (ox == 4 && oz == -3) || (ox == 4 && oz == 4)) {
				continue;
			}
			if (std::abs(ox) >= 3 && std::abs(oz) >= 3) {
				continue;
			}
			_placeLeafAt(region, leafPos + BlockPos(ox, 0, oz));
		}
	}

	for (auto ox : range_incl(-1, 2)) {
		for (auto oz : range_incl(-1, 2)) {
			if (ox >= 0 && ox <= 1 && oz >= 0 && oz <= 1) {
				continue;
			}
			if (random.nextInt(3) > 0) {
				continue;
			}

			int length = random.nextInt(3) + 2;
			for (auto branchY : range(length)) {
				_placeBlock(region, BlockPos(pos.x + ox, leafPos.y - branchY - 1, pos.z + oz), FullBlock(Block::mLog2->mID, NewLogBlock::BIG_OAK_TRUNK));
			}

			for (auto lx : range_incl(-1, 1)) {
				for (auto lz : range_incl(-1, 1)) {
					_placeLeafAt(region, leafPos + BlockPos(ox + lx, 0, oz + lz));
				}
			}

			for (auto lx : range_incl(-2, 2)) {
				for (auto lz : range_incl(-2, 2)) {
					if (std::abs(lx) == 2 && std::abs(lz) == 2) {
						continue;
					}
					_placeLeafAt(region, leafPos + BlockPos(ox + lx, -1, oz + lz));
				}
			}
		}
	}
}

void RoofTreeFeature::_placeLeafAt(BlockSource& region, const BlockPos& pos) const {
	auto& tm = region.getMaterial(pos);
	if (tm.isType(MaterialType::Air) || tm.isType(MaterialType::Vegetable)) {
		_setBlockAndData(region, pos, FullBlock(Block::mLeaves2->mID, NewLeafBlock::BIG_OAK_LEAF));
	}
}
