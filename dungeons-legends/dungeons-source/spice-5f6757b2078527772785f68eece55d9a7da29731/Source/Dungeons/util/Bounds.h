/********************************************************
*   (c) Mojang. All rights reserved                       *
*   (c) Microsoft. All rights reserved.                   *
*********************************************************/
#pragma once

#include "common_header.h"

#include "world/Pos.h"
#include "world/level/BlockPos.h"
#include "util/Math.h"
#include "world/level/LevelConstants.h"
#include "world/phys/Vec3.h"

#define UNIFORM_COORD(X, S) (X >> 4)

//bounds of this area in chunk space
struct Bounds {

	struct Iterator : public Pos {

		Iterator(const Bounds& base, int startIdx = 0) :
			Pos( base.mMin )
			, mBounds(base)
			, mIdx(startIdx){

		}

		Iterator& operator++() {
			++x;
			++mIdx;
			if (x > mBounds.mMax.x) {
				x = mBounds.mMin.x;
				++z;
				if (z > mBounds.mMax.z) {
					z = mBounds.mMin.z;
					++y;
				}
			}
			return *this;
		}

		Pos& operator*() {
			return *this;
		}

		bool operator!=(const Iterator& rhs) {
			return rhs.mIdx != mIdx;
		}

		inline int index() {
			return mIdx;
		}

protected:

		const Bounds& mBounds;
		int mIdx;
	};

	Pos mMin, mMax;
	Pos mDim;
	int mArea = 0, mVolume = 0, mSide = 0;

	Pos gridToChunk(const BlockPos& p) const {
		return Pos(
			UNIFORM_COORD(p.x, mSide),
			Math::clamp(p.y, 0, (int)(LEVEL_HEIGHT_DEPRECATED - 1)) / mSide,
			UNIFORM_COORD(p.z, mSide));
	}

	Bounds()
		: mMin()
		, mMax()
		, mDim()
		, mArea(0)
		, mVolume(0)
		, mSide(0) {
	}

	Bounds(const BlockPos& minBlock, const BlockPos& maxBlock, int side, bool flatten = false) :
		mSide(side) {

		DEBUG_ASSERT(minBlock.x <= maxBlock.x && minBlock.y <= maxBlock.y && minBlock.z <= maxBlock.z, "Max need to be less and equal than min");
		DEBUG_ASSERT(side > 0, "Chunk side can't be 0");

		auto min = gridToChunk(minBlock);
		auto max = gridToChunk(maxBlock);
		if (flatten) {
			min.y = max.y = 0;
		}

		//convert this absolute grid rect in chunks
		resize( min, max);
	}

	Bounds(const BlockPos& pos, int blockRadius, int chunkSide, bool flatten = false) :
		Bounds(pos - blockRadius, pos + blockRadius, chunkSide, flatten) {
	}

	Bounds(const ChunkPos& min, const ChunkPos& max) :
		Bounds(min, max, CHUNK_WIDTH, true) {

	}

	void resize(const Pos& cmin, const Pos& cmax) {
		mMin = cmin;
		mMax = cmax;

		mDim = mMax - mMin + 1;
		mArea = mDim.x * mDim.z;
		mVolume = mArea * mDim.y;
	}

	bool contains(const Pos& p) const {
		return (mArea > 0) &&
			   p.x >= mMin.x && p.x <= mMax.x &&
			   p.y >= mMin.y && p.y <= mMax.y &&
			   p.z >= mMin.z && p.z <= mMax.z;
	}

	bool contains(const BlockPos& p) const {
		return contains(Pos(
			UNIFORM_COORD(p.x, mSide),
			UNIFORM_COORD(p.y, mSide),
			UNIFORM_COORD(p.z, mSide)));
	}

	///true if the given area is contained in this bounds
	bool contains(const Bounds& innerArea) const {
		return contains(innerArea.mMax) && contains(innerArea.mMin);	//TODO make faster?
	}

	int idx(const Pos& p) const {
		return p.x + p.z * mDim.x + p.y * mArea;
	}

	int idxFromWorldPos(const Pos& p) const {
		return idx(p - mMin);
	}

	int idxFromWorldPos(const BlockPos& p) const {
		return idxFromWorldPos(gridToChunk(p));
	}

	///warning: only makes sense if dim.x == dim.y == dim.z
	int getRadius() const {
		return (mDim.x * mSide) / 2;
	}

	///returns a float position representing the center of this bounds
	Vec3 getCenter() const {
		return Vec3((mMin.x + mMax.x) * 0.5f, (mMin.y + mMax.y) * 0.5f, (mMin.z + mMax.z) * 0.5f);
	}

	Iterator begin() const {
		return Iterator(*this, 0);
	}

	Iterator end() const {
		return Iterator(*this, mVolume);
	}

	bool operator ==(const Bounds& rhs) {
		return (mMin == rhs.mMin &&
			mMax == rhs.mMax &&
			mDim == rhs.mDim &&
			mArea == rhs.mArea &&
			mVolume == rhs.mVolume &&
			mSide == rhs.mSide);
	}
};
