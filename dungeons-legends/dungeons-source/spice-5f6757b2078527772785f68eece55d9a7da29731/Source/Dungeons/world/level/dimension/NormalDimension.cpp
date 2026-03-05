#include "Dungeons.h"

#include "world/level/dimension/NormalDimension.h"
#include "world/level/LevelConstants.h"
#include "world/level/Level.h"
#include "world/level/biome/BiomeSource.h"
// #include "platform/threading/ThreadLocal.h"

NormalDimension::NormalDimension(Level& level) :
	Dimension(level, DimensionId::Overworld, level.getLevelData().getGenerator() == GeneratorType::Legacy ? 128 : 256) {
	//"old" worlds are still 128 blocks high

	setHasWeather(true);
// 	mSkyLightLayer.setSurrounding(Brightness::MAX);

	mSeaLevel = 63;
// 	mBiomeSource = ThreadLocal<BiomeSource>([&level] {
// 		return make_unique<BiomeSource>(level.getSeed(), level.getLevelData().getGenerator());
// 	});
}

NormalDimension::~NormalDimension() {

}

void NormalDimension::init() {
	Dimension::init();
}

std::string NormalDimension::getName() const {
	return "Overworld";
}

Vec3 NormalDimension::translatePosAcrossDimension(const Vec3& originalPos, DimensionId fromId) const {
	const float scale = 8.0f;
	Vec3 newPos(originalPos);

	if (fromId == DimensionId::Nether) {
		newPos.x *= scale;
		newPos.z *= scale;
	}
// 	else if (fromId == DimensionId::TheEnd) {
// 		newPos = mLevel.getSharedSpawnPos();
// 	}

	newPos.x = (float)Math::clamp((int)newPos.x, -Level::MAX_LEVEL_SIZE + 128, Level::MAX_LEVEL_SIZE - 128);
	newPos.z = (float)Math::clamp((int)newPos.z, -Level::MAX_LEVEL_SIZE + 128, Level::MAX_LEVEL_SIZE - 128);

	return newPos;
}
