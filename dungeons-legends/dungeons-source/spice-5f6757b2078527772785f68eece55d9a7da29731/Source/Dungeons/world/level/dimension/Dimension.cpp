/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/
#include "Dungeons.h"

#include "world/level/levelgen/synth/PerlinNoise.h"
#include "world/level/dimension/Dimension.h"
#include "world/level/dimension/NormalDimension.h"
#include "world/level/dimension/HellDimension.h"
// #include "world/level/dimension/end/TheEndDimension.h"

// #include "network/packet/BlockEventPacket.h"
// #include "network/PacketSender.h"
#include "world/level/Level.h"
#include "world/level/block/entity/BlockEntity.h"
#include "world/level/BlockSource.h"
#include "world/level/biome/BiomeSource.h"
#include "world/level/block/Block.h"
#include "world/level/chunk/MainChunkSource.h"
#include "world/level/chunk/NetworkChunkSource.h"
#include "world/level/chunk/WorldLimitChunkSource.h"
#include "world/level/levelgen/v1/RandomLevelSource.h"
#include "world/level/levelgen/v1/HellRandomLevelSource.h"
#include "world/level/levelgen/v1/TheEndRandomLevelSource.h"
#include "world/level/levelgen/flat/FlatLevelSource.h"
#include "world/level/storage/LevelStorage.h"
// #include "world/level/PortalForcer.h"
#include "world/entity/Entity.h"
// #include "world/entity/EntityFactory.h"
#include "world/entity/player/Player.h"
#include "world/level/Weather.h"
#include "world/level/Seasons.h"
#include "world/level/chunk/LevelChunk.h"
#include "world/level/GeneratorType.h"
#include "world/level/storage/SavedDataStorage.h"
// #include "world/redstone/circuit/CircuitSystem.h"

// #include "network/packet/MoveEntityPacket.h"
// #include "network/packet/SetEntityMotionPacket.h"
// #include "network/packet/UpdateBlockPacket.h"
// #include "network/packet/MoveEntityPacketData.h"
#include "Core/Math/Color.h"
#include <vector>
#include "nbt/ListTag.h"


const std::string dimensionFileIds[] = {
	"Overworld",
	"Nether",
	"TheEnd",
	"Undefined"
};

const float Dimension::MOON_BRIGHTNESS_PER_PHASE[] = {
	1.f, 0.75f, 0.5f, 0.25f, 0, 0.25f, 0.5f, 0.75f
};

Dimension::Dimension(Level& level, DimensionId dimId, Height height) : SavedData(dimensionFileIds[dimId])
	, mId(dimId)
	, mSkyDarken(Brightness::MIN)
	, mLevel(level)
// 	, mCircuitSystem(make_unique<CircuitSystem>())
	, mHeight(height)
// 	, mSkyLightLayer(0, Brightness::MAX) 
{
// 	mLevel.addListener(*this);

// 	if (dimId != DimensionId::TheEnd) {
// 		if (!mLevel.mSavedDataStorage->load(this, dimensionFileIds[dimId])) {
// 			mLevel.mSavedDataStorage->set(dimensionFileIds[dimId], this);
// 		}
// 	}

	mWeather = make_unique<Weather>(*this);
	mSeasons = make_unique<Seasons>(*this);

	//memset(mMobsPerChunkSurface, 0, sizeof(mMobsPerChunkSurface));
	//memset(mMobsPerChunkUnderground, 0, sizeof(mMobsPerChunkUnderground));

	//mMobsPerChunkSurface[MobSpawnInfo::categoryID(EntityType::Animal)] = 10;
	//mMobsPerChunkSurface[MobSpawnInfo::categoryID(EntityType::WaterAnimal)] = 3;
	//mMobsPerChunkSurface[MobSpawnInfo::categoryID(EntityType::Monster)] = 20;
	//mMobsPerChunkSurface[MobSpawnInfo::categoryID(EntityType::Ambient)] = 0;

	//mMobsPerChunkUnderground[MobSpawnInfo::categoryID(EntityType::Animal)] = 0;
	//mMobsPerChunkUnderground[MobSpawnInfo::categoryID(EntityType::WaterAnimal)] = 0;
	//mMobsPerChunkUnderground[MobSpawnInfo::categoryID(EntityType::Monster)] = 30;
	//mMobsPerChunkUnderground[MobSpawnInfo::categoryID(EntityType::Ambient)] = 9;

	setDirty();
}

