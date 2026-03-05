/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/

#pragma once

#include "CommonTypes.h"
#include "Core/Math/Color.h"

class BlockPos;
class BlockSource;
class PerlinSimplexNoise;
class Random;
class TextureData;

class FoliageColor {
public:

	static int getGrassColor(float temp, float rain);
	static int getFoliageColor(float temp, float rain);
	static int getMapGrassColor(float temp, float rain);
	static int getMapFoliageColor(float temp, float rain);
	static int getEvergreenColor(float temp, float rain);
	static int getBirchColor(float temp, float rain);
	static int getMapEvergreenColor(float temp, float rain);
	static int getMapBirchColor(float temp, float rain);

	static int getDefaultColor();

	static int getGrassColor(BlockSource& source, const BlockPos& pos);
	static int getMapGrassColor(BlockSource& source, const BlockPos& pos);

	static void setFoliageColorPalette(const TextureData& texture);
	static void setFoliageEvergreenColorPalette(const TextureData& texture);
	static void setFoliageBirchColorPalette(const TextureData& texture);
	static void setGrassColorPalette(const TextureData& data);

	static void buildGrassColor(const BlockPos& min, const BlockPos& max, BlockSource& source, Random& random);

private:

	static void _setPalette(const TextureData& texture, std::vector<int32_t>& palette);
	static int _index(float rain, float temp);
	static int _getX(float rain, float temp);
	static int _getY(float rain, float temp);

	static int _toRGB(int abgr);

	static std::vector<int32_t> foliageColors;
	static std::vector<int32_t> foliageEvergreenColors;
	static std::vector<int32_t> foliageBirchColors;
	static std::vector<int32_t> grassColors;
	
	static const int GAUSSIAN_KERNAL_TOTAL_COUNT = 24 * 4;
	static const int PALETTE_SIZE = 256;

	static const Color GRASS_TOP_LEFT;
	static const Color GRASS_TOP_RIGHT;
	static const Color GRASS_BOTTOM_LEFT;
	static const Color GRASS_BOTTOM_RIGHT;

	static const Color FOLIAGE_TOP_LEFT;
	static const Color FOLIAGE_TOP_RIGHT;
	static const Color FOLIAGE_BOTTOM_LEFT;
	static const Color FOLIAGE_BOTTOM_RIGHT;

	static const Unique<PerlinSimplexNoise> GRASS_NOISE;
};
