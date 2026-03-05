#include "Dungeons.h"

#include "BlockRegion.h"
#include "world/level/BlockPos.h"

BlockRegion::BlockRegion(const RegionSize& size)
    : mSize(size)
    , mBlocks((int) size.product())
    , mXZPlaneSize(size.x * size.z)
{
}

BlockRegion::BlockRegion(BlockRegion&& other) noexcept
	: mSize(other.mSize)
	, mBlocks(std::move(other.mBlocks))
	, mXZPlaneSize(other.mXZPlaneSize)
{
}

FullBlock BlockRegion::getBlock(int x, int y, int z) const {
    if (_inRange(x, y, z)) {
        return mBlocks[_toLinearIndex(x, y, z)];
    } else {
        return FullBlock::AIR;
    }
}

FullBlock BlockRegion::getBlock(const RegionPos& pos) const {
    return getBlock(pos.x, pos.y, pos.z);
}

BlockID BlockRegion::getBlockId(int x, int y, int z) const {
    if (_inRange(x, y, z)) {
        return mBlocks[_toLinearIndex(x, y, z)].id;
    } else {
        return BlockID::AIR;
    }
}

BlockID BlockRegion::getBlockId(const RegionPos& pos) const {
    return getBlockId(pos.x, pos.y, pos.z);
}

void BlockRegion::setBlock(int x, int y, int z, const FullBlock& block) {
    if (_inRange(x, y, z)) {
        mBlocks[_toLinearIndex(x, y, z)] = block;
    }
}

void BlockRegion::setBlock(const RegionPos& pos, const FullBlock& block) {
    setBlock(pos.x, pos.y, pos.z, block);
}

void BlockRegion::setBlock(int x, int y, int z, BlockID block) {
    if (_inRange(x, y, z)) {
        mBlocks[_toLinearIndex(x, y, z)] = FullBlock(block);
    }
}

void BlockRegion::setBlock(const RegionPos& pos, BlockID block) {
    setBlock(pos.x, pos.y, pos.z, block);
}

BlockRegion::RegionSize BlockRegion::size() const {
    return mSize;
}

bool BlockRegion::operator==(const BlockRegion& rhs) const {
	return mSize == rhs.mSize && mBlocks == rhs.mBlocks;
}

bool BlockRegion::operator!=(const BlockRegion& rhs) const {
	return !(rhs == *this);
}

//
// Internal
//
int BlockRegion::_toLinearIndex(int x, int y, int z) const {
    return y * mXZPlaneSize + z * mSize.x + x;
}

bool BlockRegion::_inRange(int x, int y, int z) const {
    return x >= 0 && x < mSize.x && y >= 0 && y < mSize.y && z >= 0 && z < mSize.z;
}
