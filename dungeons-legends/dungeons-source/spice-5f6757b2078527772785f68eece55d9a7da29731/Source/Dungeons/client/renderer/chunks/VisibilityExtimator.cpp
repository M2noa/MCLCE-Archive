/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/
#include "Dungeons.h"

#include "client/renderer/chunks/VisibilityExtimator.h"
#include "client/renderer/chunks/RenderChunk.h"
#include "client/renderer/chunks/VisibilityNode.h"
#include "world/level/block/Block.h"

//create our thread-local visibility extimator
// ThreadLocal<VisibilityExtimator> VisibilityExtimator::pool([] (){
// 	return make_unique<VisibilityExtimator>();
// });

VisibilityExtimator::VisibilityExtimator() {
}

VisibilityExtimator::BlockState& VisibilityExtimator::_at(const ChunkBlockPos& p){
	return mBlocks[p.y + p.z * RENDERCHUNK_SIDE + p.x * RENDERCHUNK_COLUMNS];
}

VisibilityExtimator::BlockState* VisibilityExtimator::_at(const ChunkBlockPos& pos, ByteMask& set) {
	static_assert(
		std::is_same< decltype(pos.x), uint8_t >::value &&
		std::is_same< decltype(pos.z), uint8_t >::value &&
		std::is_same< decltype(pos.y), int16_t >::value,
		"Types have changed, check this code");

	if (pos.x > 128) {
		set.add(Facing::WEST);
	}
	else if (pos.x >= RENDERCHUNK_SIDE) {
		set.add(Facing::EAST);
	}
	else if (pos.y < 0) {
		set.add(Facing::DOWN);
	}
	else if (pos.y >= RENDERCHUNK_SIDE) {
		set.add(Facing::UP);
	}
	else if (pos.z > 128) {
		set.add(Facing::NORTH);
	}
	else if (pos.z >= RENDERCHUNK_SIDE) {
		set.add(Facing::SOUTH);
	}
	else{
		return &_at(pos);
	}
	return nullptr;
}

void VisibilityExtimator::_visit(const ChunkBlockPos& p, ByteMask& set){
	auto block = _at(p, set);
	if (block && *block == BlockState::TS_EMPTY) {
		mFloodQueue.push_back(p);
	}
}

ByteMask VisibilityExtimator::_floodFill(const ChunkBlockPos& startPos) {
	ByteMask set;

	DEBUG_ASSERT(mFloodQueue.empty(), "Floodstack should never contain elements outside here");

	mFloodQueue.push_back(startPos);

	while (!mFloodQueue.empty()) {

		auto p = mFloodQueue.front();
		mFloodQueue.pop_front();

		auto& block = _at(p);
		if (block == BlockState::TS_EMPTY) {
			block = BlockState::TS_EMPTY_MARKED;
			--mEmptyBlocks;
			DEBUG_ASSERT(mEmptyBlocks >= 0, "Visited more tiles than empty tiles?");

			//visit all the neighbors
			for (auto& dir : Facing::DIRECTIONS) {
				_visit(p.neighbor(dir), set);
			}
		}
	}

	return set;
}

bool VisibilityExtimator::isAllEmpty() const {
	return mEmptyBlocks >= RENDERCHUNK_VOLUME - (RENDERCHUNK_SIDE * RENDERCHUNK_SIDE);
}

VisibilityNode VisibilityExtimator::finish() {
	if (isAllEmpty()) {
		return VisibilityNode(true);
	}
	else {
		//the algorithm is as follows:
		//for each non-empty, non-marked block,
		//-flood fill
		//-mark each flooded block and add each side touched by the fill to the set
		//-connect all the sides touched
		VisibilityNode res;
		ChunkBlockPos p;

		for (p.x = 0; p.x < RENDERCHUNK_SIDE; p.x++) {
			for (p.z = 0; p.z < RENDERCHUNK_SIDE; p.z++) {
				for (p.y = 0; p.y < RENDERCHUNK_SIDE; p.y++) {

					if (_at(p) == BlockState::TS_EMPTY) {//whoa new floodfill
						res.connect(_floodFill(p));

						if (isAllEmpty()) {
							return res;
						}
					}
				}
			}
		}
		return res;
	}
}

void VisibilityExtimator::start(RenderChunk& parent) {
	mOrigin = parent.getPosition();
	memset(mBlocks, 0, sizeof(mBlocks));
	mEmptyBlocks = RENDERCHUNK_VOLUME;
}

void VisibilityExtimator::setBlock(const BlockPos& pos, const Block* t){
	if (t->isSolid()) {
		_atWorld(pos) = BlockState::TS_OPAQUE;
		--mEmptyBlocks;
	}
}

bool VisibilityExtimator::isAllOpaque() const {
	return mEmptyBlocks == 0;
}

VisibilityExtimator::BlockState& VisibilityExtimator::_atWorld(const BlockPos& t) {
	auto diff = t - mOrigin;
	DEBUG_ASSERT(diff.x >= 0 && diff.x < RENDERCHUNK_SIDE && diff.y >= 0 && diff.y < RENDERCHUNK_SIDE && diff.z >= 0 && diff.z < RENDERCHUNK_SIDE, "OOB");
	return _at(ChunkBlockPos(diff.x, diff.y, diff.z));
}
