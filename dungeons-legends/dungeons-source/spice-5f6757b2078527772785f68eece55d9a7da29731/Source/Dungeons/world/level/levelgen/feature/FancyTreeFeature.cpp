#include "Dungeons.h"

#include "world/level/levelgen/feature/FancyTreeFeature.h"
#include "util/Random.h"
#include "util/Math.h"
#include "world/level/BlockSource.h"
#include "world/level/block/Block.h"
#include "world/level/block/OldLogBlock.h"
#include "world/level/block/LeafBlock.h"

FancyTreeFeature::FoliageCoords::FoliageCoords(BlockPos pos, int branchBase) :
	BlockPos(pos)
	, mBranchBase(branchBase) {

}

int FancyTreeFeature::FoliageCoords::getBranchBase() const {
	return mBranchBase;
}

FancyTreeFeature::FancyTreeFeature(Entity* placer)
	: TreeFeature(placer, enum_cast(OldLogBlock::LogType::Oak), LeafBlock::NORMAL_LEAF) {

}

bool FancyTreeFeature::place(BlockSource& region, const BlockPos& pos, Random& random) const {
	int height = 5 + random.nextInt(mHeightVariance);
	if (!_checkLocation(region, pos, height)) {
		return false;
	}

	int trunkHeight = (int)(height * mTrunkHeightScale);
	if (trunkHeight >= height) {
		trunkHeight = height - 1;
	}

	// Define foliage clusters per y
	int clustersPerY = (int)(1.382f + powf(mFoliageDensity * height / 13.0f, 2));
	if (clustersPerY < 1) {
		clustersPerY = 1;
	}

	const int trunkTop = pos.y + trunkHeight;
	int relativeY = height - mFoliageHeight;

	std::vector<FoliageCoords> foliageCoords;
	foliageCoords.push_back(FoliageCoords(pos.above(relativeY), trunkTop));

	for(; relativeY >= 0; relativeY--) {
		float treeShape = _getTreeShape(height, relativeY);
		if (treeShape < 0) {
			continue;
		}

		for (auto i : range(clustersPerY)) {
			UNUSED_VARIABLE(i);
			const float radius = mWidthScale * treeShape * (random.nextFloat() + 0.328f);
			const float angle = random.nextFloat() * 2 * PI;

			const float x = radius * Math::sin(angle) + 0.5f;
			const float z = radius * Math::cos(angle) + 0.5f;

			const BlockPos checkStart = pos.offset((int)x, relativeY - 1, (int)z);
			const BlockPos checkEnd = checkStart.above(mFoliageHeight);

			// check the center column of the cluster for obstructions.
			if (_checkLine(region, checkStart, checkEnd) == -1) {
				// If the cluster can be created, check the branch path for obstructions.
				const int dx = pos.x - checkStart.x;
				const int dz = pos.z - checkStart.z;

				const float clusterHeight = checkStart.y - Math::sqrt(float(dx * dx + dz * dz)) * mBranchSlope;
				const int branchTop = clusterHeight > trunkTop ? trunkTop : (int)clusterHeight;
				const BlockPos checkBranchBase(pos.x, branchTop, pos.z);

				// Now check the branch path
				if (_checkLine(region, checkBranchBase, checkStart) == -1) {
					// If the branch path is clear, add the position to the list of foliage positions
					foliageCoords.push_back(FoliageCoords(checkStart, checkBranchBase.y));
				}
			}
		}
	}

	_makeFoliage(region, foliageCoords);
	_makeTrunk(region, pos, trunkHeight);
	_makeBranches(region, foliageCoords, pos, height);
	return true;
}

float FancyTreeFeature::_getTreeShape(int height, int y) const {
	if (y < height * 0.3f) {
		return -1.0f;
	}

	const float radius = height / 2.0f;
	const float adjacent = radius - y;

	float distance = Math::sqrt(radius * radius - adjacent * adjacent);
	if (adjacent == 0) {
		distance = radius;
	}
	else if (std::abs(adjacent) >= radius) {
		return 0.0f;
	}

	return distance * 0.5f;
}

bool FancyTreeFeature::_checkLocation(BlockSource& region, const BlockPos& origin, int& height) const {
	// Return false if the tree can not be placed here.
	const BlockID belowBlock = region.getBlockID(origin.below());
	if (belowBlock != Block::mDirt->mID && belowBlock != Block::mGrass->mID && belowBlock != Block::mFarmland->mID) {
		return false;
	}

	int allowedHeight = _checkLine(region, origin, origin.above(height - 1));

	// If the set height is good, go with that
	if (allowedHeight == -1) {
		return true;
	}

	// If the space is too short, tell the build to abort
	if (allowedHeight < mMinAllowedHeight) {
		return false;
	}

	height = allowedHeight;
	return true;
}

int FancyTreeFeature::_checkLine(BlockSource& region, const BlockPos& startPos, const BlockPos& endPos) const {
	const BlockPos delta = endPos.offset(-startPos.x, -startPos.y, -startPos.z);

	const int steps = _getSteps(delta);

	const float dx = delta.x / (float)steps;
	const float dy = delta.y / (float)steps;
	const float dz = delta.z / (float)steps;

	for (auto i : range_incl(steps)) {
		const BlockPos blockPos = startPos.offset(int(.5f + i * dx), int(.5f + i * dy), int(.5f + i * dz));

		if (!_isFree(region.getBlockID(blockPos))) {
			return i;
		}
	}

	return -1;
}