Dimension::~Dimension() {
// 	mLevel.removeListener(*this);
}

std::unique_ptr<Dimension> Dimension::createNew(DimensionId id, Level& level) {
	switch (id) {
	case DimensionId::Nether:
		return make_unique<HellDimension>(level);
	case DimensionId::Overworld:
		return make_unique<NormalDimension>(level);
// 	case DimensionId::TheEnd:
// 		return make_unique<TheEndDimension>(level);
	default:
		return nullptr;
	}
}

void Dimension::init() {

	if (!mLevel.mIsClientSide) {
		//this is a neat chinese boxes thing that concatenates levelsources a la java

		GeneratorType generatorType;
		switch (mId) {
			case DimensionId::Nether:
				generatorType = GeneratorType::Nether;
				break;
			case DimensionId::TheEnd:
				generatorType = GeneratorType::TheEnd;
				break;
			default:
				generatorType = mLevel.getLevelData().getGenerator();
				break;
		}

		const bool enableDisk = true;	//TODO do some file-based option here

		if (enableDisk) {
			mChunkSource = make_unique<MainChunkSource>(
				mLevel.getLevelStorage().createChunkStorage(
				_createGenerator(generatorType), mLevel.getLevelData().getStorageVersion()));
		}
		else {
			mChunkSource = make_unique<MainChunkSource>(_createGenerator(generatorType));
		}
	}
	else {
		//create a one-level network source
		mChunkSource = make_unique<NetworkChunkSource>(&mLevel, this);
	}

	//wrap the world in a new limit source if needed
	if (mLevel.getLevelData().getGenerator() == GeneratorType::Legacy) {
		mChunkSource = make_unique<WorldLimitChunkSource>(std::move(mChunkSource), mLevel.getLevelData().getWorldCenter());
	}

	mBlockSource = make_unique<BlockSource>(mLevel, *this, *mChunkSource);

	updateLightRamp();
}
 
bool Dimension::isRedstoneTick() {
	return (mCircuitSystemTickRate >= CIRCUIT_TICK_RATE);
}


void Dimension::addWither(const EntityUniqueID& id) {
	mWitherIDs.insert(id);
}

void Dimension::removeWither(const EntityUniqueID& id) {
	mWitherIDs.erase(id);
}

float Dimension::getPopCap(int catID, bool surface) const {
// 	auto map = surface ? mMobsPerChunkSurface : mMobsPerChunkUnderground;
// 	return map[catID];
	return 0.0f;
}

void Dimension::tickRedstone() {
	//tick the circuit system with a valid region
	if (mCircuitSystemTickRate >= CIRCUIT_TICK_RATE) {
// 		mCircuitSystem->evaluate();
		mCircuitSystemTickRate = 0;
	}
	mCircuitSystemTickRate++;
}

void Dimension::tick() {

	mWeather->tick();
	mSeasons->tick();

	auto newDark = getOldSkyDarken(1);

	if (newDark != mSkyDarken) {
		mSkyDarken = newDark;
	}


}

void Dimension::updateLightRamp() {
	float ambientLight = 0.00f;

	for (int i = 0; i <= Brightness::MAX; i++) {
		float v = (1 - i / (float)(Brightness::MAX));
		mBrightnessRamp[i] = Math::clamp(((1 - v) / (v * 3 + 1)) + ambientLight, 0.f, 1.f);
	}
}

bool Dimension::isNaturalDimension() const {
	return true;
}

bool Dimension::isValidSpawn(int x, int z) const {
	return false;// level->getTopBlock(x, z) == Block::grass->id;
}

Color Dimension::getFogColor(float br) const {
	Color fogColor = Color::fromARGB(0xffabd2ff);
	fogColor.r *= br * 0.94f + 0.06f;
	fogColor.g *= br * 0.94f + 0.06f;
	fogColor.b *= br * 0.91f + 0.09f;
	return fogColor;
}

float Dimension::getFogDistanceScale() const {
	return 1.0f;
}

bool Dimension::isFoggyAt(int x, int z) const {
	return false;
}

float Dimension::getCloudHeight() const {
	return (float)Level::CLOUD_HEIGHT;
}

