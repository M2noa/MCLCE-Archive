/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/
#include "Dungeons.h"

#include "world/level/storage/LevelData.h"

// TODO: rherlitz

#include "nbt/CompoundTag.h"
#include "world/entity/player/Player.h"
#include "world/level/biome/BiomeSource.h"
#include "world/level/Level.h"
#include "world/level/LevelSettings.h"
// #include "world/entity/player/PermissionsHandler.h"

AdventureSettings::AdventureSettings()
	: doTickTime(true)
	, noPvM(false)
	, noMvP(false)
	, immutableWorld(false)
	, showNameTags(false)
	, autoJump(true) {
}

LevelData::LevelData()
	: mLevelName("")
	, mStorageVersion(SharedConstants::CurrentStorageVersion)
	, mNetworkVersion(SharedConstants::NetworkProtocolVersion)
	, mLastOpenedWithVersion(GameVersion::current())
	, mCurrentTick()
	, mSeed(0)
	, mSpawn(BlockPos(128, 64, 128))
	, mLimitedWorldOrigin(BlockPos::MIN)
	, mTime(0)
	, mLastPlayed(0)
	, mLoadedPlayerTag()
	, mRainLevel(0.0f)
	, mRainTime(0)
	, mLightningLevel(0.0f)
	, mLightningTime(0)
	, mGameType(GameType::Default)
	, mGameDifficulty(Difficulty::Normal)
	, mForceGameType(false)
	, mSpawnMobs((mGameType == GameType::Survival))
	, mDayCycleStopTime(-1)
	, mGenerator(GeneratorType::Overworld)
	, mWorldStartCount(0xffffffff)
	, mAchievementsDisabled(false)
	, mIsEduLevel(false)
	, mMultiplayerGame(true)
	, mLANBroadcast(true)
	, mXBLBroadcast(true)
	, mCommandsEnabled(false)
	, mTexturePacksRequired(false)
	//, mFixedInventorySlots(std::make_unique<FixedInventoryContainer>(nullptr, 0))
	, mOfferName() {
}

LevelData::LevelData(const LevelSettings& settings, const std::string& levelName,
					 GeneratorType generatorVersion, const BlockPos& defaultSpawn,
					 bool achievementsDisabled, bool isEduLevel, float rainLevel,
					 float lightningLevel)
	: mLevelName(levelName)
	, mStorageVersion(SharedConstants::CurrentStorageVersion)
	, mNetworkVersion(SharedConstants::NetworkProtocolVersion)
	, mLastOpenedWithVersion(GameVersion::current())
	, mCurrentTick()
	, mSeed(settings.getSeed())
	, mSpawn(BlockPos(128, 64, 128))
	, mLimitedWorldOrigin(BlockPos::MIN)
	, mTime(0)
	, mLastPlayed(0)
	, mLoadedPlayerTag()
	, mRainLevel(rainLevel)
	, mRainTime(0)
	, mLightningLevel(lightningLevel)
	, mLightningTime(0)
	, mGameType(settings.getGameType())
	, mGameDifficulty(settings.getGameDifficulty())
	, mForceGameType(settings.forceGameType())
	, mSpawnMobs(true)
	, mDayCycleStopTime(settings.getDayCycleStopTime())
	, mGenerator(generatorVersion)
	, mWorldStartCount(0xffffffff)
	, mAchievementsDisabled(achievementsDisabled)
	, mIsEduLevel(isEduLevel)
	, mMultiplayerGame(settings.isMultiplayerGame())
	, mLANBroadcast(settings.hasLANBroadcast())
	, mXBLBroadcast(settings.hasXBLBroadcast())
	, mCommandsEnabled(settings.hasCommandsEnabled())
	, mTexturePacksRequired(settings.isTexturepacksRequired())
	//, mFixedInventorySlots(std::make_unique<FixedInventoryContainer>(nullptr, 0))
	, mOfferName() {

	//find the spawn point
	if (defaultSpawn != BlockPos::MIN) {
		setSpawn(defaultSpawn);
	}
	else {
		setSpawn(BiomeSource(getSeed(), mGenerator).getSpawnPosition());
	}

	mRainTime = 2 * Level::TICKS_PER_DAY;
	mLightningTime = 4 * Level::TICKS_PER_DAY;
}

