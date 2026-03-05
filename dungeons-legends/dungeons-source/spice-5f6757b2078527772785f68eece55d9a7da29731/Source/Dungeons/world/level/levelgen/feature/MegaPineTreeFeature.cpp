#include "Dungeons.h"

#include "world/level/levelgen/feature/MegaPineTreeFeature.h"
#include "world/level/block/OldLeafBlock.h"
#include "world/level/block/OldLogBlock.h"
#include "world/level/material/Material.h"
#include "world/level/BlockSource.h"

MegaPineTreeFeature::MegaPineTreeFeature(Entity* placer, bool isSpruce) :
	MegaTreeFeature(placer, 13, 15, enum_cast(OldLogBlock::LogType::Spruce), enum_cast(OldLeafBlock::LeafType::Spruce))
	, mIsSpruce(isSpruce) {

}

bool MegaPineTreeFeature::place(BlockSource& region, const BlockPos& pos, Random& random) const {
	int treeHeight = _calcTreeHeigth(random);
	if (!_prepareTree(region, pos, treeHeight)) {
		return false;
	}

	createCrown(region, pos.above(treeHeight), 0, random);

	for (auto hh : range(treeHeight)) {
		if (_isFree(region.getMaterial(pos.above(hh)))) {
			_setBlockAndData(region, pos.above(hh), FullBlock(Block::mLog->mID, _getTrunkType()));
		}
		if (hh < treeHeight - 1) {

			if (_isFree(region.getMaterial(pos.offset(1, hh, 0)))) {
				_setBlockAndData(region, pos.offset(1, hh, 0), FullBlock(Block::mLog->mID, _getTrunkType()));
			}

			if (_isFree(region.getMaterial(pos.offset(1, hh, 1)))) {
				_setBlockAndData(region, pos.offset(1, hh, 1), FullBlock(Block::mLog->mID, _getTrunkType()));
			}

			if (_isFree(region.getMaterial(pos.offset(0, hh, 1)))) {
				_setBlockAndData(region, pos.offset(0, hh, 1), FullBlock(Block::mLog->mID, _getTrunkType()));
			}
		}
	}

	return true;
}

void MegaPineTreeFeature::createCrown(BlockSource& region, const BlockPos& origin, int baseRadius, Random& random) const {
	int crownHeight = random.nextInt(5) + (mIsSpruce ? _getBaseHeight() : 3);
	int prevRadius = 0;

	for (auto yy : range_incl(origin.y - crownHeight, origin.y)) {
		int yo = origin.y - yy;
		int radius = baseRadius + (int)std::floor(yo / (float)crownHeight * 3.5f);
		_placeDoubleTrunkLeaves(region, BlockPos(origin.x, yy, origin.z), radius + (yo > 0 && radius == prevRadius && (yy & 1) == 0 ? 1 : 0));
		prevRadius = radius;
	}
}

void MegaPineTreeFeature::postPlaceTree(BlockSource& region, Random& random, const BlockPos& pos) const {
	_placePodzolCircle(region, pos.west().north());
	_placePodzolCircle(region, pos.east(2).north());
	_placePodzolCircle(region, pos.west().south(2));
	_placePodzolCircle(region, pos.east(2).south(2));

	for (auto i : range(5)) {
		UNUSED_VARIABLE(i);
		int placement = random.nextInt(8 * 8);
		int xx = placement % 8;
		int zz = placement / 8;
		if (xx == 0 || xx == 7 || zz == 0 || zz == 7) {
			_placePodzolCircle(region, pos.offset(-3 + xx, 0, -3 + zz));
		}
	}
}

void MegaPineTreeFeature::_placePodzolCircle(BlockSource& region, const BlockPos& pos) const {
	for (auto xx : range_incl(-2, 2)) {
		for (auto zz : range_incl(-2, 2)) {
			if (std::abs(xx) != 2 || std::abs(zz) != 2) {
				_placePodzolAt(region, pos.offset(xx, 0, zz));
			}
		}
	}
}

void MegaPineTreeFeature::_placePodzolAt(BlockSource& region, const BlockPos& pos) const {
	for (auto dy : reverse_range(-3, 3)) {
		BlockPos blockPos = pos.above(dy);
		const Block& block = region.getBlock(blockPos);
		if (block.isType(Block::mGrass) || block.isType(Block::mDirt) || block.isType(Block::mPodzol)) {
			_setBlockAndData( region, blockPos, Block::mPodzol->mID);
			break;
		}
		else if (block.isType(Block::mAir) && dy < 0) {
			break;
		}
	}
}
