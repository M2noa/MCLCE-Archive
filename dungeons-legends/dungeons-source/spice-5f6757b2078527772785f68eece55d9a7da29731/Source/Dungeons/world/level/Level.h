/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/

#pragma once

//#include "AppPlatformListener.h"
#include "Core/Utility/UUID.h"
#include "EntityUniqueID.h"
//#include "network/packet/PlayerListPacket.h"
#include "material/MaterialType.h"
#include "util/Random.h"
#include "world/Difficulty.h"
#include "world/level/BlockSourceListener.h"
#include "world/level/ChunkPos.h"
//#include "world/level/LightUpdate.h"
#include "world/level/storage/LevelData.h"
#include "world/level/storage/SavedDataStorage.h"
#include "LevelConstants.h"
#include "world/entity/EntityTypes.h"

#include "world/phys/Vec2.h"

#include "SharedConstants.h"
#include "Core/Math/Color.h"
#include "world/phys/HitResult.h"

class AABB;
class Biome;
//class BlockEntity;
class BlockSource;
class BlockTickingQueue;
class BossEventListener;
class ChunkSource;
class DBChunkStorage;
class Dimension;
class Entity;
class EntityDamageSource;
class LevelChunk;
class LevelListener;
class LevelStorage;
class LightLayer;
class MapItemSavedData;
class Material;
//class Minecraft;
class MinecraftClient;
//class Mob;
class Spawner;
class ProjectileFactory;
class MobSpawnerData;
class NetEventCallback;
class PacketSender;
class Particle;
class Path;
//class Player;
class PlayerListEntry;
class PortalForcer;
class SavedData;
class SavedDataStorage;
class SoundPlayer;
class Villages;
class MapItemSavedData;
//class Zombie;
class CircuitSystem;
class TextureData;
class EntityDefinitionGroup;
class MinecraftEventing;
class ResourcePackManager;
class StructureManager;
enum class BossEventUpdateType : int;
enum class EntityEvent : byte;
enum class LevelEvent : short;
enum class LevelSoundEvent : unsigned char;
enum class ParticleType;

enum class EntityLocation;


class _TickPtr {
	friend class Level;
	virtual void invalidate() = 0;

public:

	virtual ~_TickPtr() {

	}

};

typedef std::unordered_map<int, Unique<Dimension> > DimensionMap;
typedef std::vector<LevelListener*> ListenerList;
//typedef std::vector<LightUpdate> LightUpdateList;

typedef struct PRInfo {
	PRInfo(Entity* e, int ticks) :
		e(e)
		, ticks(ticks) {
	}

	Entity* e;
	int ticks;
} PRInfo;
typedef std::vector<PRInfo> PendingList;


//class ChangeDimensionRequest {
//public:
//
//	enum class State {
//		PrepareRegion,
//		WaitingForChunks,
//		WaitingForRespawn
//	};
//
//	ChangeDimensionRequest(Minecraft* mc, DimensionId fromId, DimensionId toId, const Vec3& position, bool usePortal, bool respawn)
//		: mMinecraft(mc)
//		, mFromDimensionId(fromId)
//		, mToDimensionId(toId)
//		, mPosition(position)
//		, mUsePortal(usePortal)
//		, mRespawn(respawn) {
//	}
//
//	State mState = State::PrepareRegion;
//
//	Minecraft* mMinecraft;
//	DimensionId mFromDimensionId;
//	DimensionId mToDimensionId;
//	Vec3 mPosition;
//	bool mUsePortal;
//	bool mRespawn;
//};

class PlayerSuspension {
public:
	enum class State {
		Suspend,
		Resume
	};

	PlayerSuspension(const mce::UUID& id, State suspensionState)
		: mId(id)
		, mSuspensionState(suspensionState) {
	}

	mce::UUID mId;
	State mSuspensionState;
};

