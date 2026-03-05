#include "Dungeons.h"

#include "BlockCuboid.h"

static bool lessThanOrEquals(BlockPos a, BlockPos b) {
	return a.x <= b.x && a.y <= b.y && a.z <= b.z;
}

BlockCuboid::BlockCuboid(const BlockPos& minInclusive, const BlockPos& maxExclusive)
	: minInclusive(minInclusive)
	, maxExclusive(maxExclusive)
{
}

BlockCuboid BlockCuboid::fromPositionAndSize(const BlockPos& minInclusive, int sizeX, int sizeY, int sizeZ) {
	return BlockCuboid(minInclusive, minInclusive.offset(sizeX, sizeY, sizeZ));
}

BlockCuboid BlockCuboid::fromPositionAndSize(const BlockPos& minInclusive, const BlockPos& size) {
	return BlockCuboid(minInclusive, minInclusive + size);
}

BlockCuboid BlockCuboid::fromInclusiveCorners(const BlockPos& a, const BlockPos& b) {
	return BlockCuboid(BlockPos::min(a, b), BlockPos::max(a, b) + 1);
}

BlockCuboid BlockCuboid::fromSize(const BlockPos& size) {
	return BlockCuboid(BlockPos(), size);
}

BlockPos BlockCuboid::size() const {
	return maxExclusive - minInclusive;
}

int BlockCuboid::area() const {
	return std::abs((maxExclusive.x - minInclusive.x) * (maxExclusive.z - minInclusive.z));
}

bool BlockCuboid::isEmpty() const {
	return (minInclusive.x == maxExclusive.x) || (minInclusive.y == maxExclusive.y) || (minInclusive.z == maxExclusive.z);
}

uint64_t BlockCuboid::volume() const {
	return std::abs(size().product());
}

bool BlockCuboid::fits(unsigned int sizeX, unsigned int sizeY, unsigned int sizeZ) const {
	return lessThanOrEquals(minInclusive + BlockPos((int)sizeX, (int)sizeY, (int)sizeZ), maxExclusive);
}

bool BlockCuboid::fits(BlockPos size) const {
	DEBUG_ASSERT(size.isPositive(), "Size must be >= 0");
	return lessThanOrEquals(minInclusive + size, maxExclusive);
}

bool BlockCuboid::fitsXZ(unsigned int sizeX, unsigned int sizeZ) const {
	return minInclusive.x + (int)sizeX <= maxExclusive.x && minInclusive.z + (int)sizeZ <= maxExclusive.z;
}

bool BlockCuboid::fitsXZ(BlockPos size) const {
	DEBUG_ASSERT(size.x >= 0 && size.z >= 0, "Size must be >= 0");
	return minInclusive.x + size.x <= maxExclusive.x && minInclusive.z + size.z <= maxExclusive.z;
}

void BlockCuboid::expand(const BlockPos& include) {
	minInclusive = BlockPos::min(minInclusive, include);
	maxExclusive = BlockPos::max(maxExclusive, include + 1);
}

void BlockCuboid::expand(const BlockCuboid& include) {
	expand(include.minInclusive);
	expand(include.maxInclusive());
}

bool BlockCuboid::containsX(int x) const {
	return x >= minInclusive.x && x < maxExclusive.x;
}

bool BlockCuboid::containsY(int y) const {
	return y >= minInclusive.y && y < maxExclusive.y;
}

bool BlockCuboid::containsZ(int z) const {
	return z >= minInclusive.z && z < maxExclusive.z;
}

bool BlockCuboid::contains(const BlockPos& pos) const {
	return pos.x >= minInclusive.x && pos.x < maxExclusive.x
		&& pos.y >= minInclusive.y && pos.y < maxExclusive.y
		&& pos.z >= minInclusive.z && pos.z < maxExclusive.z;
}

bool BlockCuboid::containsXZ(const BlockPos& pos) const {
	return pos.x >= minInclusive.x && pos.x < maxExclusive.x && pos.z >= minInclusive.z && pos.z < maxExclusive.z;
}

bool BlockCuboid::containsXZ(int x, int z) const {
	return x >= minInclusive.x && x < maxExclusive.x && z >= minInclusive.z && z < maxExclusive.z;
}

