/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/
#pragma once

#include "client/renderer/chunks/VisibilityNode.h"
#include "world/level/BlockPos.h"
// #include "Core/Timing/time.h"
#include "world/level/ChunkBlockPos.h"
// #include "platform/threading/ThreadLocal.h"

class Block;
class RenderChunk;

class VisibilityExtimator {
public:

// 	static ThreadLocal<VisibilityExtimator> pool;

// 	Stopwatch timer;

	enum class BlockState : char {
		TS_EMPTY = 0,
		TS_OPAQUE,
		TS_EMPTY_MARKED
	};

	VisibilityExtimator();

	void start(RenderChunk& parent);

	void setBlock(const BlockPos& pos, const Block* t);

	bool isAllOpaque() const;
	bool isAllEmpty() const;

	VisibilityNode finish();

protected:

	BlockPos mOrigin;
	int mEmptyBlocks;
	BlockState mBlocks[RENDERCHUNK_VOLUME];
	std::deque<ChunkBlockPos> mFloodQueue;

	BlockState& _at(const ChunkBlockPos& t);
	BlockState* _at(const ChunkBlockPos& t, ByteMask& touchedSides);

	BlockState& _atWorld(const BlockPos& t);

	void _visit(const ChunkBlockPos& p, ByteMask& set);
	ByteMask _floodFill(const ChunkBlockPos& p);

};
