/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/
#include "Dungeons.h"

#include "world/level/dimension/HellDimension.h"
#include "world/level/Level.h"
#include "util/Math.h"
#include "world/level/levelgen/v1/HellRandomLevelSource.h"
#include "world/level/biome/Biome.h"

HellDimension::HellDimension(Level& level) :
	Dimension(level, DimensionId::Nether, HellGen::GEN_DEPTH){

	mSeaLevel = 32;
// 	mBiomeSource = ThreadLocal<BiomeSource>([&level] {
// 		return make_unique<FixedBiomeSource>(level.getSeed(), Biome::hell->mId);
// 	});
}

HellDimension::~HellDimension() {
}

void HellDimension::init() {
	Dimension::init();
	setUltraWarm(true);
	setCeiling(true);
// 	mSkyLightLayer.setSurrounding(Brightness::MIN);
}

void HellDimension::updateLightRamp() {
	float ambientLight = 0.10f;

	for (int i = 0; i <= Brightness::MAX; i++) {
		float v = (1 - i / (float)(Brightness::MAX));

		setBrightnessRamp((unsigned)i, ((1 - v) / (v * 3 + 1)) * (1 - ambientLight) + ambientLight);
	}
}

bool HellDimension::isNaturalDimension() const {
	return false;
}

bool HellDimension::isValidSpawn(int x, int z) const {
	return false;
}

bool HellDimension::showSky() const {
	return false;
}

float HellDimension::getTimeOfDay(int time, float a) const {
	return 0.5f;
}

Color HellDimension::getFogColor(float br) const {
	return Color(0.2f, 0.03f, 0.03f);
}

float HellDimension::getFogDistanceScale() const {
	return 0.4f;
}

bool HellDimension::isFoggyAt(int x, int z) const {
	return true;
}

bool HellDimension::mayRespawn() const {
	return false;
}

std::string HellDimension::getName() const {
	return "Nether";
}

Vec3 HellDimension::translatePosAcrossDimension(const Vec3& originalPos, DimensionId fromId) const {
	const float scale = 8.0f;
	Vec3 newPos(originalPos);

	if (fromId == DimensionId::Overworld) {
		newPos.x /= scale;
		newPos.z /= scale;
	}

	newPos.x = (float)Math::clamp((int)newPos.x, -Level::MAX_LEVEL_SIZE + 128, Level::MAX_LEVEL_SIZE - 128);
	newPos.z = (float)Math::clamp((int)newPos.z, -Level::MAX_LEVEL_SIZE + 128, Level::MAX_LEVEL_SIZE - 128);

	return newPos;
}
