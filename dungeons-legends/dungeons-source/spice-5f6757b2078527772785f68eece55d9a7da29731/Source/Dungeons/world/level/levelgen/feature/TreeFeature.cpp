#include "Dungeons.h"

#include "world/level/levelgen/feature/TreeFeature.h"
#include "util/Random.h"
#include "world/Direction.h"
#include "world/level/Level.h"
#include "world/level/BlockSource.h"
#include "world/level/block/OldLogBlock.h"
#include "world/level/LevelConstants.h"
#include "world/level/block/VineBlock.h"
#include "world/level/material/Material.h"
#include "world/Facing.h"

TreeFeature::TreeFeature(Entity* placer, int trunkType, int leafType, bool addJungleFeatures /*= false*/)
	: Feature(placer)
	, mTrunkType(trunkType)
	, mLeafType(leafType)
	, mAddJungleFeatures(addJungleFeatures) {

}

void TreeFeature::_placeLeaf(BlockSource& region, const BlockPos& pos) const {
	BlockID t = region.getBlockID(pos);
	if (!Block::mSolid[t]) {
		_placeBlock(region, pos, FullBlock(Block::mLeaves->mID, mLeafType));
	}
}

bool TreeFeature::_prepareSpawn(BlockSource& region, const BlockPos& pos, int treeHeight) const {
	bool free = true;
	if (pos.y < 1 || pos.y + treeHeight + 1 > region.getMaxHeight()) {
		return false;
	}

	auto& belowBlock = region.getBlock(pos.below());
	if (!Block::mSapling->mayPlaceOn(belowBlock)) {
		return false;
	}

	for (auto yy : range_incl(pos.y, pos.y + 1 + treeHeight)) {
		int r = 1;
		if (yy == pos.y) {
			r = 0;
		}
		if (yy >= pos.y + 1 + treeHeight - 2) {
			r = 2;
		}

		for (int xx = pos.x - r; xx <= pos.x + r && free; xx++) {
			for (int zz = pos.z - r; zz <= pos.z + r && free; zz++) {
				if (yy >= 0 && yy < region.getMaxHeight()) {
					free = !_isFree(region.getBlockID(BlockPos(xx, yy, zz))) ? false : free;
				}
				else {
					free = false;
				}
			}
		}
	}

	//place dirt under the tree, or blockupdates will happen
	if (free) {
		_setBlockAndData(region, pos.below(), Block::mDirt->mID);
	}

	return free;
}

bool TreeFeature::place(BlockSource& region, const BlockPos& pos, Random& random) const {
	int treeHeight = random.nextInt(3) + 5;
	return place(region, pos, random, treeHeight);
}

void TreeFeature::_placeFallenTrunk(BlockSource& region, const BlockPos& pos, Random& random, int treeHeight) const {
	static const int mask[] = {
		0, 0,
		RotatedPillarBlock::FACING_Z,
		RotatedPillarBlock::FACING_Z,
		RotatedPillarBlock::FACING_X,
		RotatedPillarBlock::FACING_X,
	};

	auto facing = random.nextInt(4) + 2;
	auto& direction = Facing::DIRECTION[facing];

	int length = treeHeight - 2;

	// check if there is enough space
	BlockPos start = pos + direction * (2 + random.nextInt(2));
	start.y = region.getHeightmap(start);

	if (start.y > pos.y + 1) {	// allow slightly higher placement
		return;
	}

	BlockPos blockPos = start;
	int emptyBlocks = 0;

	for (int i = 0; i < length; ++i, blockPos += direction) {

		if (!region.isEmptyBlock(blockPos)) {
			return;
		}

		if (!region.isSolidBlockingBlock(blockPos.below())) {
			if (++emptyBlocks > 2) {
				return;
			}
		}
		else {
			emptyBlocks = 0;	// reset! can continue as there is solid ground again
		}
	}

	DataID data = 0;
	Block::mLog->getBlockState(BlockState::MappedType).set(data, mTrunkType);
	Block::mLog->getBlockState(BlockState::Direction).set(data, mask[facing]);

	blockPos = start;

	for (int i = 0; i < length; ++i, blockPos += direction) {
		_setBlockAndData(region, blockPos, {Block::mLog->mID, data});

		if (random.nextInt(10) == 0 && region.isEmptyBlock(blockPos.above())) {
			_setBlockAndData(region, blockPos.above(), random.nextFloat() < 0.5f ? Block::mBrownMushroom->mID : Block::mRedMushroom->mID);
		}
	}
}

bool TreeFeature::_placeTrunk(BlockSource& region, const BlockPos& pos, Random& random, int treeHeight) const {

	bool stump = !_getManuallyPlaced() && (random.nextInt(80) == 0);
	float vineChance;
	if (mTrunkType == enum_cast(OldLogBlock::LogType::Birch)) {
		vineChance = 0;
	}
	else if (stump) {
		vineChance = 3.f / 4.f;
	}
	else if (mAddJungleFeatures) {
		vineChance = 1.f / 3.f;
	}
	else if (random.nextInt(12) == 0) {
		vineChance = 1.f;
	}
	else {
		vineChance = 0.f;
	}

	if (stump) {
		_placeFallenTrunk(region, pos, random, treeHeight);
		treeHeight = 1;
	}

	for (int hh = 0; hh < treeHeight; hh++) {
		BlockPos heightPos = pos.above(hh);
		BlockID t = region.getBlockID(heightPos);

		// Allow the growing tree to cut through dirt, grass and leaves
		if (t == 0 || t == Block::mLeaves->mID || t == Block::mDirt->mID || t == Block::mGrass->mID) {
			_placeBlock(region, heightPos, FullBlock(Block::mLog->mID, mTrunkType));
			if (vineChance) {
				BlockPos emptyPos = heightPos.west();
				if (random.nextFloat() < vineChance && region.isEmptyBlock(emptyPos)) {
					_placeBlock(region, emptyPos, FullBlock(Block::mVine->mID, VineBlock::VINE_EAST));
				}
				emptyPos = heightPos.east();
				if (random.nextFloat() < vineChance && region.isEmptyBlock(emptyPos)) {
					_placeBlock(region, emptyPos, FullBlock(Block::mVine->mID, VineBlock::VINE_WEST));
				}
				emptyPos = heightPos.north();
				if (random.nextFloat() < vineChance && region.isEmptyBlock(emptyPos)) {
					_placeBlock(region, emptyPos, FullBlock(Block::mVine->mID, VineBlock::VINE_SOUTH));
				}
				emptyPos = heightPos.south();
				if (random.nextFloat() < vineChance && region.isEmptyBlock(emptyPos)) {
					_placeBlock(region, emptyPos, FullBlock(Block::mVine->mID, VineBlock::VINE_NORTH));
				}
			}
		}
	}

	return !stump;
}

