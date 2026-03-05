/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/
#include "Dungeons.h"

#include "world/level/ChunkPos.h"
#include "world/level/BlockPos.h"
#include "world/phys/Vec3.h"
#include "world/entity/Entity.h"
#include "world/level/LevelConstants.h"

const ChunkPos ChunkPos::INVALID(INT_MIN, INT_MIN);

ChunkPos::ChunkPos(const BlockPos& pos) :
	x(pos.x >> 4)
	, z(pos.z >> 4){
	REQUIRES_PC_CHUNK;
}

ChunkPos::ChunkPos(const Vec3& pos) :
	ChunkPos(BlockPos(pos)){
}

float ChunkPos::distanceToSqr(const Entity& e) const {
	const float xPos = (float)(x * 16 + 8);
	const float zPos = (float)(z * 16 + 8);

	const float xd = xPos - e.mPos.x;
	const float zd = zPos - e.mPos.z;

	return xd * xd + zd * zd;
}

BlockPos ChunkPos::getMiddleBlockPosition(int y) const {
	return BlockPos(getMiddleBlockX(), y, getMiddleBlockZ());
}