bool BlockCuboid::containsXZ(const BlockCuboid& other) const {
	return other.minInclusive.x >= minInclusive.x && other.maxExclusive.x <= maxExclusive.x
		&& other.minInclusive.z >= minInclusive.z && other.maxExclusive.z <= maxExclusive.z;
}

bool BlockCuboid::contains(const BlockCuboid& other) const {
	return other.minInclusive.x >= minInclusive.x && other.maxExclusive.x <= maxExclusive.x
		&& other.minInclusive.y >= minInclusive.y && other.maxExclusive.y <= maxExclusive.y
		&& other.minInclusive.z >= minInclusive.z && other.maxExclusive.z <= maxExclusive.z;
}

bool BlockCuboid::intersects(const BlockCuboid& other) const {
	return !(maxExclusive.x <= other.minInclusive.x || minInclusive.x >= other.maxExclusive.x
	      || maxExclusive.z <= other.minInclusive.z || minInclusive.z >= other.maxExclusive.z
	      || maxExclusive.y <= other.minInclusive.y || minInclusive.y >= other.maxExclusive.y);
}

bool BlockCuboid::intersectsXZ(const BlockCuboid& other) const {
	return !(maxExclusive.x <= other.minInclusive.x || minInclusive.x >= other.maxExclusive.x
	      || maxExclusive.z <= other.minInclusive.z || minInclusive.z >= other.maxExclusive.z);
}

BlockCuboid BlockCuboid::intersection(const BlockCuboid& other) const {
	BlockPos min = BlockPos::max(minInclusive, other.minInclusive);
	BlockPos max = BlockPos::min(maxExclusive, other.maxExclusive);
	return lessThanOrEquals(min, max) ? BlockCuboid(min, max) : BlockCuboid();
}

BlockPosIteration::Iterator BlockCuboid::begin() const {
	return BlockPosIteration::Iterator(minInclusive, maxExclusive, 0);
}

BlockPosIteration::Iterator BlockCuboid::end() const {
	return BlockPosIteration::Iterator(minInclusive, maxExclusive, volume());
}

BlockCuboid BlockCuboid::operator+(const BlockPos& offset) const {
	return BlockCuboid(minInclusive + offset, maxExclusive + offset);
}

BlockCuboid BlockCuboid::operator-(const BlockPos& offset) const {
	return BlockCuboid(minInclusive - offset, maxExclusive - offset);
}

BlockCuboid& BlockCuboid::operator+=(const BlockPos& offset) {
	minInclusive += offset;
	maxExclusive += offset;
	return *this;
}

BlockCuboid& BlockCuboid::operator-=(const BlockPos& offset) {
	minInclusive -= offset;
	maxExclusive -= offset;
	return *this;
}

std::string BlockCuboid::toString() {
	std::stringstream ss;
	ss << "BlockCuboid((" << minInclusive.x << "," << minInclusive.y << ","  << minInclusive.z << "), (" << maxExclusive.x << "," << maxExclusive.y << "," << maxExclusive.z << "))";
	return ss.str();
}

bool BlockCuboid::operator==(const BlockCuboid& rhs) const {
	return minInclusive == rhs.minInclusive && maxExclusive == rhs.maxExclusive;
}

bool BlockCuboid::operator!=(const BlockCuboid& rhs) const {
	return !(rhs == *this);
}

BlockCuboid squareCuboidFromCenter(BlockPos center, int radiusXZ) {
	return cuboidFromCenter(center, radiusXZ, LEVEL_HEIGHT_DEPRECATED);
}

BlockCuboid cuboidFromCenter(BlockPos center, int radiusXYZ) {
	return cuboidFromCenter(center, radiusXYZ, radiusXYZ);
}

BlockCuboid cuboidFromCenter(BlockPos center, int radiusXZ, int radiusY) {
	radiusXZ--;
	radiusY--;
	return BlockCuboid(
		BlockPos(center.x - radiusXZ, std::max(0, center.y - radiusY), center.z - radiusXZ),
		BlockPos(1 + center.x + radiusXZ, std::min((int) LEVEL_HEIGHT_DEPRECATED, 1 + center.y + radiusY), 1 + center.z + radiusXZ)
	);
}

BlockCuboid transformed(const BlockCuboid& volume, const BlockPosTransform& transform) {
	return BlockCuboid::fromInclusiveCorners(transform(volume.minInclusive), transform(volume.maxInclusive()));
}