LevelData::LevelData( const CompoundTag& tag ) {
	getTagData(tag);
}

LevelData::LevelData(LevelData&& rhs) {
	*this = std::move(rhs);
}

LevelData& LevelData::operator=(LevelData&& rhs) {
	mLevelName = std::move(rhs.mLevelName);
	mStorageVersion = rhs.mStorageVersion;
	mNetworkVersion = rhs.mNetworkVersion;
	mCurrentTick = rhs.mCurrentTick;
	mSeed = rhs.mSeed;
	mSpawn = rhs.mSpawn;
	mLimitedWorldOrigin = rhs.mLimitedWorldOrigin;
	mTime = rhs.mTime;
	mLastPlayed = rhs.mLastPlayed;
	mLoadedPlayerTag = std::move(rhs.mLoadedPlayerTag);
	mRainLevel = rhs.mRainLevel;
	mRainTime = rhs.mRainTime;
	mLightningLevel = rhs.mLightningLevel;
	mLightningTime = rhs.mLightningTime;
	mGameType = rhs.mGameType;
	mGameDifficulty = rhs.mGameDifficulty;
	mForceGameType = rhs.mForceGameType;
	mSpawnMobs = rhs.mSpawnMobs;
	mDayCycleStopTime = rhs.mDayCycleStopTime;
	mGenerator = rhs.mGenerator;
	mWorldStartCount = rhs.mWorldStartCount;
	mAchievementsDisabled = rhs.mAchievementsDisabled;
	mIsEduLevel = rhs.mIsEduLevel;
	mMultiplayerGame = rhs.mMultiplayerGame;
	mLANBroadcast = rhs.mLANBroadcast;
	mXBLBroadcast = rhs.mXBLBroadcast;
	mCommandsEnabled = rhs.mCommandsEnabled;
	mTexturePacksRequired = rhs.mTexturePacksRequired;
	mGameRules = rhs.mGameRules;
	mLoadedFixedInventory = std::move(rhs.mLoadedFixedInventory);
	loadFixedInventoryData(mLoadedFixedInventory);
	mAdventureSettings = rhs.mAdventureSettings;
	mLastOpenedWithVersion = rhs.mLastOpenedWithVersion;
	mOfferName = rhs.mOfferName;

	return *this;
}

void LevelData::v1_read(RakNet::BitStream& bitStream, StorageVersion storageVersion){
	mStorageVersion = storageVersion;
	bitStream.Read(mSeed);
	bitStream.Read(mSpawn.x);
	bitStream.Read(mSpawn.y);
	bitStream.Read(mSpawn.z);
	bitStream.Read(mTime);
	bitStream.Read(mLastPlayed);
	RakNet::RakString rakName;
	bitStream.Read(rakName);
	mLevelName = rakName.C_String();
	bitStream.Read(mAchievementsDisabled);
	//LOGI("RBS: %d, %d, %d, %d, %d, %d\n", seed, xSpawn, ySpawn, zSpawn, time, sizeOnDisk);
}

Unique<CompoundTag> LevelData::createTag() const {
	auto tag = make_unique<CompoundTag>();
	setTagData(*tag);
	return tag;
}

