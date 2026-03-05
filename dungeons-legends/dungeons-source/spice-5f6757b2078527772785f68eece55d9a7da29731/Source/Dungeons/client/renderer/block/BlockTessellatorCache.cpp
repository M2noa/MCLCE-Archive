#include "Dungeons.h"
#include "client/renderer/block/BlockTessellatorCache.h"
#include "world/level/block/Block.h"
#include "world/level/BlockPos.h"
#include "world/level/BlockSource.h"

const Block& BlockTessellatorCache::mUnsetBlockValue = *Block::mInfoUpdateGame1;// Unset value

void BlockTessellatorCache::reset(BlockSource& region, const BlockPos& startPostion){
	mRegion = &region;
	// we can always read one outside the chunk on each side
	pos = startPostion.west(2).north(2).below(2);

	for(auto& lightColor : mLightColors) {
		lightColor.block = lightColor.sky = Brightness::INVALID;
	}

	for(auto& t : mBlocks) {
		t = &mUnsetBlockValue;
	}

	for(auto& d : mData) {
		d = 255;// Unset value
	}
}

BrightnessPair BlockTessellatorCache::getLightColor(const BlockPos& p) {
	if (p.y >= mRegion->getMaxHeight()) {
		return{ Brightness::MAX, Brightness::MIN }; //just skylight
	}
	int index = _getIndexFromPositionInArray(_getPosInArray(p));

	auto cachedLightColor = mLightColors[index];
	if(cachedLightColor.block != Brightness::INVALID && cachedLightColor.sky != Brightness::INVALID) {
		return cachedLightColor;
	}

	const Block& block = getBlock(p);
	auto light = mRegion->getLightColor(p, _getLightEmission(block));

	if(light.sky == 0 && light.block == 0) {
		if(block.hasProperty(BlockProperty::HalfSlab)) {
			const Block& blockBelow = getBlock(p.below());
			// We need to get the raw value here, can't used the cached one since that one
			// can be changed
			light = mRegion->getLightColor(p.below(), _getLightEmission(blockBelow));
		}
	}

	return light;
}

Brightness BlockTessellatorCache::_getLightEmission(const Block& block) {
	return Block::getLightEmission(block.mID);
}

const Block& BlockTessellatorCache::getBlock(const BlockPos& p) {
	if(p.y >= mRegion->getMaxHeight() && p.y < 0) {
		return *Block::mAir;
	}

	int index = _getIndexFromPositionInArray(_getPosInArray(p));
	const Block* cachedBlock = mBlocks[index];
	if(cachedBlock != &mUnsetBlockValue) {
		return *cachedBlock;
	}

	auto& checkedBlock = mRegion->getBlock(p);
	mBlocks[index] = &checkedBlock;
	return *mBlocks[index];
}

BlockPos BlockTessellatorCache::_getPosInArray(const BlockPos& p) {
	DEBUG_ASSERT(p.x >= pos.x && p.y >= pos.y && p.z >= pos.z, "Point is outside cache, should be possible.");
	DEBUG_ASSERT(p.x - pos.x < CACHE_SIDE_X && p.y - pos.y < CACHE_SIDE_Y && p.z - pos.z < CACHE_SIDE_Z, "Point is outside cache, should be possible.");

	return p - pos;
}

int BlockTessellatorCache::_getIndexFromPositionInArray(const BlockPos& p) {
	DEBUG_ASSERT(p.x >= 0 && p.y >= 0 && p.z >= 0, "Point is outside cache, should be possible.");
	DEBUG_ASSERT(p.x < CACHE_SIDE_X && p.y < CACHE_SIDE_Y && p.z < CACHE_SIDE_Z, "Point is outside cache, should be possible.");
	// TODO: Check if this order is optimal based on rendering ordering.
	//       Since we read all sides, perhaps it makes no difference.
	return (p.x * CACHE_SIDE_Y * CACHE_SIDE_Z) + (p.y * CACHE_SIDE_Z) + p.z;
}

void BlockTessellatorCache::setBlockAtPosition(const Block& block, BlockPos& p) {
	int index = _getIndexFromPositionInArray(_getPosInArray(p));
	mBlocks[index] = &block;
}

DataID BlockTessellatorCache::getData(const BlockPos& _pos) {
	if(_pos.y >= mRegion->getMaxHeight() && _pos.y < 0) {
		return 0;
	}

	int index = _getIndexFromPositionInArray(_getPosInArray(_pos));
	DataID cachedData = mData[index];
	if(cachedData != 255) {
		return cachedData;
	}

	DataID checkedData = mRegion->getData(_pos);
	mData[index] = checkedData;
	return checkedData;
}
