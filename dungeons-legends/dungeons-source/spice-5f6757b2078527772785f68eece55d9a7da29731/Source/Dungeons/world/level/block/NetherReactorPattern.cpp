/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#include "world/level/block/NetherReactorPattern.h"
#include "world/level/block/Block.h"

NetherReactorPattern::NetherReactorPattern( ) {
	const int goldId = Block::mGoldBlock->mID;
	const int stoneId = Block::mCobblestone->mID;
	const int netherCoreId = Block::mNetherReactor->mID;
	const int types[3][3][3] = {
		// Level 0
		{
			{goldId, stoneId, goldId},
			{stoneId, stoneId, stoneId},
			{goldId, stoneId, goldId}
		},
		// Level 1
		{
			{stoneId, 0, stoneId},
			{0, netherCoreId, 0},
			{stoneId, 0, stoneId}
		},
		// Level 2
		{
			{0, stoneId, 0},
			{stoneId, stoneId, stoneId},
			{0, stoneId, 0}
		}
	};

	for(auto setLevel : range_incl(2)) {
		for(auto setX : range_incl(2)) {
			for(auto setZ : range_incl(2)) {
				setBlockAt(setLevel, setX, setZ, types[setLevel][setX][setZ]);
			}
		}
	}
}

void NetherReactorPattern::setBlockAt(int level, int x, int z, int block) {
	pattern[level][x][z] = block;
}

unsigned int NetherReactorPattern::getBlockAt(int level, int x, int z) {
	return pattern[level][x][z];
}