void LevelData::setTagData(CompoundTag& tag) const {
	tag.putInt64("RandomSeed", mSeed);
	tag.putInt("GameType", static_cast<int>(mGameType));
	tag.putInt("Difficulty", static_cast<int>(mGameDifficulty));
	tag.putBoolean("ForceGameType", mForceGameType);
	tag.putInt("SpawnX", mSpawn.x);
	tag.putInt("SpawnY", mSpawn.y);
	tag.putInt("SpawnZ", mSpawn.z);
	tag.putInt64("Time", mTime);
	tag.putInt64("LastPlayed", static_cast<int>(std::time(nullptr)));
	tag.putString("LevelName", mLevelName);
	tag.putInt("StorageVersion", static_cast<int>(mStorageVersion));
	tag.putInt("NetworkVersion", mNetworkVersion);
	tag.putInt("Platform", 2);
	tag.putBoolean("spawnMobs", mSpawnMobs);
	tag.putInt("Generator", static_cast<int>(mGenerator));
	tag.putInt("LimitedWorldOriginX", mLimitedWorldOrigin.x);
	tag.putInt("LimitedWorldOriginY", mLimitedWorldOrigin.y);
	tag.putInt("LimitedWorldOriginZ", mLimitedWorldOrigin.z);
	tag.putInt("DayCycleStopTime", mDayCycleStopTime);
	tag.putInt64("worldStartCount", mWorldStartCount);
	tag.putInt64("currentTick", mCurrentTick.getTimeStamp());

	tag.putFloat("rainLevel", mRainLevel);
	tag.putInt("rainTime", mRainTime);
	tag.putFloat("lightningLevel", mLightningLevel);
	tag.putInt("lightningTime", mLightningTime);

	tag.putBoolean("hasBeenLoadedInCreative", mAchievementsDisabled);// use non-obvious name to slow down hackers and for backward compatibility
	tag.putBoolean("eduLevel", mIsEduLevel);
	tag.putBoolean("immutableWorld", mAdventureSettings.immutableWorld);

	tag.putBoolean("MultiplayerGame", mMultiplayerGame);
	tag.putBoolean("LANBroadcast", mLANBroadcast);
	tag.putBoolean("XBLBroadcast", mXBLBroadcast);

	tag.putBoolean("commandsEnabled", mCommandsEnabled);
	tag.putBoolean("texturePacksRequired", mTexturePacksRequired);

	// TODO: rherlitz
//  	tag.put("lastOpenedWithVersion", mLastOpenedWithVersion.serialize());
	tag.putString("offerName", mOfferName);

	mGameRules.setTagData(tag);
	//if (mFixedInventorySlots) {
	//	auto fixedInvData = make_unique<CompoundTag>();
	//	fixedInvData->put("fixedInventoryItems", mFixedInventorySlots->save());
	//	tag.putCompound("fixedInventory", std::move(fixedInvData));
	//}
}

void LevelData::getTagData(const CompoundTag& tag) {

	mSeed = (RandomSeed)tag.getInt64("RandomSeed");
	mAchievementsDisabled = tag.getBoolean("hasBeenLoadedInCreative");// use non-obvious name to slow down hackers and for backward compatibility
	setGameType(static_cast<GameType>(tag.getInt("GameType")));
	mForceGameType = tag.getBoolean("ForceGameType");
	mSpawn.x = tag.getInt("SpawnX");
	mSpawn.y = tag.getInt("SpawnY");
	mSpawn.z = tag.getInt("SpawnZ");
	mTime = static_cast<int>(tag.getInt64("Time"));
	mLastPlayed = static_cast<time_t>(tag.getInt64("LastPlayed"));
	mLevelName = tag.getString("LevelName");
	mStorageVersion = static_cast<StorageVersion>(tag.getInt("StorageVersion"));
	mNetworkVersion = tag.getInt("NetworkVersion");
	mGenerator = static_cast<GeneratorType>(tag.getInt("Generator"));
	mCurrentTick = static_cast<Tick>(tag.getInt64("currentTick"));

	mDayCycleStopTime = -1;

	if (tag.contains("DayCycleStopTime")) {
		mDayCycleStopTime = tag.getInt("DayCycleStopTime");
	}

	//set the limited world center for new & old worlds
	if (!tag.contains("LimitedWorldOriginX")) {
		mLimitedWorldOrigin = BlockPos(128, 64, 128);
	}
	else {
		mLimitedWorldOrigin.x = tag.getInt("LimitedWorldOriginX");
		mLimitedWorldOrigin.y = tag.getInt("LimitedWorldOriginY");
		mLimitedWorldOrigin.z = tag.getInt("LimitedWorldOriginZ");
	}

	if (tag.contains("spawnMobs")) {
		mSpawnMobs = tag.getBoolean("spawnMobs");
	}
	else {
		mSpawnMobs = true;
	}

	if (tag.contains("Player", Tag::Type::Compound)) {
		auto& data = *tag.getCompound("Player");
		mLoadedPlayerTag = std::move(*data.clone());
	}

	if (tag.contains("worldStartCount")) {
		mWorldStartCount = (uint32_t)tag.getInt64("worldStartCount");
	}
	else {
		mWorldStartCount = 0xffffffff;	//reverse start. check the explainer on EntityUniqueID.h

	}
	if (tag.contains("rainLevel")) {
		mRainLevel = tag.getFloat("rainLevel");
		mRainTime = tag.getInt("rainTime");
		mLightningLevel = tag.getFloat("lightningLevel");
		mLightningTime = tag.getInt("lightningTime");
	}
	if (tag.contains("fixedInventory")) {
		auto& data = *tag.getCompound("fixedInventory");
		mLoadedFixedInventory = std::move(*data.clone());
		loadFixedInventoryData(mLoadedFixedInventory);
	}
	else {
		mRainLevel = 0.0f;
		mRainTime = 2 * Level::TICKS_PER_DAY;
		mLightningLevel = 0.0f;
		mLightningTime = 4 * Level::TICKS_PER_DAY;
	}

	mIsEduLevel = tag.getBoolean("eduLevel");
	if (tag.contains("immutableWorld")) {
		mAdventureSettings.immutableWorld = tag.getBoolean("immutableWorld");
	}
	else {
		mAdventureSettings.immutableWorld = false;
	}

	mOfferName = tag.getString("offerName");

	mGameRules.getTagData(tag);
	if (tag.contains("Difficulty")) {
		mGameDifficulty = static_cast<Difficulty>(tag.getInt("Difficulty"));
		if (mGameDifficulty == Difficulty::Count) {
			mGameDifficulty = Difficulty::Normal;
		}
	}

	if (tag.contains("MultiplayerGame")) {
		mMultiplayerGame = tag.getBoolean("MultiplayerGame");
		mLANBroadcast = tag.getBoolean("LANBroadcast");
		mXBLBroadcast = tag.getBoolean("XBLBroadcast");
	}

	if (tag.contains("commandsEnabled")) {
		mCommandsEnabled = tag.getBoolean("commandsEnabled");
	}
	else {
		mCommandsEnabled = false;
	}

	if (tag.contains("texturePacksRequired")) {
		mTexturePacksRequired = tag.getBoolean("texturePacksRequired");
	} else {
		mTexturePacksRequired = false;
	}

	mLastOpenedWithVersion = {};
	if (auto versionTag = tag.getList("lastOpenedWithVersion")) {
		mLastOpenedWithVersion = GameVersion(*versionTag);
	}
}

