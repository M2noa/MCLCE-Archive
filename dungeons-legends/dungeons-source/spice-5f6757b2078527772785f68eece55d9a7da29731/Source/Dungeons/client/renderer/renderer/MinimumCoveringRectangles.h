/********************************************************
*   (c) Mojang. All rights reserved                       *
*   (c) Microsoft. All rights reserved.                   *
*********************************************************/

#pragma once

#include "world/level/Rect2D.h"
#include "util/range.h"

class MinimumCoveringRectangles {
public:
	//NOTE: vector<byte> is used in place of vector<bool> because vector<bool> is a weird special container which is significatively slower
	static std::vector<Rect2D> getRectangles(std::vector<byte>& textureData, uint32_t width, uint32_t height) {
		std::vector<Rect2D> rects;
		Rect2D smallestRect(0, 0, 0, 0);

		const uint32_t MAX_QUAD_SIZE = 16;
		auto subquadsY = (height - 1) / MAX_QUAD_SIZE + 1;
		auto subquadsX = (width - 1) / MAX_QUAD_SIZE + 1;

		//split the thing into 16x16 subquads to linearize the cost at the expense of quality of the covering
		for(auto i : range(subquadsY)) {
			for(auto j : range(subquadsX)) {

				auto x0 = j * MAX_QUAD_SIZE;
				auto x1 = std::min(width, (j + 1) * MAX_QUAD_SIZE);
				auto y0 = i * MAX_QUAD_SIZE;
				auto y1 = std::min(height, (i + 1) * MAX_QUAD_SIZE);
				
				while (findSmallestRect(smallestRect, textureData, x0, y0, x1, y1, height)) {
					rects.push_back(smallestRect);

					for (auto x : range(smallestRect.x, smallestRect.x + smallestRect.width)) {
						for (auto y : range(smallestRect.y, smallestRect.y + smallestRect.height)) {
							textureData[x * height + y] = 0;
						}
					}
				}
			}
		}

		return rects;
	}

	//NOTE: vector<byte> is used in place of vector<bool> because vector<bool> is a weird special container which is significatively slower
	static bool findSmallestRect(Rect2D& smallestRect, const std::vector<byte>& textureData, uint32_t x0, uint32_t y0, uint32_t x1, uint32_t y1, uint32_t texHeight) {
		uint32_t smallestSize = 0;

		for(auto x : range(x0, x1)) {
			for(auto y : range(y0, y1)) {
				largestRect(smallestRect, smallestSize, textureData, x, y, x1, y1, texHeight);
			}
		}
		return smallestSize != 0;
	}

private:

	static uint32_t area(uint32_t x, uint32_t y, uint32_t x2, uint32_t y2) {
		return (x2 - x) * (y2 - y);
	}

	//NOTE: vector<byte> is used in place of vector<bool> because vector<bool> is a weird special container which is significatively slower
	static void largestRect(Rect2D& smallestRect, uint32_t& smallestSize, const std::vector<byte>& textureData, uint32_t x0, uint32_t y0, uint32_t x1, uint32_t y1, uint32_t texHeight) {
		for(auto xItr : reverse_range(x0, x1 + 1)) {
			for(auto yItr : reverse_range(y0, y1 + 1)) {
				auto currentArea = area(x0, y0, xItr, yItr);
				if(currentArea < smallestSize) {
					break;
				}
				else if(!allCovered(textureData, x0, y0, xItr, yItr, texHeight)) {
					continue;
				}
				else {
					smallestRect = Rect2D(x0, y0, xItr - x0, yItr - y0);
					smallestSize = currentArea;
				}
			}
		}
	}

	//NOTE: vector<byte> is used in place of vector<bool> because vector<bool> is a weird special container which is significatively slower
	static bool allCovered(const std::vector<byte>& textureData, uint32_t x0, uint32_t y0, uint32_t x1, uint32_t y1, uint32_t texHeight) {
		for(auto xItr : range(x0, x1)) {
			for(auto yItr : range(y0, y1)) {
				if(textureData[xItr * texHeight + yItr] == false) {
					return false;
				}
			}
		}
		return true;
	}

};
