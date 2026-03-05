#include "Dungeons.h"
#include "EndGatewayFeature.h"
#include "world/level/BlockPos.h"
#include "world/level/BlockSource.h"
#include "world/level/block/Block.h"

bool EndGatewayFeature::place(BlockSource& region, const BlockPos& origin, Random& random) const {
	for (int x = -1; x <= 1; ++x) {
		for (int y = -2; y <= 2; ++y) {
			for (int z = -1; z <= 1; ++z) {
				BlockPos pos = BlockPos(origin.x + x, origin.y + y, origin.z + z);
				if (x == 0 && y == 0 && z == 0) {
					_setBlockAndData( region, pos, Block::mEndGateway->mID);
				}
				else if (y == 0) {
					_setBlockAndData( region, pos, Block::mAir->mID);
				}
				else if ((y == -2 || y == 2) && x == 0 && z == 0) {
					_setBlockAndData( region, pos, FullBlock(Block::mBedrock->mID, 1));
				}
				else if ((x != 0 && z != 0) || (y == -2 || y == 2)) {
					_setBlockAndData( region, pos, Block::mAir->mID);
				}
				else {
					_setBlockAndData( region, pos, FullBlock(Block::mBedrock->mID, 1));
				}
			}
		}
	}
	return true;
}

void EndGatewayFeature::testEnd(BlockSource& region, const BlockPos& origin, Random& random) const {
	for (int x = -300; x <= 300; ++x) {
		for (int y = 4; y <= 4; ++y) {
			for (int z = -300; z <= 300; ++z) {
				BlockPos pos = BlockPos(origin.x + x, y, origin.z + z);
				if ((x > -3 && x < 3) && (y < 3) && (z > -3 && z < 3)) {
					_setBlockAndData( region, pos, Block::mEndStone->mID);
				}
				//else if ((x < -40 || x > 40) && (y < 3) && (z < -30 || z > 30)) {
				//	_setBlockAndData( region, pos, Block::mEndStone->mID);
				//}
				else {
					_setBlockAndData( region, pos, Block::mAir->mID);
				}
			}
		}
	}
}