void LevelData::loadFixedInventoryData(CompoundTag& fixedInvData) {
	//if (!mFixedInventorySlots) {
	//	return;
	//}
	//auto fixedInvItemList = fixedInvData.getList("fixedInventoryItems");
	//if (fixedInvItemList) {
	//	mFixedInventorySlots->load(*fixedInvItemList);
	//}
}

RandomSeed LevelData::getSeed() const {
	return mSeed;
}

const BlockPos& LevelData::getSpawn() const {
	return mSpawn;
}

int LevelData::getTime() const {
	return mTime;
}

int LevelData::getStopTime() const {
	return mDayCycleStopTime;
}

CompoundTag& LevelData::getLoadedPlayerTag() {
	return mLoadedPlayerTag;
}

CompoundTag& LevelData::getLoadedFixedInventory() {
	return mLoadedFixedInventory;
}

void LevelData::setSeed(RandomSeed seed) {
	mSeed = seed;
}

void LevelData::setSpawn( const BlockPos& spawn ){
	mSpawn = spawn;

	if (mLimitedWorldOrigin == BlockPos::MIN) {
		mLimitedWorldOrigin = spawn;
	}
}

void LevelData::setTime( int time ){
	mTime = time;
}

void LevelData::setStopTime(int time) {
	mDayCycleStopTime = time;
}

void LevelData::clearLoadedPlayerTag() {
	mLoadedPlayerTag = CompoundTag();
}

const std::string& LevelData::getLevelName() const {
	return mLevelName;
}

void LevelData::setLevelName( const std::string& levelName ){
	mLevelName = levelName;
}

GeneratorType LevelData::getGenerator() const {
	return mGenerator;
}

void LevelData::setGenerator( GeneratorType version ){
	mGenerator = version;
}

time_t LevelData::getLastPlayed() const {
	return mLastPlayed;
}

bool LevelData::isLightning() const {
	return mLightningLevel > 0.0f;
}

float LevelData::getLightningLevel() const {
	return mLightningLevel;
}

void LevelData::setLightningLevel(float level) {
	mLightningLevel = level;
}

