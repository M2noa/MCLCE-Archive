/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/

#include "Dungeons.h"

#include "world/level/LevelSettings.h"
#include "world/level/storage/LevelData.h"


LevelSettings::LevelSettings()
	: mSeed(-1)
	, mGameType(GameType::Undefined)
	, mGameDifficulty(Difficulty::Count)
	, mForceGameType(false)
	, mGenerator(GeneratorType::Undefined)
	, mAchievementsDisabled(false)
	, mDimension(DimensionId::Undefined)
	, mDayCycleStopTime(-1)
	, mIsEduWorld(false)
	, mRainLevel(0.0f)
	, mLightningLevel(0.0f)
	, mDefaultSpawn()
	, mMultiplayerGame(false)
	, mLANBroadcast(false)
	, mXBLBroadcast(false)
	, mCommandsEnabled(false)
	, mTexturePacksRequired(false)
	, mOverrideSettings(false) {
}

LevelSettings::LevelSettings(const LevelData &data, GameType gameType, DimensionId dimension) :
	LevelSettings(
		data.getSeed(),
		gameType,
		data.getGameDifficulty(),
		dimension,
		data.getGenerator(),
		data.getSpawn(),
		data.hasAchievementsDisabled(),
		data.getStopTime(),
		data.isEduLevel(),
		data.getRainLevel(),
		data.getLightningLevel(),
		data.isMultiplayerGame(),
		data.hasLANBroadcast(),
		data.hasXBLBroadcast(),
		data.hasCommandsEnabled(),
		data.isTexturepacksRequired())
{

}

LevelSettings::LevelSettings(RandomSeed seed, GameType gameType, Difficulty gameDifficulty,
							 DimensionId dimension, GeneratorType generator, const BlockPos& defaultSpawn,
							 bool achievementsDisabled, int dayCycleStopTime, bool isEduWorld,
							 float rainLevel, float lightningLevel, bool multiplayerGame,
							 bool lanBroadcast, bool xblBroadcast, bool commandsEnabled, bool texturePackRequired)
	: mSeed(seed)
	, mGameType(gameType)
	, mGameDifficulty(gameDifficulty)
	, mForceGameType(false)
	, mGenerator(generator)
	, mAchievementsDisabled(achievementsDisabled)
	, mDimension(dimension)
	, mDayCycleStopTime(dayCycleStopTime)
	, mIsEduWorld(isEduWorld)
	, mRainLevel(rainLevel)
	, mLightningLevel(lightningLevel)
	, mDefaultSpawn(defaultSpawn)
	, mMultiplayerGame(multiplayerGame)
	, mLANBroadcast(lanBroadcast)
	, mXBLBroadcast(xblBroadcast)
	, mCommandsEnabled(commandsEnabled)
	, mTexturePacksRequired(texturePackRequired)
	, mOverrideSettings(false) {
}

const BlockPos& LevelSettings::getDefaultSpawn() const {
	return mDefaultSpawn;
}

RandomSeed LevelSettings::getSeed() const {
	return mSeed;
}

GameType LevelSettings::getGameType() const {
	return mGameType;
}

const DimensionId LevelSettings::getDimension() const{
	return mDimension;
}

Difficulty LevelSettings::getGameDifficulty() const {
	return mGameDifficulty;
}

int LevelSettings::getDayCycleStopTime() const {
	return mDayCycleStopTime;
}

void LevelSettings::setForceGameType(bool value) {
	mForceGameType = value;
}

const bool LevelSettings::forceGameType() const {
	return mForceGameType;
}

const bool LevelSettings::hasAchievementsDisabled() const {
	return mAchievementsDisabled;
}

const GeneratorType LevelSettings::getGenerator() const {
	return mGenerator;
}

const float LevelSettings::getRainLevel() const {
	return mRainLevel;
}

const float LevelSettings::getLightningLevel() const {
	return mLightningLevel;
}

const bool LevelSettings::isEduWorld() const {
	return mIsEduWorld;
}