BiomeSource& Dimension::getBiomes() {
// 	return mBiomeSource.getLocal();
	return mBiomeSource;
}

bool Dimension::mayRespawn() const {
	return true;
}

bool Dimension::hasGround() const {
	return true;
}

BlockPos Dimension::getSpawnPos() const {
	return BlockPos::ZERO;
}

int Dimension::getSpawnYPosition() const {
	return 0;
}

bool Dimension::hasBedrockFog() {
	return false;// (levelType != LevelType.flat && !hasCeiling);
}

float Dimension::getClearColorScale() {
	/*if (levelType == LevelType.flat) {
		return 1.0f;
	   }*/
	return 1.0f / 32.0f;
}

Seasons& Dimension::getSeasons() {
	return *mSeasons.get();
}

bool Dimension::showSky() const {
	return true;
}

bool Dimension::isDay() const {
	return mSkyDarken < 4;
}

float Dimension::getTimeOfDay(float a) const {
	//either use the real time, or the time stopped
	auto time = mLevel.isDayCycleActive() ? mLevel.getTime() : mLevel.getLevelData().getStopTime();
	return getTimeOfDay(time, a);
}

float Dimension::getSunAngle(float a) {
	float td = getTimeOfDay(a);
	return td * PI * 2;
}

Vec3 Dimension::getSunlightDirection(float a) {
	float angle = getSunAngle(a);
	return Vec3(0.25, cosf(angle), sinf(angle));
}

float _fakeSunHorizon(const Vec3& dir) {
	//TODO actually do a raycast?
	const float h = -0.1f;
	if (dir.y < h) {
		return 0.f;
	}
	else if (dir.y < 0.f) {
		return 1.f - dir.y / h;
	}
	else{
		return 1.f;
	}
}

float Dimension::getSunIntensity(float a, const Entity& camera, float minInfluenceAngle) {
	Weather& weather = getWeather();

	Vec3 look = camera.getViewVector(a);
	Vec3 dir = getSunlightDirection(a);

	float sunriseFactor = ((look * dir) + 1.f) * 0.5f * _fakeSunHorizon(dir);

	float intensity = (Math::clamp(sunriseFactor, minInfluenceAngle, 1.f) - minInfluenceAngle) / (1.f - minInfluenceAngle);

	return Math::lerp(intensity, 0.0f, weather.getFogLevel());
}

int Dimension::getMoonPhase() const {
	return ((int)(mLevel.getTime() / Level::TICKS_PER_DAY)) % 8;
}

float Dimension::getMoonBrightness() const {
	return MOON_BRIGHTNESS_PER_PHASE[getMoonPhase()];
}

float Dimension::getSkyDarken(float a) const {
	float td = getTimeOfDay(a);

	float brightness = 1 - (cosf(td * PI * 2.f) * 2.f + 0.2f);
	brightness = 1.f - Math::clamp(brightness, 0.f, 1.f);

	const Weather& weather = getWeather();
	brightness *= 1 - (weather.getFogLevel() * 5.0f / 16.0f);
	brightness *= 1 - (weather.getLightningLevel(a) * 5.0f / 16.0f);
	if (mWitherIDs.size() > 0) {
		brightness *= 1 - 5.0f / 16.0f;
	}
	

	return brightness * 0.8f + 0.2f;
}

Brightness Dimension::getSkyDarken() const {
	return mSkyDarken;
}

const float* Dimension::getBrightnessRamp() const {
	return mBrightnessRamp;
}

void Dimension::setBrightnessRamp(unsigned index, float brightness) {
	mBrightnessRamp[index] = brightness;
}

Brightness Dimension::getOldSkyDarken(float a) {
	float td = getTimeOfDay(a);
	float brightness = 1 - (Math::cos(td * PI * 2) * 2 + 0.5f);
	if (brightness < 0.f) {
		brightness = 0.f;
	}
	if (brightness > 1.f) {
		brightness = 1.f;
	}
	brightness = 1 - brightness;

	const Weather& weather = getWeather();
	brightness *= 1 - (weather.getFogLevel() * 6.1f / 16.0f);
	brightness *= 1 - (weather.getLightningLevel(a) * 5.0f / 16.0f);
	if (mWitherIDs.size() > 0) {
		brightness *= 1 - 5.0f / 16.0f;
	}

	brightness = 1 - brightness;

	return (Brightness)((int)(brightness * 11));
}

