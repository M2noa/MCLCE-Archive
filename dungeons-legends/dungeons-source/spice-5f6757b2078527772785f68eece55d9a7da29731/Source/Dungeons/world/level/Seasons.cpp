/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/

#include "Dungeons.h"

#include "world/level/Seasons.h"

#include "world/level/biome/Biome.h"
#include "world/level/BlockSource.h"
#include "world/level/chunk/LevelChunk.h"
#include "world/level/dimension/Dimension.h"
#include "world/level/Level.h"
#include "world/level/Weather.h"

Seasons::Seasons(Dimension& d)
	: mDimension(d)
	, mSnowNoise(89328, 5)
{
}

Seasons::~Seasons() {
}

static const float SEASONS_DRY_MELT_RATE_PER_SECOND = 0.001f;	// If temp - freezing == 1.0f, this is how much snow melts per
														// second
static const float SEASONS_RAIN_MELT_RATE_PER_SECOND = 0.002f;
static const float SEASONS_SNOW_ACCUM_RATE_PER_SECOND = 0.001f;// 01f;

static const float SEASONS_FOLIAGE_DRY_MELT_RATE_PER_SECOND = 0.04f;	// If temp - freezing == 1.0f, this is how much snow
																// melts per second
static const float SEASONS_FOLIAGE_RAIN_MELT_RATE_PER_SECOND = 0.08f;
static const float SEASONS_FOLIAGE_SNOW_ACCUM_RATE_PER_SECOND = 0.04f;	// 01f;

void Seasons::tick() {
	Weather& weather = mDimension.getWeather();

	for (int i = 0; i < Biome::BIOMES_COUNT; ++i) {
		Biome* b = Biome::getBiome(i);
		if (b != nullptr) {
			if (weather.isRaining()) {
				float deltaAmount = 0.0f;
				float foliageDeltaAmount = 0.0f;
				if (b->mTemperature > Biome::RAIN_TEMP_THRESHOLD) {
					// it's raining
					deltaAmount = (Biome::RAIN_TEMP_THRESHOLD - b->mTemperature) * SEASONS_RAIN_MELT_RATE_PER_SECOND * weather.getRainLevel(0.0f) * b->mDownfall;
					foliageDeltaAmount = (Biome::RAIN_TEMP_THRESHOLD - b->mTemperature) * SEASONS_FOLIAGE_RAIN_MELT_RATE_PER_SECOND * weather.getRainLevel(0.0f) * b->mDownfall;
				}
				else {
					deltaAmount = SEASONS_SNOW_ACCUM_RATE_PER_SECOND * weather.getRainLevel(0.0f) * b->mDownfall;
					foliageDeltaAmount = SEASONS_FOLIAGE_SNOW_ACCUM_RATE_PER_SECOND * weather.getRainLevel(0.0f) * b->mDownfall;
				}
				b->mSnowAccumulation = Math::clamp(b->mSnowAccumulation + (deltaAmount / SharedConstants::TicksPerSecond), b->mMinSnowLevel, b->mMaxSnowLevel);
				b->mFoliageSnow = Math::clamp(b->mFoliageSnow + (foliageDeltaAmount / SharedConstants::TicksPerSecond), 0.0f, 1.0f);
			}
			else {
				float meltAmount = SEASONS_DRY_MELT_RATE_PER_SECOND;
				float foliageMeltAmount = SEASONS_FOLIAGE_DRY_MELT_RATE_PER_SECOND;
				b->mSnowAccumulation = Math::clamp(b->mSnowAccumulation - (meltAmount / SharedConstants::TicksPerSecond), b->mMinSnowLevel, b->mMaxSnowLevel);
				b->mFoliageSnow = Math::clamp(b->mFoliageSnow - (foliageMeltAmount / SharedConstants::TicksPerSecond), 0.0f, 1.0f);
			}
		}
	}
}

void Seasons::postProcess(LevelChunk& lc, BlockSource& region, const ChunkPos& cp) {

	const auto noiseMin = -26;
	const auto noiseMax = 26;

	//HACK this really sucks
	std::vector<LevelChunk::DirtyTicksCounter> originalDirtyTicks;
	for (auto&& sc : lc.getSubChunks()) {
		originalDirtyTicks.push_back(sc->mDirtyTicksCounter);
	}

	Weather& weather = region.getDimension().getWeather();

	BlockPos tpos, tmin(cp), tmax(tmin + BlockPos(CHUNK_WIDTH - 1, region.getMaxHeight() - 1, CHUNK_DEPTH - 1));

	for (tpos.z = tmin.z; tpos.z <= tmax.z; ++tpos.z) {
		for (tpos.x = tmin.x; tpos.x <= tmax.x; ++tpos.x) {

			ChunkBlockPos ctp(tpos);
			tpos = lc.getTopRainBlockPos(ctp);
			ctp.y = 0;	// Top Rain Block Pos can be 128, if we use that, then getBiome will fail

			Biome& biome = lc.getBiome(ctp);
			if (biome.canHaveSnowfall()) {
				// Check for top snow already existing, if so theres no need to call canPlaceTopSnow().
				if (weather.canPlaceTopSnow(region, tpos, true, false, nullptr)) {
					auto val = mSnowNoise.getValue((Vec3)(tpos));
					auto f = (val - noiseMin) / (noiseMax - noiseMin) - 0.17f;

					int snowLevel = (int)(std::ceil(f * biome.getMinSnowLayers()));

					//check if any neighbor block is higher and increase snow if it is
					for (auto&& facing : Facing::Plane::HORIZONTAL) {
						auto h = region.getTopRainBlockPos(tpos.relative(facing)).y;
						if (h > tpos.y) {
							snowLevel++;
						}
						else if (h < tpos.y && snowLevel > 1) {
							snowLevel--;
						}
					}

					snowLevel = Math::clamp(0, snowLevel, biome.getMaxSnowLayers());

					weather.rebuildTopSnowToDepth(region, tpos, snowLevel);
				}
			}
		}
	}

	//HACK kill this
	for (auto i : range(Math::min(originalDirtyTicks.size(), lc.getSubChunks().size()))) {
		lc.getSubChunks()[i]->mDirtyTicksCounter = originalDirtyTicks[i];
	}
}
