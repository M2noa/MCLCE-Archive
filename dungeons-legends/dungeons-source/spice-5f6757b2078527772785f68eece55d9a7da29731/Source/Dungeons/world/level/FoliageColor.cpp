/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/

#include "Dungeons.h"

#include "world/level/FoliageColor.h"
//#include "Core/Debug/DebugUtils.h"
#include "world/level/LevelConstants.h"
#include "util/Random.h"
#include "world/level/biome/Biome.h"
#include "world/level/BlockPos.h"
#include "world/level/BlockSource.h"
#include "world/level/LevelConstants.h"
#include "world/level/levelgen/synth/PerlinSimplexNoise.h"
#include "world/phys/Vec3.h"
#include "util/Math.h"
#include "client/renderer/texture/TextureData.h"
//#include "Renderer/HAL/Enums/TextureFormat.h"

const Color FoliageColor::GRASS_TOP_LEFT = Color::fromARGB(0x47d033);
const Color FoliageColor::GRASS_TOP_RIGHT = Color::fromARGB(0x6cb493);
const Color FoliageColor::GRASS_BOTTOM_LEFT = Color::fromARGB(0xbfb655);
const Color FoliageColor::GRASS_BOTTOM_RIGHT = Color::fromARGB(0x80b497);

const Color FoliageColor::FOLIAGE_TOP_LEFT = Color::fromARGB(0x1ABF00);
const Color FoliageColor::FOLIAGE_TOP_RIGHT = Color::fromARGB(0x4BA875);
const Color FoliageColor::FOLIAGE_BOTTOM_LEFT = Color::fromARGB(0xAEA42A);
const Color FoliageColor::FOLIAGE_BOTTOM_RIGHT = Color::fromARGB(0x60A17B);

std::vector<int32_t> FoliageColor::foliageColors;
std::vector<int32_t> FoliageColor::foliageBirchColors;
std::vector<int32_t> FoliageColor::foliageEvergreenColors;
std::vector<int32_t> FoliageColor::grassColors;

int FoliageColor::_index(float rain, float temp) {
	rain *= temp;
	const int x = Math::clamp((int)((1 - temp) * 255), 0, 255);
	const int y = Math::clamp((int)((1 - rain) * 255), 0, 255);

	return (y << 8) | x;
}

int FoliageColor::_getX(float rain, float temp) {
	return Math::clamp((int) ((1 - temp) * 255), 0, 255);
}

int FoliageColor::_getY(float rain, float temp) {
	rain *= temp;
	return Math::clamp((int)((1 - rain) * 255), 0, 255);
}

int FoliageColor::getMapGrassColor(float temp, float rain) {
	int x = _getX(rain, temp);
	int y = _getY(rain, temp);
	auto l1 = Color::lerp(GRASS_TOP_LEFT, GRASS_TOP_RIGHT, x / 256.0f);
	auto l2 = Color::lerp(GRASS_BOTTOM_LEFT, GRASS_BOTTOM_RIGHT, x / 256.0f);
	return Color::lerp(l1, l2, y / 256.0f).toARGB() & 0xffffff;
}

int FoliageColor::getMapFoliageColor(float temp, float rain) {
	int x = _getX(rain, temp);
	int y = _getY(rain, temp);
	auto l1 = Color::lerp(FOLIAGE_TOP_LEFT, FOLIAGE_TOP_RIGHT, x / 256.0f);
	auto l2 = Color::lerp(FOLIAGE_BOTTOM_LEFT, FOLIAGE_BOTTOM_RIGHT, x / 256.0f);
	return Color::lerp(l1, l2, y / 256.0f).toARGB() & 0xffffff;
}

int FoliageColor::getGrassColor(float temp, float rain) {
	DEBUG_ASSERT(grassColors.size() == 256 * 256, "Grass Color palette not initialized");

	auto idx = _index(rain, temp);
	return grassColors[idx];
}

int FoliageColor::getFoliageColor(float temp, float rain) {
	DEBUG_ASSERT(foliageColors.size() == 256 * 256, "Foliage Color palette not initialized");

	auto idx = _index(rain, temp);
	return foliageColors[idx];
}

int FoliageColor::getEvergreenColor(float temp, float rain) {
	DEBUG_ASSERT(foliageEvergreenColors.size() == 256 * 256, "Foliage Color palette not initialized");

	auto idx = _index(rain, temp);
	return foliageEvergreenColors[idx];
}

int FoliageColor::getMapEvergreenColor(float temp, float rain) {
	UNUSED_PARAMETER(temp, rain);
	return 0x619961;
}

int FoliageColor::getBirchColor(float temp, float rain) {
	DEBUG_ASSERT(foliageBirchColors.size() == 256 * 256, "Foliage Color palette not initialized");

	auto idx = _index(rain, temp);
	return foliageBirchColors[idx];
}

int FoliageColor::getMapBirchColor(float temp, float rain) {
	UNUSED_PARAMETER(temp, rain);
	return 0x80a755;
}

int FoliageColor::getDefaultColor() {
	return 0x48b518;
}

int FoliageColor::_toRGB(int abgr) {
	return (abgr & 0xff) << 16 | (abgr & 0xff00) | (abgr & 0xff0000) >> 16;
}