Color Dimension::getSunriseColor(float a) const {
	float td = getTimeOfDay(a);

	Color sunriseCol;
	float span = 0.4f;
	float tt = Math::cos(td * PI * 2.f) - 0.0f;
	float mid = -0.0f;
	if (tt >= mid - span && tt <= mid + span) {
		float aa = ((tt - mid) / span) * 0.5f + 0.5f;
		float mix = 1 - (((1 - Math::sin(aa * PI))) * 0.99f);
		mix = mix * mix;
		sunriseCol[0] = (aa * 0.3f + 0.7f);
		sunriseCol[1] = (aa * aa * 0.7f + 0.2f);
		sunriseCol[2] = (aa * aa * 0.0f + 0.2f);
		sunriseCol[3] = mix;
	}
	return sunriseCol;
}

Color Dimension::getSkyColor(const Entity& source, float a) {
	return getSkyColor(
		source.getRegion(),
		source.getInterpolatedPosition(a),
		a);
}

Color Dimension::getSkyColor(BlockSource& source, const BlockPos& pos, float a) {
	float td = getTimeOfDay(a);

	float brightness = Math::clamp(Math::cos(td * PI * 2) * 2 + 0.5f, 0.f, 1.f);

	Biome& biome = source.getBiome(pos);
	Color skyColor = biome.getSkyColor(biome.getTemperature()) * brightness;

	const Weather& weather = getWeather();
	float fogLevel = weather.getFogLevel();
	if (weather.isRaining() && fogLevel > 0) {
		fogLevel *= 4.0f;
		skyColor = Color::lerp(skyColor, Color(0.5f, 0.5f, 0.5f) * brightness, Math::clamp(fogLevel, 0.0f, 1.0f));
	}

	float thunderLevel = weather.getLightningLevel(a);
	if (thunderLevel > 0 || mWitherIDs.size() > 0) {
		float mid = (skyColor.r * 0.30f + skyColor.g * 0.59f + skyColor.b * 0.11f) * 0.2f;

		float ba = 1 - thunderLevel * 0.75f;
		if (mWitherIDs.size() > 0) {
			ba = .25f;
		}
		skyColor.r = skyColor.r * ba + mid * (1 - ba);
		skyColor.g = skyColor.g * ba + mid * (1 - ba);
		skyColor.b = skyColor.b * ba + mid * (1 - ba);
	}

	if (weather.getSkyFlashTime() > 0) {
		float f = (weather.getSkyFlashTime() - a);
		if (f > 1) {
			f = 1;
		}
		f *= 0.45f;
		skyColor.r = skyColor.r * (1 - f) + 0.8f * f;
		skyColor.g = skyColor.g * (1 - f) + 0.8f * f;
		skyColor.b = skyColor.b * (1 - f) + 1 * f;
	}

	return skyColor;
}

Color Dimension::getCloudColor(float a) {
	float td = getTimeOfDay(a);

	float brightness = Math::cos(td * PI * 2) * 2.0f + 0.5f;
	if (brightness < 0.0f) {
		brightness = 0.0f;
	}
	if (brightness > 1.0f) {
		brightness = 1.0f;
	}

	Color color = Color::WHITE;

	const Weather& weather = getWeather();
	float rainLevel = weather.getRainLevel(a);
	if (rainLevel > 0) {
		float mid = (color.r * 0.30f + color.g * 0.59f + color.b * 0.11f) * 0.6f;

		float ba = 1 - rainLevel * 0.95f;
		color.r = color.r * ba + mid * (1 - ba);
		color.g = color.g * ba + mid * (1 - ba);
		color.b = color.b * ba + mid * (1 - ba);
	}

	color.r *= brightness * 0.90f + 0.10f;
	color.g *= brightness * 0.90f + 0.10f;
	color.b *= brightness * 0.85f + 0.15f;

	float thunderLevel = weather.getLightningLevel(a);
	if (thunderLevel > 0 || mWitherIDs.size() > 0) {
		float mid = (color.r * 0.30f + color.g * 0.59f + color.b * 0.11f) * 0.2f;

		float ba = 1 - thunderLevel * 0.95f;
		if (mWitherIDs.size() > 0) {
			ba = .05f;
		}
		color.r = color.r * ba + mid * (1 - ba);
		color.g = color.g * ba + mid * (1 - ba);
		color.b = color.b * ba + mid * (1 - ba);
	}

	return color;
}

