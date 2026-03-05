#include "Dungeons.h"

#include "world/level/ChunkBlockPos.h"

ChunkBlockPos::ChunkBlockPos(const BlockPos& pos) :
	ChunkBlockPos( pos.x & 15, pos.y, pos.z & 15 ){

}

BlockPos ChunkBlockPos::operator+(const BlockPos& p) const {
	return BlockPos(p.x + x, p.y + y, p.z + z);
}
