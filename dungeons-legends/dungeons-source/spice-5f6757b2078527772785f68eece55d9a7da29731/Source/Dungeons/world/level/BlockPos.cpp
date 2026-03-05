#include "Dungeons.h"

#include "world/level/BlockPos.h"
#include "world/level/ChunkPos.h"
#include "world/level/LevelConstants.h"

#include "world/phys/Vec3.h"
#include "world/Facing.h"

const BlockPos BlockPos::MIN(INT_MIN, INT_MIN, INT_MIN);
const BlockPos BlockPos::MAX(INT_MAX, INT_MAX, INT_MAX );
const BlockPos BlockPos::ONE(1, 1, 1);
const BlockPos BlockPos::ZERO;

BlockPos::BlockPos(const Vec3& v) :
	BlockPos(v.x, v.y, v.z) {

}

BlockPos::BlockPos(const ChunkPos& cp, int y) :
	BlockPos(cp.x * CHUNK_WIDTH, y, cp.z * CHUNK_DEPTH) {

}

BlockPos::BlockPos(float x, float y, float z) 
	: x((int)floorf(x))
	, y((int)floorf(y))
	, z((int)floorf(z)) {

}

BlockPos BlockPos::neighbor(FacingID direction) const {
	return *this + Facing::DIRECTION[direction];
}

Vec3 BlockPos::center() const {
	return Vec3((float)x, (float)y, (float)z) + Vec3(0.5f, 0.5f, 0.5f);
}

BlockPos BlockPos::relative(FacingID facing, int steps) const {
	return BlockPos(x + Facing::getStepX(facing) * steps, y + Facing::getStepY(facing) * steps, z + Facing::getStepZ(facing) * steps);
}

BlockPosIterator::BlockPosIterator(const BlockPos& a, const BlockPos& b)
	: mMinCorner(BlockPos::min(a, b))
	, mMaxCorner(BlockPos::max(a, b))
	, mCurrentPos(mMinCorner)
	, mDone(false) {
}

const BlockPos& BlockPosIterator::operator*() {
	return mCurrentPos;
}

bool BlockPosIterator::operator!=(const BlockPosIterator& other) const {
	return mMinCorner != other.mMinCorner ||
		   mMaxCorner != other.mMaxCorner ||
		   mCurrentPos != other.mCurrentPos ||
		   mDone != other.mDone;
}

BlockPosIterator& BlockPosIterator::operator++() {
	if (mCurrentPos.z < mMaxCorner.z) {
		++mCurrentPos.z;
	}
	else if (mCurrentPos.y < mMaxCorner.y) {
		mCurrentPos.z = mMinCorner.z;
		++mCurrentPos.y;
	}
	else if (mCurrentPos.x < mMaxCorner.x) {
		mCurrentPos.z = mMinCorner.z;
		mCurrentPos.y = mMinCorner.y;
		++mCurrentPos.x;
	}
	else {
		mDone = true;
	}

	return *this;
}

BlockPosIterator BlockPosIterator::begin() const {
	return BlockPosIterator(mMinCorner, mMaxCorner);
}

BlockPosIterator BlockPosIterator::end() const {
	BlockPosIterator endIt(mMinCorner, mMaxCorner);
	endIt.mCurrentPos = mMaxCorner;
	endIt.mDone = true;
	return endIt;
}

//
// Iterator
//
BlockPosIteration::Iterator::Iterator(const BlockPos& min, const BlockPos& max, uint64_t startIdx /*= 0*/)
	: BlockPos(min)
	, mMin(min)
	, mMax(max)
	, mIdx(startIdx) {
}

BlockPosIteration::Iterator& BlockPosIteration::Iterator::operator++() {
	++x;
	++mIdx;
	if (x >= mMax.x) {
		x = mMin.x;
		++z;
		if (z >= mMax.z) {
			z = mMin.z;
			++y;
		}
	}
	return *this;
}

BlockPos& BlockPosIteration::Iterator::operator*() {
	return *this;
}

bool BlockPosIteration::Iterator::operator!=(const Iterator& rhs) {
	return rhs.mIdx != mIdx;
}

BlockPosIteration::BlockPosIteration(const BlockPos& min, const BlockPos& max)
	: mMin(min)
	, mMax(max)
	, mVolume((max - min).product())
{
}

BlockPosIteration BlockPosIteration::range(const BlockPos& pos) {
	return BlockPosIteration(BlockPos(), pos);
}

BlockPosIteration BlockPosIteration::range(const BlockPos& min, const BlockPos& max) {
	return BlockPosIteration(min, max);
}

BlockPosIteration::Iterator BlockPosIteration::begin() const {
	return BlockPosIteration::Iterator(mMin, mMax, 0);
}

BlockPosIteration::Iterator BlockPosIteration::end() const {
	return BlockPosIteration::Iterator(mMin, mMax, mVolume);
}
