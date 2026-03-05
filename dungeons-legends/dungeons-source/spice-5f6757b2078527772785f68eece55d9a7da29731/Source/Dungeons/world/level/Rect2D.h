#pragma once

#include "UnrealMathUtility.h"

struct Rect2D {
	int x;
	int y;
	int width;
	int height;
	int offsetX;
	int offsetY;

	Rect2D() :
		x(0)
		, y(0)
		, width(0)
		, height(0)
		, offsetX(0)
		, offsetY(0) {
	}

	Rect2D(int x_, int y_, int width_, int height_, int offsetX_ = 0, int offsetY_ = 0)
		: x(x_)
		, y(y_)
		, width(width_)
		, height(height_)
		, offsetX(offsetX_)
		, offsetY(offsetY_) {
	}

	static Rect2D fromOriginAndSize(FVector2D origin, FVector2D size) {
		return Rect2D(FMath::RoundToInt(origin.X), FMath::RoundToInt(origin.Y), FMath::RoundToInt(size.X), FMath::RoundToInt(size.Y));
	}

	static Rect2D fromTopLeftLowerRight(const FVector2D& topLeft, const FVector2D& bottomRight) {
		float width = bottomRight.X - topLeft.X;
		float height = bottomRight.Y - topLeft.Y;

		return Rect2D(FMath::RoundToInt(topLeft.X), FMath::RoundToInt(topLeft.Y), FMath::RoundToInt(width), FMath::RoundToInt(height));
	}

	int area() {
		return width * height;
	}

	Rect2D getOverlapping(const Rect2D& other) const {

		auto maxX = std::min(x + width, other.x + other.width);
		auto maxY = std::min(y + height, other.y + other.height);

		return{
			std::max(x, other.x),
			std::max(y, other.y),
			std::max(maxX - x, 0),
			std::max(maxY - y, 0)
		};
	}

	bool intersectsWith(const Rect2D& other, bool adjacentIntersects = true) {

		bool adjacentDoesntIntesect = !adjacentIntersects;

		int aMinX = x + offsetX;
		int aMaxX = aMinX + width;
		int aMinY = y + offsetY;
		int aMaxY = aMinY + height;

		int bMinX = other.x + other.offsetX;
		int bMaxX = bMinX + other.width;
		int bMinY = other.y + other.offsetY;
		int bMaxY = bMinY + other.height;

		// use separating axis theorem to determine overlap
		if (aMinX > bMaxX || (adjacentDoesntIntesect && aMinX == bMaxX)) {
			return false;
		}

		if (aMinY > bMaxY || (adjacentDoesntIntesect && aMinY == bMaxY)) {
			return false;
		}

		if (aMaxX < bMinX || (adjacentDoesntIntesect && aMaxX == bMinX)) {
			return false;
		}

		if (aMaxY < bMinY || (adjacentDoesntIntesect && aMaxY == bMinY)) {
			return false;
		}

		return true;
	}
};
