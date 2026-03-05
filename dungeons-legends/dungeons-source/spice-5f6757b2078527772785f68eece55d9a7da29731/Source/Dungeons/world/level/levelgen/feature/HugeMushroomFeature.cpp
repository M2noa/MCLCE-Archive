#include "Dungeons.h"

#include "world/level/levelgen/feature/HugeMushroomFeature.h"
#include "util/Random.h"
#include "world/level/LevelConstants.h"
#include "world/level/BlockSource.h"
#include "world/level/biome/Biome.h"
#include "world/level/block/Block.h"
#include "world/level/material/Material.h"

HugeMushroomFeature::HugeMushroomFeature()
	: mForcedType(-1) {
}

HugeMushroomFeature::HugeMushroomFeature(int forcedType)
	: mForcedType(forcedType) {
}

bool HugeMushroomFeature::place(BlockSource& region, const BlockPos& pos, Random& random) const {
	BlockPos blockPos(pos);
	int type = (mForcedType >= 0) ? mForcedType : random.nextInt(2);

	int treeHeight = random.nextInt(3) + 4;
	if (random.nextInt(12) == 0) {
		treeHeight *= 2;
	}

	bool free = true;
	if (blockPos.y < 1 || blockPos.y + treeHeight + 1 >= region.getMaxHeight()) {
		return false;
	}

	for (auto yy : range_incl(blockPos.y, blockPos.y + 1 + treeHeight)) {
		int r = (yy <= (blockPos.y + 3)) ? 0 : 3;

		for (int xx = blockPos.x - r; xx <= blockPos.x + r && free; xx++) {
			for (int zz = blockPos.z - r; zz <= blockPos.z + r && free; zz++) {
				if (yy >= 0 && yy < region.getMaxHeight()) {
					
					auto& mat = region.getMaterial({ xx, yy, zz });
					if (!mat.isType(MaterialType::Air) && !mat.isType(MaterialType::Leaves)) {
						free = false;
					}
				}
				else {
					free = false;
				}
			}
		}
	}

	if (!free) {
		return false;
	}
		
	BlockID oneBlockBelow = region.getBlockID(blockPos.below());
	if (!_canSurvive(oneBlockBelow)) {
		
		// Unique to MCPE, allow huge mushrooms to spawn in shallow (1 or 2 block deep) swamp water.
		if (region.getBiome(blockPos).getBiomeType() == Biome::BiomeType::Swamp) {
			
			BlockID twoBlocksBelow = region.getBlockID(blockPos.below(2));
			BlockID threeBlocksBelow = region.getBlockID(blockPos.below(3));
			
			// if 1 below is water and 2 below is survivable 
			// OR if 1 and 2 below is water and 3 below is survivable... allow the mushroom
			if ((oneBlockBelow == Block::mStillWater->mID) && 
				(_canSurvive(twoBlocksBelow) || (twoBlocksBelow == Block::mStillWater->mID && _canSurvive(threeBlocksBelow)))) {
				
				// grow the tree into the water
				int moveAmount = (twoBlocksBelow == Block::mStillWater->mID) ? 2 : 1;
				blockPos.y -= moveAmount;
				treeHeight += moveAmount;
			}
			else {
				return false;
			}
		}
		else {
			return false;
		}
	}

	int low = blockPos.y + treeHeight;
	if (type == 1) {
		low = blockPos.y + treeHeight - 3;
	}

	for (auto yy : range_incl(low, blockPos.y + treeHeight)) {
		int offs = 1;
		if (yy < blockPos.y + treeHeight) {
			offs += 1;
		}
		if (type == 0) {
			offs = 3;
		}

		for (auto xx : range_incl(blockPos.x - offs, blockPos.x + offs)) {
			for (auto zz : range_incl(blockPos.z - offs, blockPos.z + offs)) {
				int data = 5;
				if (xx == blockPos.x - offs) {
					data--;
				}
				if (xx == blockPos.x + offs) {
					data++;
				}
				if (zz == blockPos.z - offs) {
					data -= 3;
				}
				if (zz == blockPos.z + offs) {
					data += 3;
				}

				if (type == 0 || yy < blockPos.y + treeHeight) {
					if ((xx == blockPos.x - offs || xx == blockPos.x + offs) && (zz == blockPos.z - offs || zz == blockPos.z + offs)) {
						continue;
					}
					if (xx == blockPos.x - (offs - 1) && zz == blockPos.z - offs) {
						data = 1;
					}
					if (xx == blockPos.x - offs && zz == blockPos.z - (offs - 1)) {
						data = 1;
					}

					if (xx == blockPos.x + (offs - 1) && zz == blockPos.z - offs) {
						data = 3;
					}
					if (xx == blockPos.x + offs && zz == blockPos.z - (offs - 1)) {
						data = 3;
					}

					if (xx == blockPos.x - (offs - 1) && zz == blockPos.z + offs) {
						data = 7;
					}
					if (xx == blockPos.x - offs && zz == blockPos.z + (offs - 1)) {
						data = 7;
					}

					if (xx == blockPos.x + (offs - 1) && zz == blockPos.z + offs) {
						data = 9;
					}
					if (xx == blockPos.x + offs && zz == blockPos.z + (offs - 1)) {
						data = 9;
					}
				}

				if (data == 5 && yy < blockPos.y + treeHeight) {
					data = 0;
				}
				if (data != 0 || blockPos.y >= blockPos.y + treeHeight - 1) {
					// TODO [EB] Get rid of weird offset logic!
					BlockPos mushroomPos(xx, yy, zz);
					if (!region.isSolidBlockingBlock(mushroomPos)) {
						_setBlockAndData(region, mushroomPos, FullBlock((BlockID)(Block::mBrownMushroomBlock->mID + type), data));
					}
				}
			}
		}
	}

	for (auto hh : range(treeHeight)) {
		BlockPos abovePos = blockPos.above(hh);
		// TODO [EB] Get rid of weird offset logic!
		if (!region.isSolidBlockingBlock(abovePos)) {
			_setBlockAndData(region, abovePos, FullBlock((BlockID)(Block::mBrownMushroomBlock->mID + type), 10));
		}
	}

	return true;
}

bool HugeMushroomFeature::_canSurvive(BlockID belowBlock) const {
	return belowBlock == Block::mDirt->mID || belowBlock == Block::mGrass->mID || belowBlock == Block::mMycelium->mID || belowBlock == Block::mPodzol->mID;
}