void LevelSettings::setIsEduWorld(bool isEdu) {
	mIsEduWorld = isEdu;
}

bool LevelSettings::isMultiplayerGame() const {
	return mMultiplayerGame;
}

bool LevelSettings::hasLANBroadcast() const {
	return mLANBroadcast;
}

bool LevelSettings::hasXBLBroadcast() const {
	return mXBLBroadcast;
}

bool LevelSettings::hasCommandsEnabled() const {
	return mCommandsEnabled;
}

bool LevelSettings::isTexturepacksRequired() const {
	return 	mTexturePacksRequired;
}

void LevelSettings::setOverrideSavedSettings(bool overrideSaved) {
	mOverrideSettings = overrideSaved;
}

bool LevelSettings::shouldOverrideSavedSettings() const {
	return mOverrideSettings;
}

void LevelSettings::overrideSavedSettings(LevelData& data) const {
	data.setGameDifficulty(mGameDifficulty);
	data.setCommandsEnabled(mCommandsEnabled);
	data.setTexturepacksRequired(mTexturePacksRequired);
}

// if the level has been, or *will be*, loaded in creative or with cheats enabled
bool LevelSettings::achievementsWillBeDisabledOnLoad() const {
	return (hasAchievementsDisabled() || hasCommandsEnabled() || GameType::Creative == getGameType());
}

void LevelSettings::setNewWorldBehaviorPackIdentities(const std::vector<PackIdVersion>& identities) {
	mNewWorldBehaviorPackIdentities = identities;
}

const std::vector<PackIdVersion>& LevelSettings::getNewWorldBehaviorPackIdentities() const {
	return mNewWorldBehaviorPackIdentities;
}

void LevelSettings::setNewWorldResourcePackIdentities(const std::vector<PackIdVersion>& identities) {
	mNewWorldResourcePackIdentities = identities;
}

const std::vector<PackIdVersion>& LevelSettings::getNewWorldResourcePackIdentities() const {
	return mNewWorldResourcePackIdentities;
}

GameType LevelSettings::validateGameType(GameType gameType) {
	switch (gameType) {
		case GameType::Creative:
		case GameType::Survival:
			return gameType;
		default:
			return GameType::Default;
	}
}

Difficulty LevelSettings::validateGameDifficulty(Difficulty gameDifficulty) {
	switch (gameDifficulty) {
		case Difficulty::Easy:
		case Difficulty::Hard:
		case Difficulty::Normal:
		case Difficulty::Peaceful:
			return gameDifficulty;
		default:
			return Difficulty::Normal;
	}
}

std::string LevelSettings::gameTypeToString(GameType gameType) {
	if (gameType == GameType::Survival) {
		return "Survival";
	}
	if (gameType == GameType::Creative) {
		return "Creative";
	}
	return "Undefined";
}

std::string LevelSettings::generatorTypeToString(GeneratorType generatorType) {
	if (generatorType == GeneratorType::Legacy) {
		return "Legacy";
	}
	if (generatorType == GeneratorType::Overworld) {
		return "Overworld";
	}
	if (generatorType == GeneratorType::Flat) {
		return "Flat";
	}
	if (generatorType == GeneratorType::Nether) {
		return "Nether";
	}
	if (generatorType == GeneratorType::TheEnd) {
		return "TheEnd";
	}
	return "Undefined";
}

RandomSeed LevelSettings::parseSeedString(const std::string& seed, RandomSeed defaultSeed) {
	if (seed.size() >= 2) {
		std::string seedString = Util::stringTrim(seed);
		if (seedString.length() > 0) {
			RandomSeed tmpSeed;
			int value = sscanf(seedString.c_str(), "%d", &tmpSeed);
			// Try to read it as an integer
			if (value > 0 && (seedString == "-1" || tmpSeed != -1)) {
				return tmpSeed;
			}	// Hash the "seed"
			else {
				return Util::hashCode(seedString);
			}
		}
	}
	return defaultSeed;
}