static const float STAR_BRIGHTNESS_SCALE = 3.0f;

float Dimension::getStarBrightness(float a) {
	float td = getTimeOfDay(a);

	float br = 1 - (Math::cos(td * PI * 2) * 2 + 0.75f);
	float rainLevel = getWeather().getRainLevel(a);
	br *= Math::clamp(1.0f - STAR_BRIGHTNESS_SCALE * rainLevel, 0.0f, 1.0f);
	if (br < 0.f) {
		br = 0;
	}
	if (br > 1.f) {
		br = 1;
	}

	return br * br * 0.5f;
}

DimensionId Dimension::getId() const {
	return mId;
}

Level& Dimension::getLevel() const {
	return mLevel;
}

const Level& Dimension::getLevelConst() const {
	return mLevel;
}

ChunkSource& Dimension::getChunkSource() const {
	return *(mChunkSource.get());
}

BlockSource& Dimension::getBlockSourceDEPRECATEDUSEPLAYERREGIONINSTEAD() const {
	return *(mBlockSource.get());
}

Weather& Dimension::getWeather() const {
	return *(mWeather.get());
}

// CircuitSystem& Dimension::getCircuitSystem() {
// 	return *mCircuitSystem.get();
// }

EntityMap& Dimension::getEntityIdMap() {
	return mEntityIdLookup;
}

const EntityMap& Dimension::getEntityIdMapConst() const {
	return mEntityIdLookup;
}

Entity* Dimension::fetchEntity(EntityUniqueID entityId, bool getRemoved) {

	// first look through entities
	auto eit = mEntityIdLookup.find(entityId);
	if (eit != mEntityIdLookup.end()) {
		return (eit->second->isRemoved() && !getRemoved) ? nullptr : eit->second;
	}

	Entity* entity = nullptr;

	// then look through players
	mLevel.forEachPlayer([&getRemoved, &entity, &entityId](Player& player) {
		if ((!player.isRemoved() || getRemoved) && player.getUniqueID() == entityId) {
			entity = &player;
			return false;
		}

		return true;
	});

	return entity;
}

Vec3 Dimension::translatePosAcrossDimension(const Vec3& originalPos, DimensionId fromId) const {
	return originalPos;
}

void Dimension::transferEntity(const Vec3& spawnPos, std::unique_ptr<CompoundTag> entityTag) {

	if (mLevel.mIsClientSide) {
		return;
	}

// 	if (mBlockSource->hasChunksAt(BlockPos(spawnPos), 50)) {
// 
// 		if (auto entity = EntityFactory::loadEntity(entityTag.get())) {
// 			_completeEntityTransfer(*mBlockSource, std::move(entity));
// 		}
// 		else {
// 			// Try patching up an old serialized Entity and adding it to the level
// 			EntityFactory::fixLegacyEntity(*mBlockSource, entityTag.get());
// 		}
// 	}
// 	else {
// 
// 		ChunkPos chunkPos(spawnPos);
// 		ChunkPosToEntityListMap::iterator limboEntityPair = mLimboEntities.find(chunkPos);
// 		if (limboEntityPair == mLimboEntities.end()) {
// 			mLimboEntities.emplace(chunkPos, EntityTagList());
// 			limboEntityPair = mLimboEntities.find(chunkPos);
// 		}
// 
// 		EntityTagList& tagList = limboEntityPair->second;
// 		tagList.push_back(std::move(entityTag));
// 
// 		setDirty();
// 	}
}

// void Dimension::onNewChunkFor(Player& p, LevelChunk& lc) {
// 
// 	if (mLevel.mIsClientSide || p.getDimensionId() != mId || !p.isRegionValid()) {
// 		return;
// 	}
// 
// 	BlockSource& region = p.getRegion();
// 
// 	ChunkPosToEntityListMap::iterator limboEntityPair = mLimboEntities.find(lc.getPosition());
// 	if (limboEntityPair != mLimboEntities.end()) {
// 
// 		EntityTagList& entityTagList = limboEntityPair->second;
// 
// 		for (auto& entityTag : entityTagList) {
// 
// 			if (auto entity = EntityFactory::loadEntity(entityTag.get())) {
// 				_completeEntityTransfer(region, std::move(entity));
// 			}
// 		}
// 
// 		mLimboEntities.erase(lc.getPosition());
// 
// 		setDirty();
// 	}
// }

