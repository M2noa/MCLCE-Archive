/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#include "world/level/block/LiquidBlockDynamic.h"
#include "world/level/BlockSource.h"
#include "world/level/Level.h"
#include "world/level/material/Material.h"
// #include "world/level/BlockTickingQueue.h"
#include "world/level/dimension/Dimension.h"

LiquidBlockDynamic::LiquidBlockDynamic(const std::string& nameId, int id, const Material& material)
	: LiquidBlock(nameId, id, material) {
	setTicking(true);
}

void LiquidBlockDynamic::_spread(BlockSource& region, const BlockPos& pos, int depth) const{
	DEBUG_ASSERT(pos.y >= 0, "Invalid spread position");

	region.setBlockAndData(pos, mID, depth, Block::UPDATE_ALL);

// 	if (region.getTickQueue(pos)->isInstaticking()) {	//when instaticking, no updates are sent, sent manually
// 		region.updateNeighborsAt(pos);
// 	}
// 
// 	region.getTickQueue(pos)->add(region, pos, mID, getTickDelay(region));
}

void LiquidBlockDynamic::_trySpreadTo(BlockSource& region, const BlockPos& pos, int neighbor) const{
// 	if (_canSpreadTo(region, pos)) {
// 		//dont create any effect when instaticking
// 		BlockTickingQueue* tickQueue = region.getTickQueue(pos);
// 		if (tickQueue != nullptr) {
// 			if (!tickQueue->isInstaticking()) {
// 				BlockID old = region.getBlockID(pos);
// 
// 				// don't bother spawning
// 				if (old > 0) {
// 					if (mMaterial.isType(MaterialType::Lava)) {
// 						emitFizzParticle(region, pos);
// 					} else if (Block::mBlocks[old]->waterSpreadCausesSpawn()) {
// 						Block::mBlocks[old]->spawnResources(region, pos, region.getData(pos), 1.0f);
// 					}
// 				}
// 			}
// 
// 			_spread(region, pos, neighbor);
// 		}
// 	}
}

void LiquidBlockDynamic::tick(BlockSource& region, const BlockPos& pos, Random& random) const{
	BlockPos below = pos.below();

	int depth = getDepth(region, pos);
	int dropOff = 1;
	if (mMaterial.isType(MaterialType::Lava) && !region.getDimensionConst().isUltraWarm()) {
		dropOff = 2;
	}

	//put stuff on fire
	if (mMaterial.isType(MaterialType::Lava)) {
		trySpreadFire(region, pos, random);
	}

	bool becomeStatic = true;
	int tickDelay = getTickDelay(region);
	if (depth > 0) {
		int highest = -100;
		int maxCount = 0;
		highest = _getHighest(region, pos.west(), highest, maxCount);
		highest = _getHighest(region, pos.east(), highest, maxCount);
		highest = _getHighest(region, pos.south(), highest, maxCount);
		highest = _getHighest(region, pos.north(), highest, maxCount);

		int newDepth = highest + dropOff;
		if (newDepth >= 8 || highest < 0) {
			newDepth = -1;
		}

		if (getDepth(region, pos.above()) >= 0) {
			int above = getDepth(region, pos.above());
			if (above >= 8) {
				newDepth = above;
			} else{
				newDepth = above + 8;
			}
		}

		if (maxCount >= 2 && mMaterial.isType(MaterialType::Water)) {
			// Only spread spring if it's on top of an existing spring, or on top of solid ground.
			if (region.getMaterial(below).isSolid()) {
				newDepth = 0;
			} else if (region.getMaterial(below) == mMaterial && region.getData(below) == 0) {
				newDepth = 0;
			}
		}

		if (mMaterial.isType(MaterialType::Lava)) {
			if (depth < 8 && newDepth < 8) {
				if (newDepth > depth) {
					if (random.nextInt(4) != 0) {
						tickDelay = tickDelay * 4;
					}
				}
			}
		}

		if (newDepth != depth) {
			depth = newDepth;
			if (depth < 0) {
				region.setBlockAndData(pos, FullBlock::AIR, Block::UPDATE_ALL);
			} else {
				DataID data = 0;
				getBlockState(BlockState::LiquidDepth).set(data, depth);

				region.setBlockAndData(pos, mID, data, Block::UPDATE_ALL);
				region.updateNeighborsAt(pos);
// 				region.getTickQueue(pos)->add(region, pos, mID, tickDelay);	//tickdelay makes this instant if needed
			}
		} else {
			if (becomeStatic) {
				_setStatic(region, pos);
			}
		}
	} else {
		_setStatic(region, pos);
	}

	if (_canSpreadTo(region, below )) {
		//convert to stone if the block is water
		if (mMaterial.isType(MaterialType::Lava)) {
			if (region.getMaterial(below).isType(MaterialType::Water)) {
				region.setBlockAndData(below, Block::mStone->mID, Block::UPDATE_ALL);

				emitFizzParticle(region, below);

				return;
			}
		}

		if (depth >= 8) {
			_trySpreadTo(region, below, depth);
		} else {
			_trySpreadTo(region, below, depth + 8);
		}

	} else if (depth >= 0 && (depth == 0 || _isWaterBlocking(region, below))) {
		std::array<bool, 4> spreads = _getSpread(region, pos);

		int neighbor = depth + dropOff;
		if (depth >= 8) {
			neighbor = 1;
		}
		if (neighbor >= 8) {
			return;
		}
		if (spreads[0]) {
			_trySpreadTo(region, pos.west(), neighbor);
		}
		if (spreads[1]) {
			_trySpreadTo(region, pos.east(), neighbor);
		}
		if (spreads[2]) {
			_trySpreadTo(region, pos.north(), neighbor);
		}
		if (spreads[3]) {
			_trySpreadTo(region, pos.south(), neighbor);
		}
	}
}

