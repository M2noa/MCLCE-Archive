#pragma once

class ChunkBlockPos;

#include <cstdint>

#include "world/Facing.h"
#include "world/level/LevelConstants.h"
#include "ChunkBlockPos.h"

class SubChunkBlockPos {
public:
	uint8_t x, y, z;

	SubChunkBlockPos(uint8_t _x, uint8_t _y, uint8_t _z)
		: x(_x)
		, y(_y)
		, z(_z) {
	}

	explicit SubChunkBlockPos(uint8_t i) : SubChunkBlockPos(i, i, i) {}

	SubChunkBlockPos() :
		SubChunkBlockPos(0) {
	}

	explicit SubChunkBlockPos(const ChunkBlockPos& pos) 
		: x(pos.x)
		, y(pos.y % 16)
		, z(pos.z) {

	}

	uint16_t index() const {
		DEBUG_ASSERT(isInChunk(), "Using invalid Chunk tile pos");

		return y + (z + x * CHUNK_WIDTH) * CHUNK_WIDTH;
	}

	uint8_t index2D() const {
		DEBUG_ASSERT(isInChunk(), "Using invalid Chunk tile pos");

		return z * CHUNK_WIDTH + x;
	}

	uint8_t ZMajorIndex2D() const {
		DEBUG_ASSERT(isInChunk(), "Using invalid Chunk tile pos");

		return x * CHUNK_WIDTH + z;
	}

	bool isInChunk() const {
		return x < CHUNK_WIDTH && y < CHUNK_WIDTH && z < CHUNK_WIDTH;
	}

	size_t hashCode() const {
		return ((size_t)x) | (((size_t)y) << 8) | (((size_t)z) << 16);
	}

	bool operator==(const SubChunkBlockPos& rhs) const {
		return x == rhs.x && y == rhs.y && z == rhs.z;
	}

	SubChunkBlockPos above() const {
		return SubChunkBlockPos(x, y + 1, z);
	}

	SubChunkBlockPos below() const {
		return SubChunkBlockPos(x, y - 1, z);
	}

	SubChunkBlockPos neighbor(Facing::Name facing) const {
		auto& d = Facing::DIRECTION[facing];
		return SubChunkBlockPos(x + d.x, y + d.y, z + d.z);
	}

	BlockPos operator+(const BlockPos& p) const;

	SubChunkBlockPos operator+(const SubChunkBlockPos& p) const {
		return SubChunkBlockPos(p.x + x, p.y + y, p.z + z);
	}

};

namespace std {
	///hash specialization for unordered_maps
	template<>
	struct hash<SubChunkBlockPos> {
		// hash functor for vector
		size_t operator()(const SubChunkBlockPos& _Keyval) const {
			return _Keyval.hashCode();
		}

	};

}
