#pragma once

#include "Core/Utility/buffer_span.h"
#include "CommonTypes.h"
#include "world/Pos.h"
#include "util/NibblePair.h"
#include "world/level/ChunkBlockPos.h"

//a class that wraps memory to provide access as a block getVolume
class BlockVolume {
public:
	BlockVolume(buffer_span_mut<uint8_t> buffer, int width, int height, int depth) 
		: mWidth(width)
		, mHeight(height)
		, mDepth(depth) {
		DEBUG_ASSERT(mWidth > 0 && mHeight > 0 && mDepth > 0, "Invalid dimensions");
		DEBUG_ASSERT(buffer.byte_size() >= getVolume() + getVolume() / 2, "Buffer is too small");

		auto ptr = buffer.data();
		blocks = { reinterpret_cast<BlockID*>(ptr), reinterpret_cast<BlockID*>(ptr + getVolume()) };

		ptr += blocks.byte_size();
		data = { reinterpret_cast<NibblePair*>(ptr), reinterpret_cast<NibblePair*>(ptr + getVolume() / 2) };
	}

	///constructor with just blocks, no data
	BlockVolume(buffer_span_mut<BlockID> buffer, int width, int height, int depth)
		: mWidth(width)
		, mHeight(height)
		, mDepth(depth) {
		DEBUG_ASSERT(mWidth > 0 && mHeight > 0 && mDepth > 0, "Invalid dimensions");
		DEBUG_ASSERT(buffer.byte_size() == getVolume(), "Wrong buffer size");

		auto ptr = buffer.data();
		blocks = { reinterpret_cast<BlockID*>(ptr), reinterpret_cast<BlockID*>(ptr + getVolume()) };
	}

	buffer_span_mut<BlockID> blocks;
	buffer_span_mut<NibblePair> data;

	uint32_t getVolume() const {
		return mWidth * mHeight * mDepth;
	}

	uint32_t getHeight() const {
		return mHeight;
	}
	
	uint32_t index(const Pos& pos) const {
		DEBUG_ASSERT(pos.x >= 0 && pos.x < (int)mWidth && pos.y >= 0 && pos.y < (int)mHeight && pos.z >= 0 && pos.z < (int)mDepth, "Out of bounds");

		return pos.y + (pos.z + pos.x * mWidth) * mHeight;
	}

	uint32_t index(const ChunkBlockPos& pos) const {
		DEBUG_ASSERT(pos.x >= 0 && pos.x < (int)mWidth && pos.y >= 0 && pos.y < (int)mHeight && pos.z >= 0 && pos.z < (int)mDepth, "Out of bounds");

		return pos.y + (pos.z + pos.x * mWidth) * mHeight;
	}

	BlockID& block(uint32_t index) {
		return blocks[index];
	}

	BlockID& block(const Pos& pos) {
		return blocks[index(pos)];
	}

	BlockID& block(const ChunkBlockPos& pos) {
		return blocks[index(pos)];
	}

	const DataID getData(const Pos& pos) const {
		auto idx = index(pos);
		auto& pair = data[idx / 2];
		if (idx % 2 == 0) {
			return pair.first;
		}
		else {
			return pair.second;
		}
	}

	void setBlock(uint32_t index, BlockID id) {
		blocks[index] = id;
	}

	void setBlock(const Pos& pos, BlockID id) {
		blocks[index(pos)] = id;
	}

	void setBlock(const ChunkBlockPos& pos, BlockID id) {
		blocks[index(pos)] = id;
	}

	void setData(const Pos& pos, DataID val) {
		auto idx = index(pos);
		auto& pair = data[idx / 2];
		if (idx % 2 == 0) {
			pair.first = val;
		}
		else {
			pair.second = val;
		}
	}

	void setData(const ChunkBlockPos& pos, DataID val) {
		setData(Pos{ pos.x, pos.y, pos.z }, val);
	}

private:
	uint32_t mWidth, mHeight, mDepth;
};

