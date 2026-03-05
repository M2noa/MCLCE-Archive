/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/
#pragma once

class BlockPos;

#include "world/Facing.h"
#include "world/level/LevelConstants.h"

class ChunkBlockPos {

public:

	uint8_t x, z;
	Height y;

	explicit ChunkBlockPos(uint8_t i) :
		x(i)
		, y(i)
		, z(i) {

	}

	ChunkBlockPos() :
		ChunkBlockPos(0) {
	}

	ChunkBlockPos(uint8_t _x, Height _y, uint8_t _z) 
		: x(_x)
		, y(_y)
		, z(_z) {
	}

	explicit ChunkBlockPos( const BlockPos& pos );

	uint16_t index() const {
		DEBUG_ASSERT( isInChunk(), "Using invalid Chunk tile pos");

		return y + (z + x * CHUNK_WIDTH) * LEVEL_HEIGHT_DEPRECATED;
	}

	uint8_t index2D() const {
		DEBUG_ASSERT( isInChunk2D(), "Using invalid Chunk tile pos");

		return z * CHUNK_WIDTH + x;
	}

	uint8_t ZMajorIndex2D() const {
		DEBUG_ASSERT(isInChunk2D(), "Using invalid Chunk tile pos");

		return x * CHUNK_WIDTH + z;
	}

	bool isInChunk() const {
		return x < CHUNK_WIDTH && y < LEVEL_HEIGHT_DEPRECATED && z < CHUNK_DEPTH;
	}

	bool isInChunk2D() const {
		return x < CHUNK_WIDTH && z < CHUNK_DEPTH;
	}

	size_t hashCode() const {
		return ((size_t)x) | (((size_t)y) << 8) | (((size_t)z) << 16);
	}

	bool operator==(const ChunkBlockPos& rhs) const {
		return x == rhs.x && y == rhs.y && z == rhs.z;
	}

	ChunkBlockPos above() const {
		return ChunkBlockPos(x, y + 1, z);
	}

	ChunkBlockPos below() const {
		return ChunkBlockPos(x, y - 1, z);
	}

	ChunkBlockPos neighbor(Facing::Name facing) const {
		auto& d = Facing::DIRECTION[facing];
		return ChunkBlockPos(x + d.x, y + d.y, z + d.z);
	}

	BlockPos operator+(const BlockPos& p) const;

	ChunkBlockPos operator+(const ChunkBlockPos& p) const {
		return ChunkBlockPos(p.x + x, p.y + y, p.z + z);
	}

};

namespace std {
	///hash specialization for unordered_maps
	template<>
	struct hash<ChunkBlockPos>{
		// hash functor for vector
		size_t operator()(const ChunkBlockPos& _Keyval) const {
			return _Keyval.hashCode();
		}

	};

}
