/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/
#pragma once

#include "world/level/LevelListener.h"
#include "world/level/saveddata/SavedData.h"
//#include "platform/threading/ThreadLocal.h"
#include "world/level/ChunkPos.h"
#include "EntityUniqueID.h"
#include "world/level/biome/BiomeSource.h"
//#include "world/level/Spawner.h"
//#include "world/level/MobSpawnInfo.h"
//#include "world/level/LightLayer.h"

class Level;
class BiomeSource;
class ChunkSource;
class CompoundTag;
class BlockSource;
class Weather;
class Seasons;
// class MoveEntityPacketData;
// class CircuitSystem;
// class Packet;
class Vec3;
class Color;

enum class GeneratorType;

typedef std::unordered_map<EntityUniqueID, Entity*> EntityMap;

class Dimension :
	public LevelListener
	, public SavedData
	{
	friend class Level;
public:

	static const float MOON_BRIGHTNESS_PER_PHASE[];

	Dimension(Level& level, DimensionId dimId, Height height);
	virtual ~Dimension();

	static std::unique_ptr<Dimension> createNew(DimensionId id, Level& level);

	virtual void init();
	virtual void tick();
	virtual void tickRedstone();

	virtual void updateLightRamp();

	virtual bool isNaturalDimension() const;
	virtual bool isValidSpawn(int x, int z) const;
	virtual Color getFogColor(float br) const;

	virtual float getFogDistanceScale() const;

	virtual bool isFoggyAt(int x, int z) const;
	virtual float getCloudHeight() const;
	Height getSeaLevel() const {
		return mSeaLevel;
	}

	BiomeSource& getBiomes();

	virtual bool mayRespawn() const;
	virtual bool hasGround() const;
	virtual BlockPos getSpawnPos() const;
	virtual int getSpawnYPosition() const;
	virtual bool hasBedrockFog();
	virtual float getClearColorScale();

	virtual std::string getName() const = 0;

	Seasons& getSeasons();

	virtual bool showSky() const;
	virtual bool isDay() const;
	float getTimeOfDay(float a) const;
	float getSunAngle(float a);
	Vec3 getSunlightDirection(float a);
	float getSunIntensity(float a, const Entity& camera, float minInfluenceAngle);
	int getMoonPhase() const;
	float getMoonBrightness() const;
	float getSkyDarken(float a) const;
	Brightness getOldSkyDarken(float a);
	Color getSunriseColor(float a) const;
	Color getSkyColor(const Entity& source, float a);
	Color getSkyColor(BlockSource& source, const BlockPos& pos, float a);
	Color getCloudColor(float a);
	float getStarBrightness(float a);
	Height getHeight() const {
		return mHeight;
	}

	DimensionId getId() const;

	const Level& getLevelConst() const;
	Level& getLevel() const;
	ChunkSource& getChunkSource() const;
	BlockSource& getBlockSourceDEPRECATEDUSEPLAYERREGIONINSTEAD() const;
	Weather& getWeather() const;
// 	CircuitSystem& getCircuitSystem();

	EntityMap& getEntityIdMap();
	const EntityMap& getEntityIdMapConst() const;
	Entity* fetchEntity(EntityUniqueID entityId, bool getRemoved = false);

	virtual Vec3 translatePosAcrossDimension(const Vec3& originalPos, DimensionId fromId) const;
	void transferEntity(const Vec3& spawnPos, std::unique_ptr<CompoundTag> entityTag);

	// from LevelListener
	//virtual void onNewChunkFor(Player& p, LevelChunk& lc) override;

	// from SavedData
	virtual void load(const CompoundTag& tag) override;
	virtual void save(CompoundTag& tag) override;

	//virtual void sendBroadcast(const Packet& packet, Player* except = nullptr);

	//void sendPacketForPosition(const BlockPos& position, const Packet& packet, const Player* except = nullptr);
	//void sendPacketForEntity(const Entity& entity, const Packet& packet, const Player* except = nullptr);
	//virtual void onBlockEvent(BlockSource& source, int x, int y, int z, int b0, int b1) override;
	virtual void onBlockChanged(BlockSource& source, const BlockPos& pos, FullBlock block, FullBlock oldBlock, int updateFlags, Entity* changer) override;
	
	bool isUltraWarm() const;
	void setUltraWarm(bool warm);

	bool hasCeiling() const;
	void setCeiling(bool ceiling);
	
	//TODO move this to the client only
	bool hasUpsideDownLight() const {
		return hasCeiling();
	}

	Brightness getSkyDarken() const;

	//const LightLayer& getSkyLightLayer() const {
	//	return mSkyLightLayer;
	//}

	Brightness getSkyLight() const {
		//return getSkyLightLayer().getSurrounding();
		return Brightness();
	}

	bool hasWeather() const {
		return mHasWeather;
	}
	void setHasWeather(bool weather) {
		mHasWeather = weather;
	}

	const float* getBrightnessRamp() const; 
	void setBrightnessRamp(unsigned index, float brightness);

	bool isRedstoneTick();

	void addWither(const EntityUniqueID& id);
	void removeWither(const EntityUniqueID& id);

	float getPopCap(int categoryID, bool surface) const;
	
protected:
	virtual float getTimeOfDay(int time, float a) const;

	std::unique_ptr<ChunkSource> _createGenerator(GeneratorType type);
	void _completeEntityTransfer(BlockSource& region, std::unique_ptr<Entity> entity);

	Level& mLevel;

	Height mSeaLevel;
	//ThreadLocal<BiomeSource> mBiomeSource;
	BiomeSource mBiomeSource = 0;
	std::unique_ptr<BlockSource> mBlockSource;

	//LightLayer mSkyLightLayer;

	//float mMobsPerChunkSurface[MobSpawnInfo::MOBCATEGORY_COUNT];
	//float mMobsPerChunkUnderground[MobSpawnInfo::MOBCATEGORY_COUNT];

private:

	DimensionId mId;
	bool mUltraWarm = false;
	bool mHasCeiling = false;
	bool mHasWeather = false;
	Brightness mSkyDarken;
	Height mHeight;

	float mBrightnessRamp[16];
		
	std::unique_ptr<ChunkSource> mChunkSource;
	std::unique_ptr<Weather> mWeather;
	std::unique_ptr<Seasons> mSeasons;
// 	Unique<CircuitSystem> mCircuitSystem;
	const int CIRCUIT_TICK_RATE = 2;
	int mCircuitSystemTickRate = CIRCUIT_TICK_RATE;
	EntityMap mEntityIdLookup;

	typedef std::vector<std::unique_ptr<CompoundTag> > EntityTagList;
	typedef std::unordered_map<ChunkPos, EntityTagList> ChunkPosToEntityListMap;
	ChunkPosToEntityListMap mLimboEntities;

	std::set<EntityUniqueID> mWitherIDs;
};
