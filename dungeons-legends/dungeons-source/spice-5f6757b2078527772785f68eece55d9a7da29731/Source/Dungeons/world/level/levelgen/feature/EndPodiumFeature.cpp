/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/
#include "Dungeons.h"

#include "world/level/levelgen/feature/EndPodiumFeature.h"
#include "world/level/block/Block.h"
#include "util/Random.h"
#include "world/level/BlockSource.h"
#include "world/Direction.h"
#include "world/level/block/TorchBlock.h"
#include "world/Facing.h"

int EndPodiumFeature::PODIUM_RADIUS = 4;
int EndPodiumFeature::PODIUM_PILLAR_HEIGHT = 4;
int EndPodiumFeature::RIM_RADIUS = 1;
float EndPodiumFeature::CORNER_ROUNDING = 0.5f;
BlockPos EndPodiumFeature::END_PODIUM_LOCATION = BlockPos(0, 100, 0);
BlockPos EndPodiumFeature::END_PODIUM_CHUNK_POSITION = BlockPos((END_PODIUM_LOCATION.x - PODIUM_RADIUS) & ~0xF, 0, (END_PODIUM_LOCATION.z - PODIUM_RADIUS) & ~0xF);

EndPodiumFeature::EndPodiumFeature(bool active) :
	mActive(active) {
}

bool EndPodiumFeature::place(BlockSource& region, const BlockPos& origin, Random& random) const {
	for (int x = -PODIUM_RADIUS; x <= PODIUM_RADIUS; ++x) {
		for (int y = -1; y <= 32; ++y) {
			for (int z = -PODIUM_RADIUS; z <= PODIUM_RADIUS; ++z) {
				BlockPos pos = BlockPos(origin.x + x, origin.y + y, origin.z + z);
				float dist = pos.distSqr(BlockPos(origin.x, pos.y, origin.z));

				if (dist <= (PODIUM_RADIUS - CORNER_ROUNDING)*(PODIUM_RADIUS - CORNER_ROUNDING)) {
					if (pos.y < origin.y) {
						if (dist <= (PODIUM_RADIUS - RIM_RADIUS - CORNER_ROUNDING)*(PODIUM_RADIUS - RIM_RADIUS - CORNER_ROUNDING)) {
							// The floor/inside of the podium
							_setBlockAndData( region, pos, FullBlock(Block::mBedrock->mID, 1));
						}
						else if (pos.y < origin.y) {
							// The base below the podium should be endstone
							_setBlockAndData( region, pos, Block::mEndStone->mID);
						}
					}
					else if (pos.y > origin.y) {
						// Make sure nothing is above the podium
						_setBlockAndData( region, pos, Block::mAir->mID);
					}
					else if (dist > (PODIUM_RADIUS - RIM_RADIUS - CORNER_ROUNDING)*(PODIUM_RADIUS - RIM_RADIUS - CORNER_ROUNDING)) {
						// The rim around the podium
						_setBlockAndData( region, pos, FullBlock(Block::mBedrock->mID, 1));
					}
					else if (mActive) {
						// The "filling"
						_setBlockAndData( region, BlockPos(pos), Block::mEndPortal->mID);
					}
					else {
						_setBlockAndData( region, BlockPos(pos), Block::mAir->mID);
					}
				}
			}
		}
	}

	// Place the pillar in the middle
	for (int y = 0; y < PODIUM_PILLAR_HEIGHT; y++) {
		_setBlockAndData( region, origin.above(y), FullBlock(Block::mBedrock->mID, 1));
	}

	BlockPos centerOfPillar = origin.above(PODIUM_PILLAR_HEIGHT / 2);
	
	_setBlockAndData( region, centerOfPillar.relative(Facing::NORTH), {Block::mTorch->mID, (DataID)TorchBlock::TORCH_SOUTH});
	_setBlockAndData( region, centerOfPillar.relative(Facing::SOUTH), {Block::mTorch->mID, (DataID)TorchBlock::TORCH_NORTH});
	_setBlockAndData( region, centerOfPillar.relative(Facing::EAST), {Block::mTorch->mID, (DataID)TorchBlock::TORCH_WEST});
	_setBlockAndData( region, centerOfPillar.relative(Facing::WEST), {Block::mTorch->mID, (DataID)TorchBlock::TORCH_EAST});

	return true;
}