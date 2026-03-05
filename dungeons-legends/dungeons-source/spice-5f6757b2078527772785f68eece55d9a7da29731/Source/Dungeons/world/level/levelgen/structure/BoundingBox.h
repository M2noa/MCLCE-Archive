#pragma once

#include "world/Direction.h"
#include "util/Math.h"
#include "world/level/BlockPos.h"

class IntArrayTag;

class BoundingBox {
public:

	int x0, y0, z0, x1, y1, z1;

	BoundingBox()
		:
		x0(0)
		, y0(0)
		, z0(0)
		, x1(0)
		, y1(0)
		, z1(0){

	}

	BoundingBox(int* sourceData){
		//if (sourceData.length == 6) {
		x0 = sourceData[0];
		y0 = sourceData[1];
		z0 = sourceData[2];
		x1 = sourceData[3];
		y1 = sourceData[4];
		z1 = sourceData[5];
		//}
	}

	static BoundingBox getUnknownBox() {
		return BoundingBox(0x7fffffff, 0x7fffffff, 0x7fffffff, -(0x7fffffff), -(0x7fffffff), -(0x7fffffff));
	}

	static BoundingBox createProper(int x0, int y0, int z0, int x1, int y1, int z1) {
		return BoundingBox(Math::min(x0, x1), Math::min(y0, y1), Math::min(z0, z1), Math::max(x0, x1), Math::max(y0, y1), Math::max(z0, z1));
	}

	static BoundingBox orientBox(int footX, int footY, int footZ, int offX, int offY, int offZ, int width, int height, int depth, int orientation) {
		switch (orientation) {
		default:
			return BoundingBox(footX + offX, footY + offY, footZ + offZ, footX + width - 1 + offX, footY + height - 1 + offY, footZ + depth - 1 + offZ);
		case Direction::NORTH:
			// foot is at x0, y0, z1
			return BoundingBox(footX + offX, footY + offY, footZ - depth + 1 + offZ, footX + width - 1 + offX, footY + height - 1 + offY, footZ + offZ);
		case Direction::SOUTH:
			// foot is at x0, y0, z0
			return BoundingBox(footX + offX, footY + offY, footZ + offZ, footX + width - 1 + offX, footY + height - 1 + offY, footZ + depth - 1 + offZ);
		case Direction::WEST:
			// foot is at x1, y0, z0, but width and depth are flipped
			return BoundingBox(footX - depth + 1 + offZ, footY + offY, footZ + offX, footX + offZ, footY + height - 1 + offY, footZ + width - 1 + offX);
		case Direction::EAST:
			// foot is at x0, y0, z0, but width and depth are flipped
			return BoundingBox(footX + offZ, footY + offY, footZ + offX, footX + depth - 1 + offZ, footY + height - 1 + offY, footZ + width - 1 + offX);
		}
	}

	BoundingBox(const BoundingBox& other) {
		x0 = other.x0;
		y0 = other.y0;
		z0 = other.z0;
		x1 = other.x1;
		y1 = other.y1;
		z1 = other.z1;
	}

	BoundingBox(int _x0, int _y0, int _z0, int _x1, int _y1, int _z1) {
		x0 = _x0;
		y0 = _y0;
		z0 = _z0;
		x1 = _x1;
		y1 = _y1;
		z1 = _z1;
	}

	BoundingBox(int _x0, int _z0, int _x1, int _z1) {
		x0 = _x0;
		z0 = _z0;
		x1 = _x1;
		z1 = _z1;
		// the bounding box for this constructor is limited to world size,
		// excluding bedrock level
		y0 = 1;
		y1 = 512;
	}

	bool isValid() const {
		return x1 >= x0 && y1 >= y0 && z1 >= z0;
	}

	bool intersects(const BoundingBox& other) const {
		return !(x1 < other.x0 || x0 > other.x1 || z1 < other.z0 || z0 > other.z1 || y1 < other.y0 || y0 > other.y1);
	}

	bool intersects(int _x0, int _y0, int _z0, int _x1, int _y1, int _z1) const {
		return !(x1 < _x0 || x0 > _x1 || z1 < _z0 || z0 > _z1 || y1 < _y0 || y0 > _y1);
	}

	bool intersects(int _x0, int _z0, int _x1, int _z1) const {
		return !(x1 < _x0 || x0 > _x1 || z1 < _z0 || z0 > _z1);
	}

	void expand(const BoundingBox& other) {
		x0 = Math::min(x0, other.x0);
		y0 = Math::min(y0, other.y0);
		z0 = Math::min(z0, other.z0);
		x1 = std::max(x1, other.x1);
		y1 = std::max(y1, other.y1);
		z1 = std::max(z1, other.z1);
	}

	bool getIntersection(const BoundingBox& other, BoundingBox& out) const {
		if (!intersects(other)) {
			return false;
		}
		out.x0 = std::max(x0, other.x0);
		out.y0 = std::max(y0, other.y0);
		out.z0 = std::max(z0, other.z0);
		out.x1 = Math::min(x1, other.x1);
		out.y1 = Math::min(y1, other.y1);
		out.z1 = Math::min(z1, other.z1);

		return true;
	}

	void move(int dx, int dy, int dz) {
		x0 += dx;
		y0 += dy;
		z0 += dz;
		x1 += dx;
		y1 += dy;
		z1 += dz;
	}

	bool isInside(int x, int y, int z) const {
		return x >= x0 && x <= x1 && z >= z0 && z <= z1 && y >= y0 && y <= y1;
	}

	bool isInside(const BlockPos& pos) const {
		return pos.x >= x0 && pos.x <= x1 && pos.z >= z0 && pos.z <= z1 && pos.y >= y0 && pos.y <= y1;
	}

	int getXSpan() const {
		return x1 - x0 + 1;
	}

	int getYSpan() const {
		return y1 - y0 + 1;
	}

	int getZSpan() const {
		return z1 - z0 + 1;
	}

	int getXCenter() const {
		return x0 + (x1 - x0 + 1) / 2;
	}

	int getYCenter() const {
		return y0 + (y1 - y0 + 1) / 2;
	}

	int getZCenter() const {
		return z0 + (z1 - z0 + 1) / 2;
	}

	Unique<IntArrayTag> createTag() const;

};