int LevelData::getLightningTime() const {
	return mLightningTime;
}

void LevelData::setLightningTime(int lightningTime) {
	mLightningTime = lightningTime;
}

bool LevelData::isRaining() const {
	return mRainLevel > 0.0f;
}

float LevelData::getRainLevel() const {
	return mRainLevel;
}

void LevelData::setRainLevel(float level) {
	mRainLevel = level;
}

int LevelData::getRainTime() const {
	return mRainTime;
}

void LevelData::setRainTime(int rainTime) {
	mRainTime = rainTime;
}

GameType LevelData::getGameType() const {
	return mGameType;
}

void LevelData::setGameType(GameType type) {
	mGameType = type;
}

Difficulty LevelData::getGameDifficulty() const {
	return mGameDifficulty;
}

void LevelData::setGameDifficulty(Difficulty difficulty) {
	mGameDifficulty = difficulty;
}

bool LevelData::getForceGameType() const {
	return mForceGameType;
}

void LevelData::setForceGameType(bool value){
	mForceGameType = value;
}

bool LevelData::getSpawnMobs() const {
	return mSpawnMobs;
}

void LevelData::setSpawnMobs( bool doSpawn ){
	mSpawnMobs = doSpawn;
}

bool LevelData::hasAchievementsDisabled() const{
	return mAchievementsDisabled;
}

void LevelData::disableAchievements(){
	mAchievementsDisabled = true;
}

// if the level has been, or *will be*, loaded in creative or with cheats enabled
bool LevelData::achievementsWillBeDisabledOnLoad() const {
	return (hasAchievementsDisabled() || hasCommandsEnabled() || GameType::Creative == getGameType());
}

void LevelData::recordStartUp() {
	--mWorldStartCount;
}

const BlockPos& LevelData::getWorldCenter() const {
	return mLimitedWorldOrigin;
}

uint32_t LevelData::getWorldStartCount() const {
	return mWorldStartCount;
}

StorageVersion LevelData::getStorageVersion() const {
	return mStorageVersion;
}

void LevelData::setStorageVersion(StorageVersion version) {
	mStorageVersion = version;
}

int LevelData::getNetworkVersion() const {
	return mNetworkVersion;
}

void LevelData::setNetworkVersion(int version) {
	mNetworkVersion = version;
}

const std::string& LevelData::getOfferName() const {
	return mOfferName;
}

void LevelData::setOfferName(const std::string& offerName) {
	mOfferName = offerName;
}

const Tick& LevelData::getCurrentTick() const {
	return mCurrentTick;
}

void LevelData::incrementTick() {
	++mCurrentTick;
}

void LevelData::setCurrentTick(Tick& CurrentTick) {
	mCurrentTick = CurrentTick;
}


AdventureSettings& LevelData::getAdventureSettings() {
	return mAdventureSettings;
}

GameRules& LevelData::getGameRules() {
	return mGameRules;
}

bool LevelData::isEduLevel() const {
	return mIsEduLevel;
}

void LevelData::setIsEduLevel(bool IsEduLevel) {
	mIsEduLevel = IsEduLevel;
}

bool LevelData::isMultiplayerGame() const {
	return mMultiplayerGame;
}

//FixedInventoryContainer& LevelData::getFixedInventory() {
//	return *mFixedInventorySlots;
//}

void LevelData::setMultiplayerGame(bool multiplayer) {
	mMultiplayerGame = multiplayer;
}

bool LevelData::hasLANBroadcast() const {
	return mLANBroadcast;
}

void LevelData::setLANBroadcast(bool broadcast) {
	mLANBroadcast = broadcast;
}

bool LevelData::hasXBLBroadcast() const {
	return mXBLBroadcast;
}

void LevelData::setXBLBroadcast(bool broadcast) {
	mXBLBroadcast = broadcast;
}

bool LevelData::hasCommandsEnabled() const {
	return mCommandsEnabled;
}

void LevelData::setCommandsEnabled(bool commandsEnabled) {
	mCommandsEnabled = commandsEnabled;
}

bool LevelData::isTexturepacksRequired() const {
	return mTexturePacksRequired;
}

void LevelData::setTexturepacksRequired(bool texturepacksRequired) {
	mTexturePacksRequired = texturepacksRequired;
}