int LiquidBlockDynamic::_getHighest(BlockSource& region, const BlockPos& pos, int current, int& outMaxCount) const {
	int d = getDepth(region, pos);
	if (d < 0) {
		return current;
	}

	if (d == 0) {
		outMaxCount++;
	}

	if (d >= 8) {
		d = 0;
	}

	return current < 0 || d < current ? d : current;
}

void LiquidBlockDynamic::_setStatic(BlockSource& region, const BlockPos& pos) const{
	region.setBlockAndData(pos, (BlockID)(mID + 1), region.getData(pos), Block::UPDATE_CLIENTS);
}

bool LiquidBlockDynamic::_canSpreadTo(BlockSource& region, const BlockPos& pos) const{
	if (pos.y < 0 || !region.hasBlock(pos)) {
		return false;
	} else {
		const Material& target = region.getMaterial(pos);
		if (target == mMaterial) {
			return false;
		} else if (target.isType(MaterialType::Lava)) {
			return false;
		} else {
			return !_isWaterBlocking(region, pos);
		}
	}
}

bool LiquidBlockDynamic::_isWaterBlocking(BlockSource& region, const BlockPos& pos) const{
	auto& t = region.getBlock(pos);

	return t.isWaterBlocking();
}

std::array<bool, 4> LiquidBlockDynamic::_getSpread(BlockSource& region, const BlockPos& pos) const {
	std::array<int, NUM_DIRECTIONS> dist;
	for (int d = 0; d < 4; d++) {
		dist[d] = 1000;
		BlockPos pp = pos;

		if (d == 0) {
			pp.x--;
		}
		if (d == 1) {
			pp.x++;
		}
		if (d == 2) {
			pp.z--;
		}
		if (d == 3) {
			pp.z++;
		}

		if (_isWaterBlocking(region, pp)) {
			continue;
		} else if (region.getMaterial(pp) == mMaterial) {
			int depth = getBlockState(BlockState::LiquidDepth).get<int>(region.getData(pp));
			if (depth == 0) {
				continue;
			}
		} else {
			if (!_isWaterBlocking(region, pp.below())) {
				dist[d] = 0;
			} else {
				dist[d] = _getSlopeDistance(region, pp, 1, d);
			}
		}
	}

	int lowest = dist[0];

	for (int d = 1; d < 4; d++) {
		if (dist[d] < lowest) {
			lowest = dist[d];
		}
	}

	std::array<bool, 4> result;
	for (int d = 0; d < 4; d++) {
		result[d] = dist[d] == lowest;
	}

	return result;
}

int LiquidBlockDynamic::_getSlopeDistance(BlockSource& region, const BlockPos& pos, int pass, int from) const {
	int lowest = 1000;

	for (int d = 0; d < 4; d++) {
		if (d == 0 && from == 1) {
			continue;
		}
		if (d == 1 && from == 0) {
			continue;
		}
		if (d == 2 && from == 3) {
			continue;
		}
		if (d == 3 && from == 2) {
			continue;
		}

		BlockPos pp(pos);

		if (d == 0) {
			pp.x--;
		}
		if (d == 1) {
			pp.x++;
		}
		if (d == 2) {
			pp.z--;
		}
		if (d == 3) {
			pp.z++;
		}

		if (_isWaterBlocking(region, pp)) {
			continue;
		} else if (region.getMaterial(pp) == mMaterial) {
			int depth = getBlockState(BlockState::LiquidDepth).get<int>(region.getData(pp));
			if (depth == 0) {
				continue;
			}
		} else {
			if (!_isWaterBlocking(region, pp.below())) {
				return pass;
			} else {
				if (pass < 4) {
					int v = _getSlopeDistance(region, pp, pass + 1, d);
					if (v < lowest) {
						lowest = v;
					}
				}
			}
		}
	}
	return lowest;
}

void LiquidBlockDynamic::onPlace(BlockSource& region, const BlockPos& pos) const {
	LiquidBlock::onPlace(region, pos);
// 	if (region.getBlockID(pos) == mID) {
// 		region.getTickQueue(pos)->add(region, pos, mID, getTickDelay(region));
// 	}
}
