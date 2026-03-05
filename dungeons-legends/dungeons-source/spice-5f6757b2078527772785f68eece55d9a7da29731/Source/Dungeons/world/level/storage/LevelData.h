/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/
#pragma once

#include "CommonTypes.h"

#include "nbt/CompoundTag.h"
#include "world/Difficulty.h"
#include "world/level/BlockPos.h"
#include "world/level/GeneratorType.h"
#include "world/level/GameType.h"
//#include "world/level/storage/PlayerData.h"
#include "world/level/storage/StorageVersion.h"
#include "world/level/Tick.h"
#include "world/level/storage/GameRules.h"
#include "world/item/ItemInstance.h"
// #include "world/inventory/FixedInventoryContainer.h"
#include "GameVersion.h"

class LevelSettings;
class CompoundTag;
class ListTag;

struct AdventureSettings {
	AdventureSettings();

	bool doTickTime;
	bool noPvM;
	bool noMvP;
	bool immutableWorld;
	bool showNameTags;
	bool autoJump;
};

class LevelData {
public:
	//typedef std::vector<std::unique_ptr<ItemInstance>> FixedInventoryVector;

	LevelData();
	LevelData(const LevelSettings& settings, const std::string& levelName,
			  GeneratorType generatorVersion, const BlockPos& defaultSpawn,
			  bool achievementsDisabled, bool isEduLevel, float rainLevel,
			  float lightningLevel);
	LevelData(const CompoundTag& tag);
	LevelData(LevelData&& rhs);
	LevelData& operator=(LevelData&& rhs);

	LevelData(const LevelData&) = delete;
	LevelData& operator=(const LevelData&) = delete;

	void v1_read(RakNet::BitStream& bitStream, StorageVersion storageVersion);

	Unique<CompoundTag> createTag() const;

	void getTagData(const CompoundTag& tag);
	void loadFixedInventoryData(CompoundTag& fixedInvData);
	void setTagData(CompoundTag& tag) const;

	RandomSeed      getSeed() const;
	const BlockPos& getSpawn() const;

	int     getTime() const;
	int     getStopTime() const;

	CompoundTag& getLoadedPlayerTag();
	CompoundTag& getLoadedFixedInventory();

	void        setSeed(RandomSeed seed);
	void        setSpawn(const BlockPos& spawn);
	void        setStopTime(int time);

	void        setTime(int time);

	void clearLoadedPlayerTag();

	const std::string& getLevelName() const;
	void        setLevelName(const std::string& levelName);

	GeneratorType           getGenerator() const;
	void        setGenerator(GeneratorType version);

	time_t      getLastPlayed() const;

	bool        isLightning() const;
	float       getLightningLevel() const;
	void        setLightningLevel(float level);
	int         getLightningTime() const;
	void        setLightningTime(int lightningTime);
	bool        isRaining() const;
	float       getRainLevel() const;
	void        setRainLevel(float level);
	int         getRainTime() const;
	void        setRainTime(int rainTime);

	GameType    getGameType() const;
	void        setGameType(GameType type);
	Difficulty  getGameDifficulty() const;
	void        setGameDifficulty(Difficulty difficulty);
	bool        getForceGameType() const;
	void        setForceGameType(bool value);

	bool        getSpawnMobs() const;
	void        setSpawnMobs(bool doSpawn);

	bool        hasAchievementsDisabled() const;
	void        disableAchievements();

	// if the level has been, or *will be*, loaded in creative or with cheats enabled
	bool		achievementsWillBeDisabledOnLoad() const;

	void recordStartUp();

	const BlockPos& getWorldCenter() const;

	uint32_t getWorldStartCount() const;

	StorageVersion getStorageVersion() const;
	void setStorageVersion(StorageVersion version);

	int getNetworkVersion() const;
	void setNetworkVersion(int version);

	const std::string& getOfferName() const;
	void setOfferName(const std::string& offerName);

	const Tick& getCurrentTick() const;
	void incrementTick();
	void setCurrentTick(Tick& CurrentTick);

	const GameVersion& getLastLoadedWithVersion() const {
		return mLastOpenedWithVersion;
	}
	
	AdventureSettings& getAdventureSettings();
	GameRules& getGameRules();
	//FixedInventoryContainer& getFixedInventory();
	bool isEduLevel() const;
	void setIsEduLevel(bool IsEduLevel);

	bool isMultiplayerGame() const;
	void setMultiplayerGame(bool multiplayer);
	bool hasLANBroadcast() const;
	void setLANBroadcast(bool broadcast);
	bool hasXBLBroadcast() const;
	void setXBLBroadcast(bool broadcast);

	bool hasCommandsEnabled() const;
	void setCommandsEnabled(bool commandsEnabled);

	bool isTexturepacksRequired() const;
	void setTexturepacksRequired(bool texturepacksRequired);

private:

	AdventureSettings mAdventureSettings;
	GameRules mGameRules;

	std::string mLevelName;
	StorageVersion mStorageVersion;
	int mNetworkVersion;
	Tick mCurrentTick;

	RandomSeed mSeed;
	BlockPos mSpawn;
	BlockPos mLimitedWorldOrigin;
	int mTime;
	time_t mLastPlayed;
	CompoundTag mLoadedPlayerTag;

	CompoundTag mLoadedFixedInventory;
	//std::unique_ptr<FixedInventoryContainer> mFixedInventorySlots;

	float mRainLevel;
	int mRainTime;
	float mLightningLevel;
	int mLightningTime;

	GameVersion mLastOpenedWithVersion;

	GameType mGameType;
	Difficulty mGameDifficulty;
	bool mForceGameType;
	bool mSpawnMobs;
	int mDayCycleStopTime;
	//@note: This version is never written or loaded to disk. The only purpose
	//       is to use it in the level generator on server and clients.
	GeneratorType mGenerator;

	uint32_t mWorldStartCount;

	bool mAchievementsDisabled;
	bool mIsEduLevel;

	bool mMultiplayerGame;
	bool mLANBroadcast;
	bool mXBLBroadcast;

	bool mCommandsEnabled;
	bool mTexturePacksRequired;
	std::string mOfferName; // Set if we create a level from a template
};