void Dimension::load(const CompoundTag& tag) {

	if (mLevel.mIsClientSide) {
		return;
	}

	mLimboEntities.clear();

	const ListTag* perChunkTagList = tag.getList("LimboEntities");
	if (perChunkTagList == nullptr) {
		return;
	}

	for (int i = 0; i < perChunkTagList->size(); ++i) {
		Tag* perChunkTag = perChunkTagList->get(i);
		if (perChunkTag->getId() == Tag::Type::Compound) {
			CompoundTag* perChunkCompoundTag = static_cast<CompoundTag*>(perChunkTag);

			int chunkX = perChunkCompoundTag->getInt("ChunkX");
			int chunkZ = perChunkCompoundTag->getInt("ChunkZ");
			ChunkPos chunkPos(chunkX, chunkZ);

			mLimboEntities.emplace(chunkPos, EntityTagList());
			EntityTagList& clonedEntityTagList = mLimboEntities.find(chunkPos)->second;

			if (const ListTag* entityTagList = perChunkCompoundTag->getList("EntityTagList")) {
				for (int j = 0; j < entityTagList->size(); ++j) {
					CompoundTag* entityTag = (CompoundTag*)entityTagList->get(j);
					std::unique_ptr<CompoundTag> clonedEntityTag = entityTag->clone();
					clonedEntityTagList.push_back(std::move(clonedEntityTag));
				}
			}
		}
	}
}

void Dimension::save(CompoundTag& tag) {

	if (mLevel.mIsClientSide) {
		return;
	}

	std::unique_ptr<ListTag> perChunkTagList = make_unique<ListTag>();

	for (auto& limboEntityPair : mLimboEntities) {

		const ChunkPos& chunkPos = limboEntityPair.first;
		EntityTagList& entityTagList = limboEntityPair.second;

		// create a compound tag containing the chunk position and the
		// list of serialized entities within it.
		std::unique_ptr<CompoundTag> perChunkTag = make_unique<CompoundTag>();
		perChunkTag->putInt("ChunkX", chunkPos.x);
		perChunkTag->putInt("ChunkZ", chunkPos.z);
		std::unique_ptr<ListTag> clonedEntityTagList = make_unique<ListTag>();

		for (auto& entityTag : entityTagList) {
			std::unique_ptr<CompoundTag> clonedEntityTag = entityTag->clone();
			clonedEntityTagList->add(std::move(clonedEntityTag));
		}
		perChunkTag->put("EntityTagList", std::move(clonedEntityTagList));

		perChunkTagList->add(std::move(perChunkTag));
	}

	tag.put("LimboEntities", std::move(perChunkTagList));
}

float Dimension::getTimeOfDay(int time, float a) const {
	int dayStep = (int)(time % Level::TICKS_PER_DAY);
	float td = (dayStep + a) / Level::TICKS_PER_DAY - 0.25f;
	if (td < 0) {
		td += 1;
	}
	if (td > 1) {
		td -= 1;
	}
	float tdo = td;
	td = 1 - (float)((Math::cos(td * PI) + 1) / 2);
	td = tdo + (td - tdo) / 3.0f;
	return td;
}

std::unique_ptr<ChunkSource> Dimension::_createGenerator(GeneratorType type) {
	switch (type) {
	case GeneratorType::Legacy:
		return make_unique<RandomLevelSource>(&mLevel, this, mLevel.getSeed(), true);
	case GeneratorType::Overworld:
		return make_unique<RandomLevelSource>(&mLevel, this, mLevel.getSeed(), false);
	case GeneratorType::Flat:
		// Hack TODO FIX Johan!
		return make_unique<FlatLevelSource>(&mLevel, this, /*minecraft.options.flatWorldLayers*/ FlatLevelSource::DEFAULT_LAYERS);
	case GeneratorType::Nether:
		return make_unique<HellRandomLevelSource>(&mLevel, this, mLevel.getSeed());
	case GeneratorType::TheEnd:
		return make_unique<TheEndRandomLevelSource>(&mLevel, this, mLevel.getSeed());
	default:
		DEBUG_FAIL("not implemented!");
		return nullptr;
	}
}