int FancyTreeFeature::_getSteps(const BlockPos& pos) const {
	const int absX = std::abs(pos.x);
	const int absY = std::abs(pos.y);
	const int absZ = std::abs(pos.z);

	if (absZ > absX && absZ > absY) {
		return absZ;
	}
	else if (absY > absX) {
		return absY;
	}

	return absX;
}

void FancyTreeFeature::_makeFoliage(BlockSource& region, const std::vector<FoliageCoords>& foliageCoords) const {
	for (auto& foliageCoord : foliageCoords) {
		_foliageCluster(region, foliageCoord);
	}
}

void FancyTreeFeature::_foliageCluster(BlockSource& region, const FoliageCoords& foliageCoord) const {
	for (auto y : range(mFoliageHeight)) {
		_crossection(region, foliageCoord.above(y), _getFoliageShape(y), Block::mLeaves->mID);
	}
}

float FancyTreeFeature::_getFoliageShape(int y) const {
	if (y < 0 || y >= mFoliageHeight) {
		return -1.0f;
		// TODO: [EB]: Eventually test/change/fix this code!
	}
	else if (y == 0 || y == mFoliageHeight - 1) {
		return 2.0f;
		/* According to a 'Reddit comment' Jens had, the following code should fix the 'decaying leaves'.
		   -- Put instead of the else if above!
		   } else if (y == 0 || y == foliageHeight - 2) {
		   return 2f;
		   } else if (y == foliageHeight - 1) {
		   return 1.5f;
		 */
	}
	else {
		return 3.0f;
	}
}

void FancyTreeFeature::_crossection(BlockSource& region, const BlockPos& pos, float radius, const BlockID block) const {
	const int r = (int)(radius + 0.618f);

	for (auto dx : range_incl(-r, r)) {
		for (auto dz : range_incl(-r, r)) {
			if (std::pow(std::abs(dx) + 0.5f, 2) + std::pow(std::abs(dz) + 0.5f, 2) <= radius * radius) {
				const BlockPos blockPos = pos.offset(dx, 0, dz);

				// If the material of the checked block is anything other
				// than air or foliage, skip this block.
				if (_isFree(region.getMaterial(blockPos))) {
					_setBlockAndData(region, blockPos, FullBlock(block));
				}
			}
		}
	}
}

void FancyTreeFeature::_makeTrunk(BlockSource& region, const BlockPos& origin, int trunkHeight) const {
	// Create the trunk of the tree.
	const BlockPos end = origin.above(trunkHeight);
	const BlockID material = Block::mLog->mID;

	_limb(region, origin, end, material);
	if (mTrunkWidth == 2) {
		_limb(region, origin.east(), end.east(), material);
		_limb(region, origin.east().south(), end.east().south(), material);
		_limb(region, origin.south(), end.south(), material);
	}
}

void FancyTreeFeature::_limb(BlockSource& region, const BlockPos& startPos, const BlockPos& endPos, const BlockID material) const {
	const BlockPos delta = endPos.offset(-startPos.x, -startPos.y, -startPos.z);

	const int steps = _getSteps(delta);

	const float dx = delta.x / (float)steps;
	const float dy = delta.y / (float)steps;
	const float dz = delta.z / (float)steps;

	for (auto i : range_incl(steps)) {
		const BlockPos blockPos = startPos.offset(int(.5f + i * dx), int(.5f + i * dy), int(.5f + i * dz));
		const int data = _getLogFacingData(startPos, blockPos);

		_setBlockAndData(region, blockPos, FullBlock(material, data));
	}
}

int FancyTreeFeature::_getLogFacingData(const BlockPos& startPos, const BlockPos& blockPos) const {
	int dir = LogBlock::FACING_Y;
	int xdiff = std::abs(blockPos.x - startPos.x);
	int zdiff = std::abs(blockPos.z - startPos.z);
	int maxdiff = std::max(xdiff, zdiff);

	if (maxdiff > 0) {
		if (xdiff == maxdiff) {
			dir = LogBlock::FACING_X;
		}
		else if (zdiff == maxdiff) {
			dir = LogBlock::FACING_Z;
		}
	}

	DataID data = 0;
	Block::mLog->getBlockState(BlockState::Direction).set(data, dir);

	return dir;
}

void FancyTreeFeature::_makeBranches(BlockSource& region, const std::vector<FoliageCoords>& foliageCoords, const BlockPos& origin, int height) const {
	for (auto& endCoord : foliageCoords) {
		const int branchBase = endCoord.getBranchBase();
		const BlockPos baseCoord(origin.x, branchBase, origin.z);

		if (_trimBranches(height, branchBase - origin.y)) {
			_limb(region, baseCoord, endCoord, Block::mLog->mID);
		}
	}
}

bool FancyTreeFeature::_trimBranches(int height, int localY) const {
	return localY >= height * 0.2f;
}