class Level :
	public BlockSourceListener {
	friend class Dimension;
	friend class ServerNetworkHandler;
	friend class ServerLevel;
	typedef std::unordered_set<BlockSource*> RegionSet;

	struct LevelChunkQueuedSavingElement {
		LevelChunkQueuedSavingElement(ChunkPos pos, DimensionId id, int dist) {
			mPosition = pos;
			mDimensionId = id;
			mDist = dist;
		}

		int mDist; // how far away is the chunk?  Nearer chunks get prioritized
		ChunkPos mPosition;  // where is the chunk?
		DimensionId mDimensionId; // which dimension is the chunk?
	};

	class CompareLevelChunkQueuedSavingElement {
	public:
		bool operator()(const LevelChunkQueuedSavingElement &lhs, const LevelChunkQueuedSavingElement &rhs) {
			return rhs.mDist < lhs.mDist; // want to ensure shorter distances get handled first
		}
	};

public:

	static const int MAX_LEVEL_SIZE = 32000000;

	static const int CLOUD_HEIGHT = 128;
	static const int NOON_TICKS = 5000;
	static const int TICKS_PER_DAY = SharedConstants::TicksPerSecond * 60 * 20;
	static const int TICKS_PER_MOON_MONTH = TICKS_PER_DAY * 8;
	static const int MIDDLE_OF_NIGHT_TIME = 12000;

	static const int DIFFICULTY_TIME_GLOBAL_OFFSET = TICKS_PER_DAY * -3;
	static const int MAX_DIFFICULTY_TIME_GLOBAL = TICKS_PER_DAY * 6;
	static const int MAX_DIFFICULTY_TIME_LOCAL = TICKS_PER_DAY * 150;

	//static std::string createUniqueLevelID(int modifier = 0);
	static RandomSeed createRandomSeed();

	Level(Unique<LevelStorage> levelStorage);
	//Level(SoundPlayer& soundPlayer, Unique<LevelStorage> levelStorage, const std::string& levelName, const LevelSettings& settings, bool isClientSide, MinecraftEventing& eventing, ResourcePackManager& addOnResourcePackManager, StructureManager& structureManager);
	virtual ~Level();

	//Dimension& createDimension(DimensionId id);
	Dimension* getDimension(DimensionId id) const;
	//PortalForcer& getPortalForcer();
	//void requestPlayerChangeDimension(Player& player, Unique<ChangeDimensionRequest> changeRequest);
	//void entityChangeDimension(Entity& entity, DimensionId toId);

	//void loadPlayer(Unique<Player> newPlayer);

	//Spawner& getSpawner() const;
	//ProjectileFactory& getProjectileFactory() const;
	//EntityDefinitionGroup& getEntityDefinitions() const;

	Difficulty getDifficulty() const;
	//bool isMultiplayerGame() const;
	//bool hasLANBroadcast() const;
	//bool hasXBLBroadcast() const;
	//float getSpecialMultiplier(DimensionId dimension);

	//bool hasCommandsEnabled() const;

	//int getLightsToUpdate();
	//void updateLights();
	//void setUpdateLights(bool doUpdate);

	// New ONLY way to add entities (non-Player) to the 
	//virtual Entity * addEntity(BlockSource &region, Unique<Entity> e);
	//virtual Entity * addGlobalEntity(BlockSource &region, Unique<Entity> e);
	//virtual Entity * addAutonomousEntity(BlockSource &region, Unique<Entity> e);
	//virtual void addPlayer(Unique<Player> player);
	//void suspendPlayer(Player& player);
	//void resumePlayer(Player& player);

	//bool isPlayerSuspended(Player& player) const;

	//virtual Unique<Entity> takeEntity(EntityUniqueID entityId);
	//virtual Unique<Entity> borrowEntity(EntityUniqueID entityId, LevelChunk* lc = nullptr);

	//void getEntities(DimensionId dimension, EntityType type, const AABB& aabb, std::vector<Entity*>& entities, bool ignoreTargetType = false);
	//Entity* fetchEntity(EntityUniqueID entityId, bool getRemoved = false) const;
	//Entity* getRuntimeEntity(EntityRuntimeID entityId, bool getRemoved = false) const;
	//Mob* getMob(EntityUniqueID entityId) const;
	//Player* getPlayer(const std::string& name) const;
	//Player* getPlayer(const mce::UUID& name) const;
	//Player* getPlayer(EntityUniqueID entityID) const;
	//Player* getRuntimePlayer(EntityRuntimeID runtimeId) const;
	//Player* getNextPlayer(const EntityUniqueID& entityID, bool includeLocal = false);	//needed for follow player
	//																					// mechanic
	//Player* getPrevPlayer(const EntityUniqueID& entityID, bool includeLocal = false);
	//int getNumRemotePlayers();
	//Player* getLocalPlayer() const;
	//Player* getRandomPlayer();
	//int32_t getNewPlayerId() const;
	//MinecraftEventing& getEventing();
	//Color getPlayerColor(const Player& player) const;
	//const Tick& getCurrentTick() const;
	//void removeAllNonPlayerEntities(EntityUniqueID keepEntityId = EntityUniqueID());

	StructureManager& getStructureManager();

	//void addListener(LevelListener& listener);
	//void removeListener(LevelListener& listener);

	//void tickEntities();

	//virtual void onPlayerDeath(Player& player, const EntityDamageSource& source);

	//virtual void tick();

	//virtual void /*directTickEntities*/(BlockSource& blockSource);

	//void animateTick(Entity& relativeTo);

	//void explode(BlockSource& region, Entity* source, const Vec3& pos, float r, bool fire = false, bool breaksBlocks = true, float maxResistance = std::numeric_limits<float>::max());
	//void denyEffect(const Vec3& pos);
	//void potionSplash(const Vec3& pos, const Color& color, bool instantaneousEffect);

	//bool checkAndHandleMaterial(const AABB& box, MaterialType material, Entity* e);
	//bool checkMaterial(const AABB& box, MaterialType material, Entity* e);
	//bool extinguishFire(BlockSource& region, const BlockPos& pos, FacingID face);

	bool isDayCycleActive();

	//void setDayCycleActive(bool active);

	//Unique<Path> findPath(Entity& from, Entity& to, float maxDist, bool canPassDoors, bool canOpenDoors, bool avoidWater, bool canFloat, bool avoidPortals);

	//Unique<Path> findPath(Entity& from, int xBest, int yBest, int zBest, float maxDist, bool canPassDoors, bool canOpenDoors, bool avoidWater, bool canFloat, bool avoidPortals);

	//Player* getNearestPlayer(Entity& source, float maxDist);
	//Player* getNearestPlayer(float x, float y, float z, float maxDist);
	//Player* getNearestAttackablePlayer(Entity& source, float maxDist);
	//Player* getNearestAttackablePlayer(BlockPos source, float maxDist, Entity* sourceEntity = nullptr);

	// Not currently used in MC either but will perhaps be used later.
	//virtual void updateSleepingPlayerList();

	int getTime() const;
	int setTime(int time);
	int setStopTime(int time);
	RandomSeed getSeed();
	//const BlockPos& getSharedSpawnPos();
	//void setDefaultSpawn(const BlockPos& spawnPos);
	//const BlockPos& getDefaultSpawn() const;
	//void setSpawnSettings(bool spawnEntities);
	//bool getSpawnEntities() const;

	//virtual void setDifficulty(Difficulty difficulty);
	//void setMultiplayerGame(bool multiplayerGame);
	//void setLANBroadcast(bool broadcast);
	//void setXBLBroadcast(bool broadcast);

	//virtual void setCommandsEnabled(bool commandsEnabled);

	bool hasLevelStorage() const;
	LevelStorage& getLevelStorage();
	LevelData& getLevelData();
	const LevelData& getLevelData() const;

	//void saveLevelData();
	//void saveGameData();
	//void savePlayers();
	//void saveAutonomousEntities();
	//void loadAutonomousEntities();
	//void saveBiomeData();
	//void saveDirtyChunks();

	//Particle* addParticle(ParticleType id, const Vec3& pos, const Vec3& dir, int data = 0);

	//void playSound(LevelSoundEvent type, Vec3 const& pos, int blockID = -1, EntityType entityType = EntityType::Undefined, bool isBabyMob = false, bool isGlobal = false);
	// Restrict sound to be played only if the sound is from the same dimension that the local player is in 
	// So far, only global entities, like dragon, are ticked locally but can be in different dimension from the player's
	//void playSound(BlockSource& region, LevelSoundEvent type, Vec3 const& pos, int blockID = -1, EntityType entityType = EntityType::Undefined, bool isBabyMob = false, bool isGlobal = false);
	//void playSynchronizedSound(BlockSource& region, LevelSoundEvent type, Vec3 const& pos, int blockID = -1, EntityType entityType = EntityType::Undefined, bool isBabyMob = false, bool isGlobal = false);

	void handleLevelEvent(LevelEvent type, Vec3 const& pos, int data);
	//void handleSoundEvent(LevelSoundEvent type, Vec3 const& pos, int blockID, EntityType entityType = EntityType::Undefined, bool isBabyMob = false, bool isGlobal = false);
	void broadcastLevelEvent(LevelEvent type, Vec3 const& pos, int data, Player* p = nullptr);
	void broadcastDimensionEvent(BlockSource& region, LevelEvent type, Vec3 const& pos, int data, Player* except = nullptr);
	//void broadcastSoundEvent(BlockSource& region, LevelSoundEvent type, Vec3 const& pos, int blockID = -1, EntityType entityType = EntityType::Undefined, bool isBabyMob = false, bool isGlobal = false);

	//void broadcastEntityEvent(Entity* e, EntityEvent eventId);

	//void addBossEventListener(BossEventListener* listener);
	//void removeBossEventListener(BossEventListener* listener);
	//void broadcastBossEvent(BossEventUpdateType type, Mob* boss);

	//void awakenAllPlayers();

	//std::string getPlayerNames();
	//int getActivePlayerCount() const;

	void forEachPlayer(std::function<bool(Player&)> callback);
	void forEachPlayer(std::function<bool(const Player&)> callback) const;

	//Player* findPlayer(std::function<bool(const Player&)> pred) const;

	//int getUserCount() const;

	//std::vector<Unique<Player>>& getUsers();
	//const std::vector<Unique<Player>>& getUsers() const;

	//virtual void runLightUpdates(BlockSource& source, const LightLayer& layer, const BlockPos& min, const BlockPos& max);

	virtual void onSourceCreated(BlockSource& source) override;
	virtual void onSourceDestroyed(BlockSource& source) override;

	virtual void onNewChunkFor(Player& p, LevelChunk& lc);
	virtual void onChunkLoaded(LevelChunk& lc);

	void onChunkDiscarded(LevelChunk& lc);

	virtual void queueEntityRemoval(Unique<Entity>&& removed, bool discardingChunk = false);
	virtual void removeEntityReferences(Entity& removed, bool discardingChunk = false);

	//void forceRemoveEntity(Entity& e);
	//void forceFlushRemovedPlayers();

	//AI pointers stuff - //TODO maybe clean
	//void registerTemporaryPointer(_TickPtr& ptr);
	//void unregisterTemporaryPointer(_TickPtr& ptr);

	//bool destroyBlock(BlockSource& region, const BlockPos& pos, bool dropResources);

	//void upgradeStorageVersion(StorageVersion v);

	//virtual void onAppSuspended();
	//void waitAsyncSuspendWork();

	//bool isUpdatingLights() const;

	//void _destroyEffect(const BlockPos& pos, FullBlock block, const Vec3& s, float soundScale = 1.0f);

	EntityUniqueID getNewUniqueID();
	//EntityRuntimeID getNextRuntimeID();

	//const std::array<ChunkPos, 57>& getTickingOffsets() const {
	//	return tickingChunksOffset;
	//}

	//bool isExporting() const;
	//void setIsExporting(bool IsExporting);
	//FixedInventoryContainer& getFixedInventory();

	//SavedDataStorage& getSavedData() const;
	//void setSavedData(const std::string& id, SavedData* data);

	//bool doesMapExist(const EntityUniqueID uuid);
	//MapItemSavedData& getMapSavedData(const EntityUniqueID uuid);
	//void requestMapInfo(const EntityUniqueID uuid);
	//MapItemSavedData& createMapSavedData(const EntityUniqueID& uuid = EntityUniqueID());
	//MapItemSavedData& createMapSavedData(const std::vector<EntityUniqueID>& mapIds);

	//std::string getScreenshotsFolder() const;

	//std::string getLevelId() const;
	//void setLevelId(std::string LevelId);

	//virtual ResourcePackManager* getClientResourcePackManager() const;
	//virtual ResourcePackManager* getServerResourcePackManager() const;

protected:

	static std::array<ChunkPos, 57> tickingChunksOffset;

private:

	//std::vector<Unique<Player>> mPlayers;

	PlayerList mActivePlayers;
	PlayerList mSuspendedPlayers;
	//std::vector<PlayerSuspension> mSuspendResumeList;

	//void _backgroundTickSeasons();

	//void _syncTime(int time);
	//void _tickTemporaryPointers();

public:

	//int isNightMode() const;
	//void setNightMode(bool isNightMode);

	bool isClientSide() const;
	void setIsClientSide(bool val);

	//Unique<Villages>& getVillages();

	//std::unordered_map<mce::UUID, PlayerListEntry>& getPlayerList();

	//GlobalEntityList& getGlobalEntities();
	//AutonomousEntityList& getAutonomousEntities();
	//// Search for entity in autonomous entities list, meaning only autonomous entities in loaded chunks
	//Entity* getAutonomousActiveEntity(EntityUniqueID id) const;
	//// Search for entity in autonomous loaded entities list, meaning only entities in unload chunks
	//Entity* getAutonomousInactiveEntity(EntityUniqueID id) const;
	//// Search for entity in both autonomous entity lists, meaning autonomous entities in loaded or unloaded chunks
	//Entity* getAutonomousEntity(EntityUniqueID id) const;
	//OwnedEntityList& getAutonomousLoadedEntities();
	//Entity* moveAutonomousEntityTo(const Vec3& to, EntityUniqueID id, BlockSource& toRegion);
	//PacketSender* getPacketSender() const;
	//void setPacketSender(PacketSender* val);

	//void sendFixedInventoryUpdatePacket();
	//NetEventCallback* getNetEventCallback() const;
	//void setNetEventCallback(NetEventCallback* val);

	Random& getRandom() const;

	//HitResult& getHitResult();

	//AdventureSettings& getAdventureSettings();
	//GameRules& getGameRules();

	bool getTearingDown() const;

	//void takePicture(TextureData & outImage, Entity* camera, Entity* target, bool applySquareFrame = false, const std::string & filename = "");
	//void setFixedInventorySlot(int slot, ItemInstance* item);
	//ItemInstance* getFixedInventorySlot(int slot);

//	int getFixedInventorySlotCount();
//	void setFixedInventorySlotCount(int val);
//
//	SoundPlayer& getSoundPlayer() {
//		return mSoundPlayer;
//	}
//
//	void setSimPaused(bool value) {
//		mSimPaused = value;
//	}
//
//	bool getSimPaused(void) {
//		return mSimPaused;
//	}
//
//	void setFinishedInitializing();
//
//protected:
//
//	SoundPlayer& mSoundPlayer;
//
	bool mIsFindingSpawn = false;
	bool mSpawnEntities = true;
	EntityUniqueID mLastUniqueID;
//	EntityRuntimeID mLastRuntimeID = EntityRuntimeID(0);
//	std::unordered_map<EntityRuntimeID, EntityUniqueID> mRuntimeIdMap;
//
	ListenerList m_Listeners;
	Unique<LevelStorage> mLevelStorage;
//	Unique<SavedDataStorage> mSavedDataStorage;
	LevelData mLevelData;

	RegionSet mRegions;

	bool mNightMode = false;
	bool mLightUpdateRunning = false;

//	Unique<EntityDefinitionGroup> mEntityDefinitions;
//	Unique<Spawner> mMobSpawner;
//	Unique<ProjectileFactory> mProjectileFactory;

	DimensionMap mDimensions;
//	Unique<PortalForcer> mPortalForcer;
//
//	typedef std::vector<BossEventListener*> BossEventListenerList;
//	BossEventListenerList mBossEventListeners;
//
//private:

	//typedef std::unordered_map<Player*, Unique<ChangeDimensionRequest> > ChangeDimensionRequestMap;

	//ChangeDimensionRequestMap mChangeDimensionRequests;

	//std::unordered_map<mce::UUID, PlayerListEntry> mPlayerList;

	//PacketSender* mPacketSender;

	//HitResult mHitResult;

	mutable Random mRandom, mAnimateRandom;
	//NetEventCallback* mNetEventCallback = nullptr;

	OwnedEntitySet mPendingEntitiesToRemove;
	bool mUpdatingBlockEntities = false;

	GlobalEntityList mGlobalEntities;
	AutonomousEntityList mAutonomousEntities;
	OwnedEntityList mAutonomousLoadedEntities;

	//Unique<Villages> mVillages;

	bool mIsClientSide;
	bool mIsExporting;

	//LightUpdateList mLightUpdates;
	//int mAvailableImmediateLightUpdates = 0;

	double m_LastSaveDataTime = 0;
	PendingList mPendingPlayerRemovals;

	int mLastTimePacketSent = 0;

	bool mChunkSaveInProgress = false;
	bool mSimPaused = false;

	std::string mLevelId;

	//std::priority_queue <LevelChunkQueuedSavingElement, std::vector<LevelChunkQueuedSavingElement>, CompareLevelChunkQueuedSavingElement> mLevelChunkSaveQueue;

	//std::unordered_set<_TickPtr*> mTempTickPointers;

	//std::unordered_map<EntityUniqueID, std::unique_ptr<MapItemSavedData>> mMapData;
	//bool _loadMapData(const EntityUniqueID& id);
	//void _saveMapData(MapItemSavedData& data);

	//// Todo: do not rely on this in other places
	bool mTearingDown = false;
	//MinecraftEventing& mEventing;

	StructureManager& mStructureManager;

	//Unique<ChunkSource> _createGenerator(GeneratorType type);
	//MapItemSavedData& _createMapSavedData(const EntityUniqueID& uuid);

	//void _saveSomeChunks();

	//void _cleanupDisconnectedPlayers();

	//void _removeAllPlayers();
	//void _validatePlayerName(Player& player);

	//bool _playerChangeDimension(Player* player, ChangeDimensionRequest& changeRequest);
	//void _handleChangeDimensionRequests();
	//void _handlePlayerSuspension();
	//void _suspendPlayer(const mce::UUID& id);
	//void _resumePlayer(const mce::UUID& id);
	//void _saveAllMapData();
	//void _saveDimensionStructures();
	//void _loadDimensionStructures(int dimensionId);

	//// ID of the more zoomed out version of this map
	//EntityUniqueID mParentMapId;
	int mMaxLightLoopCount = 0;
	int mMobSpawnTick = 0;

	bool mIsFinishedInitializing = false;
};