void Dimension::onBlockChanged(BlockSource& source, const BlockPos& pos, FullBlock block, FullBlock oldBlock, int updateFlags, Entity* changer) {
	if(getBlockSourceDEPRECATEDUSEPLAYERREGIONINSTEAD().getDimensionId() != source.getDimensionId()) {
		return;
	}

	if(block.id == 0xff) {
		block.id = source.getBlockID(pos);
	}

	if (!mLevel.isClientSide()) {
// 		sendPacketForPosition(pos, UpdateBlockPacket(pos, block, updateFlags));

		//
		// HACK/TODO/FIXME: TEPackets must come after the updateBlockPacket, so flush block packets
		// before sending TEpackets
		//
		//tell the clients to update the TE too if it's a te
		//TODO: remove this from here
// 		if (block.id > 0 && Block::mBlocks[block.id]->hasBlockEntity()) {
// 			auto blockEntity = source.getBlockEntity(pos);
// 			if (blockEntity) {
// 				if (auto packet = blockEntity->getUpdatePacket(source)) {
// 					sendPacketForPosition(pos, packet->setReliableOrdered());	// HACK: send reliable AFTER the block update
// 				}
// 			}
// 		}
	}
}

bool Dimension::isUltraWarm() const {
	return mUltraWarm;
}

void Dimension::setUltraWarm(bool warm) {
	mUltraWarm = warm;
}

bool Dimension::hasCeiling() const {
	return mHasCeiling;
}

void Dimension::setCeiling(bool ceiling) {
	mHasCeiling = ceiling;
}

// void Dimension::onBlockEvent( BlockSource& source, int x, int y, int z, int b0, int b1 ) {
// 	if(getBlockSourceDEPRECATEDUSEPLAYERREGIONINSTEAD().getDimensionId() != source.getDimensionId()) {
// 		return;
// 	}
// 
// 	if(!mLevel.mIsClientSide) {
// 		BlockEventPacket packet(BlockPos(x, y, z), b0, b1);
// 		sendBroadcast(packet);
// 	}
// }
// 
// void Dimension::sendBroadcast(const Packet& packet, Player* except /* = nullptr */) {
// 	DEBUG_ASSERT(!mLevel.mIsClientSide, "Sending broadcast from the client is not recommended");
// 
// 	mLevel.forEachPlayer([this, &except, &packet](Player& player) {
// 		//TODO have a list of players per dimension
// 		if(player.getDimensionId() == mId && except != &player) {
// 			mLevel.mPacketSender->send(player.mOwner, packet);
// 		}
// 
// 		return true;
// 	});
// }
// 
// void Dimension::sendPacketForPosition(const BlockPos& position, const Packet& packet, const Player* except) {
// 	DEBUG_ASSERT(!mLevel.mIsClientSide, "Sending broadcast from the client is not recommended");
// 
// 	mLevel.forEachPlayer([this, &except, &packet, &position](Player& player) {
// 		if (except != &player && player.isPositionRelevant(mId, position)) {
// 			mLevel.mPacketSender->send(player.mOwner, packet);
// 		}
// 		return true;
// 	});
// }
// 
// void Dimension::sendPacketForEntity(const Entity& entity, const Packet& packet, const Player* except) {
// 	DEBUG_ASSERT(!mLevel.mIsClientSide, "Sending broadcast from the client is not recommended");
// 
// 	mLevel.forEachPlayer([this, &except, &packet, &entity](Player& player) {
// 		if (except != &player && player.isEntityRelevant(entity)) {
// 			mLevel.mPacketSender->send(player.mOwner, packet);
// 		}
// 
// 		return true;
// 	});
// }

void Dimension::_completeEntityTransfer(BlockSource& region, std::unique_ptr<Entity> entity) {
// 	DimensionId fromDimensionId = entity->getDimensionId();
// 	
// 	Entity* e = entity.get();
// 	e->setRegion(region);
// 	
// 	if (fromDimensionId != TheEnd) {
// 		mLevel.getPortalForcer().force(*e);		
// 	}
// 	else {
// 		Vec3 p = e->getPos();
// 		p.y = region.getAboveTopSolidBlock(BlockPos(p));
// 		e->setPos(p);
// 	}
// 	
// 	mLevel.addEntity(region, std::move(entity));	
}