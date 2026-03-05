#pragma once

#include "world/level/BlockPos.h"
#include "world/level/LevelConstants.h"

struct BlockCuboid {
	BlockPos minInclusive;
	BlockPos maxExclusive;

	BlockCuboid() = default;
	BlockCuboid(const BlockCuboid& cuboid) = default;
	BlockCuboid(const BlockPos& minInclusive, const BlockPos& maxExclusive);
	BlockCuboid& operator=(const BlockCuboid& rhs) = default;

	static BlockCuboid fromPositionAndSize(const BlockPos& minInclusive, int sizeX, int sizeY, int sizeZ);
	static BlockCuboid fromPositionAndSize(const BlockPos& minInclusive, const BlockPos& size);
	static BlockCuboid fromInclusiveCorners(const BlockPos& a, const BlockPos& b);
	static BlockCuboid fromSize(const BlockPos& size);

	BlockPos size() const;
	int area() const;
	bool isEmpty() const;
	uint64_t volume() const;
	BlockPos maxInclusive() const { return maxExclusive - 1; }

	bool fits(unsigned int sizeX, unsigned int sizeY, unsigned int sizeZ) const;
	bool fits(BlockPos size) const;
	bool fitsXZ(unsigned int sizeX, unsigned int sizeZ) const;
	bool fitsXZ(BlockPos size) const;

	void expand(const BlockPos& include);
	void expand(const BlockCuboid& include);

	bool containsX(int x) const;
	bool containsY(int y) const;
	bool containsZ(int z) const;
	bool contains(const BlockPos&) const;
	bool contains(const BlockCuboid&) const;
	bool containsXZ(const BlockPos&) const;
	bool containsXZ(int x, int z) const;
	bool containsXZ(const BlockCuboid&) const;

	bool intersects(const BlockCuboid& other) const;
	bool intersectsXZ(const BlockCuboid& other) const;
	BlockCuboid intersection(const BlockCuboid& other) const;

	BlockPosIteration::Iterator begin() const;
	BlockPosIteration::Iterator end() const;

	BlockCuboid operator+(const BlockPos&) const;
	BlockCuboid operator-(const BlockPos&) const;
	BlockCuboid& operator+=(const BlockPos&);
	BlockCuboid& operator-=(const BlockPos&);
	bool operator==(const BlockCuboid&) const;
	bool operator!=(const BlockCuboid&) const;

	std::string toString();
};

const BlockCuboid MaxCuboid = {
	BlockPos(INT_MIN, 0, INT_MIN),
	BlockPos(INT_MAX, LEVEL_HEIGHT_DEPRECATED, INT_MAX)
};

const uint64_t MaxSaneCuboidVolume = 128 * 128 * 128; // Arbitrary big number

BlockCuboid squareCuboidFromCenter(BlockPos center, int radiusXZ);
BlockCuboid cuboidFromCenter(BlockPos center, int radiusXYZ);
BlockCuboid cuboidFromCenter(BlockPos center, int radiusXZ, int radiusY);

BlockCuboid transformed(const BlockCuboid&, const BlockPosTransform&);
