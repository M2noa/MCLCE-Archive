#include "Dungeons.h"

#include "world/level/levelgen/feature/MegaJungleTreeFeature.h"
#include "world/level/BlockSource.h"
#include "world/level/block/VineBlock.h"

MegaJungleTreeFeature::MegaJungleTreeFeature(Entity* placer, int baseHeight, int heightInterval, int trunkType, int leafType) :
	MegaTreeFeature(placer, baseHeight, heightInterval, trunkType, leafType) {

}

bool MegaJungleTreeFeature::place(BlockSource& region, const BlockPos& pos, Random& random) const {
	int treeHeight = _calcTreeHeigth(random);
	if (!_prepareTree(region, pos, treeHeight)) {
		return false;
	}

	_createCrown(region, pos.above(treeHeight), 2);

	int branchHeight = pos.y + treeHeight - 2 - random.nextInt(4);
	while (branchHeight > pos.y + treeHeight / 2) {
		float angle = random.nextFloat() * PI * 2.0f;

		int bx = pos.x + (int)(0.5f + Math::cos(angle) * 4.0f);
		int bz = pos.z + (int)(0.5f + Math::sin(angle) * 4.0f);
		for (auto b : range(5)) {
			bx = pos.x + (int)(1.5f + Math::cos(angle) * b);
			bz = pos.z + (int)(1.5f + Math::sin(angle) * b);
			_setBlockAndData(region, BlockPos(bx, branchHeight - 3 + b / 2, bz), FullBlock(Block::mLog->mID, _getTrunkType()));
		}

		int leafHeight = 1 + random.nextInt(2);
		int topPosition = branchHeight;
		for (auto yy : range_incl(topPosition - leafHeight, topPosition)) {
			int yo = yy - topPosition;
			_placeSingleTrunkLeaves(region, BlockPos(bx, yy, bz), 1 - yo);
		}

		branchHeight -= 2 + random.nextInt(4);
	}

	for (auto hh : range(treeHeight)) {
		const BlockPos above = pos.above(hh);
		_placeVines(region, above, random, hh, VineBlock::VINE_EAST, VineBlock::VINE_SOUTH);
		if (hh < treeHeight - 1) {
			_placeVines(region, above.east(), random, hh, VineBlock::VINE_WEST, VineBlock::VINE_SOUTH);
			_placeVines(region, above.south().east(), random, hh, VineBlock::VINE_WEST, VineBlock::VINE_NORTH);
			_placeVines(region, above.south(), random, hh, VineBlock::VINE_EAST, VineBlock::VINE_NORTH);
		}
	}

	return true;
}

void MegaJungleTreeFeature::_placeVines(BlockSource& region, const BlockPos& pos, Random& random, int height, int dirx, int dirz) const {
	if (_isFree(region.getMaterial(pos))) {
		_setBlockAndData(region, pos, FullBlock(Block::mLog->mID, _getTrunkType()));
		if (height > 0) {
			BlockPos posX = dirx == VineBlock::VINE_EAST ? pos.west() : pos.east();
			BlockPos poxZ = dirz == VineBlock::VINE_NORTH ? pos.south() : pos.north();
			_placeVine(region, posX, random, dirx);
			_placeVine(region, poxZ, random, dirz);
		}
	}
}

void MegaJungleTreeFeature::_placeVine(BlockSource& region, const BlockPos& pos, Random& random, int direction) const {
	if (random.nextInt(3) > 0 && region.isEmptyBlock(pos)) {
		_setBlockAndData(region, pos, FullBlock(Block::mVine->mID, direction));
	}
}

void MegaJungleTreeFeature::_createCrown(BlockSource& region, const BlockPos& pos, int baseRadius) const {
	int crownHeight = 2;
	for (auto dy : range_incl(-crownHeight, 0)) {
		_placeDoubleTrunkLeaves(region, pos.above(dy), baseRadius + 1 - dy);
	}
}