int FoliageColor::getGrassColor(BlockSource& source, const BlockPos& pos) {
	int totalRed = 0;
	int totalGreen = 0;
	int totalBlue = 0;

	BlockPos offset;

	for (offset.z = -2; offset.z <= 2; offset.z++) {
		for (offset.x = -2; offset.x <= 2; offset.x++) {
			int grassColor = source.getBiome(pos + offset * 4).getGrassColor(pos + offset);
			char multiply = 4;
			totalRed += ((grassColor & 0xff0000) >> 16) * multiply;
			totalGreen += ((grassColor & 0xff00) >> 8) * multiply;
			totalBlue += (grassColor & 0xff) * multiply;
		}
	}

	totalRed = Math::clamp(totalRed / GAUSSIAN_KERNAL_TOTAL_COUNT, 0, 255);
	totalGreen = Math::clamp(totalGreen / GAUSSIAN_KERNAL_TOTAL_COUNT, 0, 255);
	totalBlue = Math::clamp(totalBlue / GAUSSIAN_KERNAL_TOTAL_COUNT, 0, 255);

	return (totalRed << 16) | (totalGreen << 8) | totalBlue;
}

int FoliageColor::getMapGrassColor(BlockSource& source, const BlockPos& pos) {
	int totalRed = 0;
	int totalGreen = 0;
	int totalBlue = 0;

	BlockPos offset;

	for (offset.z = -2; offset.z <= 2; offset.z++) {
		for (offset.x = -2; offset.x <= 2; offset.x++) {
			int grassColor = source.getBiome(pos + offset * 4).getMapGrassColor(pos + offset);
			char multiply = 4;
			totalRed += ((grassColor & 0xff0000) >> 16) * multiply;
			totalGreen += ((grassColor & 0xff00) >> 8) * multiply;
			totalBlue += (grassColor & 0xff) * multiply;
		}
	}

	totalRed = Math::clamp(totalRed / GAUSSIAN_KERNAL_TOTAL_COUNT, 0, 255);
	totalGreen = Math::clamp(totalGreen / GAUSSIAN_KERNAL_TOTAL_COUNT, 0, 255);
	totalBlue = Math::clamp(totalBlue / GAUSSIAN_KERNAL_TOTAL_COUNT, 0, 255);

	return (totalRed << 16) | (totalGreen << 8) | totalBlue;
}

const Unique<PerlinSimplexNoise> FoliageColor::GRASS_NOISE = make_unique<PerlinSimplexNoise>( 89328, 5 );

static void extractColorData(const TextureData& texture, std::vector<int32_t>& palette) {
	const uint8_t* textureData = texture.getData();

	int pixelCount = texture.getWidth() * texture.getHeight();

	switch (texture.getTextureFormat()) {
// 	case mce::TextureFormat::R8G8B8_UNORM:
// 		palette.resize(pixelCount);
// 		for (int pixelIndex : range(pixelCount)) {
// 			int32_t convertedValue = 0xFF000000 | (textureData[0] << 16) | (textureData[1] << 8) | textureData[2];
// 			palette[pixelIndex] = convertedValue;
// 			textureData += 3;
// 		}
// 		break;

	case EPixelFormat::PF_B8G8R8A8:
		palette.resize(pixelCount);
		for (int pixelIndex : range(pixelCount)) {
			int32_t convertedValue = 0xFF000000 | (textureData[2] << 16) | (textureData[1] << 8) | textureData[0];
			palette[pixelIndex] = convertedValue;
			textureData += 4;
		}
		break;
		
// 	case mce::TextureFormat::R8G8B8A8_UNORM:
// 		palette.resize(pixelCount);
// 		for (int pixelIndex : range(pixelCount)) {
// 			int32_t convertedValue = 0xFF000000 | (textureData[0] << 16) | (textureData[1] << 8) | textureData[2];
// 			palette[pixelIndex] = convertedValue;
// 			textureData += 4;
// 		}
// 		break;
	default:
		break;
	}
}

void FoliageColor::_setPalette(const TextureData& texture, std::vector<int32_t>& palette) {
	std::vector<int32_t> temp;
	extractColorData(texture, temp);

	if (temp.size() >= PALETTE_SIZE * PALETTE_SIZE) {
		temp.resize(PALETTE_SIZE*PALETTE_SIZE);
		palette.resize(PALETTE_SIZE *PALETTE_SIZE);
		std::copy(temp.begin(), temp.end(), palette.begin());
	}
}

void FoliageColor::setFoliageColorPalette(const TextureData& texture) {
	_setPalette(texture, foliageColors);
}

void FoliageColor::setFoliageBirchColorPalette(const TextureData& texture) {
	_setPalette(texture, foliageBirchColors);
}

void FoliageColor::setFoliageEvergreenColorPalette(const TextureData& texture) {
	_setPalette(texture, foliageEvergreenColors);
}

void FoliageColor::setGrassColorPalette(const TextureData& texture) {
	_setPalette(texture, grassColors);
}

void FoliageColor::buildGrassColor(const BlockPos& min, const BlockPos& max, BlockSource& source, Random& random) {

	// Set grass color for block
	for (BlockPos p = min; p.x < max.x; ++p.x) {
		for (p.z = min.z; p.z < max.z; ++p.z) {
			int grassColor = getGrassColor(source, p);

			auto b = (unsigned char*)&grassColor;

			//apply some noise over the grass color
			int h = (int)floor(GRASS_NOISE->getValue(p));
			const int f = (int)((h - 30) * 0.5f) + random.nextInt(8) - 4;
			b[0] = static_cast<unsigned char>(Math::clamp((int)b[0] + f, 0, 255));
			b[1] = static_cast<unsigned char>(Math::clamp((int)b[1] + f, 0, 255));
			b[2] = static_cast<unsigned char>(Math::clamp((int)b[2] + f, 0, 255));

			source.setGrassColor(grassColor, p);
		}
	}
}
