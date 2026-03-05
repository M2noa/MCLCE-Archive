/********************************************************
*   (c) Mojang. All rights reserved                       *
*   (c) Microsoft. All rights reserved.                   *
*********************************************************/
#pragma once

#include <cmath>
#include <functional>
#include <cstdlib>

#include "network/BinaryStream.h"

class BlockPos;
class Vec3;
class Entity;

class ChunkPos {
public:

	const static ChunkPos INVALID;

	int x, z;

	ChunkPos()
		:   x(0)
		, z(0){
	}

	ChunkPos(int x_, int z_)
		:   x(x_)
		, z(z_){
	}

	ChunkPos(const BlockPos& pos);

	ChunkPos(const Vec3& pos);

	int hashCode() const {
		return (x * 0x1f1f1f1f) ^ z;
	}

	static int hashCode(int x, int z) {
		return (x * 0x1f1f1f1f) ^ z;
	}

	bool operator==(const ChunkPos& rhs) const {
		return x == rhs.x && z == rhs.z;
	}

	bool operator!=(const ChunkPos& rhs) const {
		return x != rhs.x || z != rhs.z;
	}

	ChunkPos& operator=(const ChunkPos& rhs) {
		x = rhs.x;
		z = rhs.z;
		return *this;
	}

	ChunkPos operator+(const ChunkPos& rhs) const {
		return ChunkPos(x + rhs.x, z + rhs.z);
	}

	ChunkPos operator-(const ChunkPos& rhs) const {
		return ChunkPos(x - rhs.x, z - rhs.z);
	}

	ChunkPos operator+(int t) const {
		return ChunkPos(x + t, z + t);
	}

	ChunkPos operator-(int t) const {
		return ChunkPos(x - t, z - t);
	}

	int distanceToSqr(const ChunkPos& pos) const {
		const int a = pos.x - x;
		const int b = pos.z - z;
		return a * a + b * b;
	}

	float distanceToSqr(const Entity& e) const;

	ChunkPos north() const {
		return ChunkPos(x, z - 1);
	}

	ChunkPos south() const {
		return ChunkPos(x, z + 1);
	}

	ChunkPos east() const {
		return ChunkPos(x - 1, z);
	}

	ChunkPos west() const {
		return ChunkPos(x + 1, z);
	}

	bool isNeighbor(const ChunkPos& p) const {
		const int dx = std::abs(p.x - x);
		const int dz = std::abs(p.z - z);
		return dx <= 1 && dz <= 1 && p != *this;
	}

	int getMiddleBlockX() const {
		return (x << 4) + 8;
	}

	int getMiddleBlockZ() const {
		return (z << 4) + 8;
	}

	BlockPos getMiddleBlockPosition(int y) const;

};

//specialization for serialization
template<>
struct serialize<ChunkPos>{
	void static write(const ChunkPos& val, BinaryStream& stream){
		stream.writeVarInt(val.x);
		stream.writeVarInt(val.z);
	}

	ChunkPos static read(ReadOnlyBinaryStream& stream){
		auto x = stream.getVarInt();
		auto z = stream.getVarInt();
		return ChunkPos(x, z);
	}
};

namespace std {
	template<>
	struct hash<ChunkPos>{

		size_t operator()(const ChunkPos& x) const {
			return x.hashCode();
		}

	};

}