bool TreeFeature::place(BlockSource& region, const BlockPos& pos, Random& random, int treeHeight) const {
	if (!_prepareSpawn(region, pos, treeHeight)) {
		return false;
	}

	if (!_placeTrunk(region, pos, random, treeHeight)) {
		return true;
	}

	for (auto yy : range_incl(pos.y - 3 + treeHeight, pos.y + treeHeight)) {
		int yo = yy - (pos.y + treeHeight);
		int offs = 1 - yo / 2;

		for (auto xx : range_incl(pos.x - offs, pos.x + offs)) {
			int xo = xx - pos.x;

			for (auto zz : range_incl(pos.z - offs, pos.z + offs)) {
				int zo = zz - pos.z;
				if (std::abs(xo) == offs && std::abs(zo) == offs && (random.nextInt(2) == 0 || yo == 0)) {
					continue;
				}

				_placeLeaf(region, BlockPos(xx, yy, zz));
			}
		}
	}

	if (mAddJungleFeatures) {
		for (auto yy : range_incl(pos.y - 3 + treeHeight, pos.y + treeHeight)) {
			int yo = yy - (pos.y + treeHeight);
			int offs = 2 - yo / 2;

			for (auto xx : range_incl(pos.x - offs, pos.x + offs)) {
				for (auto zz : range_incl(pos.z - offs, pos.z + offs)) {
					_addJungleFeatures(region, BlockPos(xx, yy, zz), random);
				}
			}
		}

		// also chance for cocoa plants around stem
		if (random.nextInt(5) == 0 && treeHeight > 5) {
			_addCocoaPlants(region, pos, random, treeHeight);
		}
	}

	// place dirt underneath tree once grown.
	_placeBlock(region, pos.below(), FullBlock(Block::mDirt->mID));

	return true;
}

void TreeFeature::_addVine(BlockSource& region, const BlockPos& pos, int dir) const {
	_placeBlock(region, pos, FullBlock(Block::mVine->mID, dir));

	int maxDir = 4;
	BlockPos blockPos = pos.below();
	while (region.getBlockID(blockPos) == 0 && maxDir > 0) {
		_placeBlock(region, blockPos, FullBlock(Block::mVine->mID, dir));
		maxDir--;
		blockPos = blockPos.below();
	}
}

bool TreeFeature::_isFree(BlockID block) const {
	const Block* tt = Block::mBlocks[block];
	if (tt != nullptr) {
		return _isFree(tt->getMaterial())
			|| tt == Block::mGrass
			|| tt == Block::mDirt;
	}
	return true;
}

bool TreeFeature::_isFree(const Material& material) const {
	return material.isType(MaterialType::Air)
		|| material.isType(MaterialType::Leaves);
}

int TreeFeature::_getTrunkType() const {
	return mTrunkType;
}

int TreeFeature::_getLeafType() const {
	return mLeafType;
}

void TreeFeature::_addJungleFeatures(BlockSource& region, const BlockPos& vinePos, Random& random) const {
	if (region.getBlockID(vinePos) == Block::mLeaves->mID) {
		BlockPos randomPos = vinePos.west();
		if (random.nextInt(4) == 0 && region.getBlockID(randomPos) == 0) {
			_addVine(region, randomPos, VineBlock::VINE_EAST);
		}

		randomPos = vinePos.east();
		if (random.nextInt(4) == 0 && region.getBlockID(randomPos) == 0) {
			_addVine(region, randomPos, VineBlock::VINE_WEST);
		}

		randomPos = vinePos.north();
		if (random.nextInt(4) == 0 && region.getBlockID(randomPos) == 0) {
			_addVine(region, randomPos, VineBlock::VINE_SOUTH);
		}

		randomPos = vinePos.south();
		if (random.nextInt(4) == 0 && region.getBlockID(randomPos) == 0) {
			_addVine(region, randomPos, VineBlock::VINE_NORTH);
		}
	}
}

void TreeFeature::_addCocoaPlants(BlockSource& region, const BlockPos& pos, Random& random, int treeHeight) const {
	for (auto rows : range(2)) {
		for (auto dir : range(4)) {
			if (random.nextInt(4 - rows) == 0) {
				int age = random.nextInt(3);
				BlockPos cocoaPos = pos + BlockPos(Direction::STEP_X[Direction::DIRECTION_OPPOSITE[dir]], treeHeight - 5 + rows, Direction::STEP_Z[Direction::DIRECTION_OPPOSITE[dir]]);
				_placeBlock(region, cocoaPos, FullBlock(Block::mCocoa->mID, (age << 2) | dir));
			}
		}
	}
}
