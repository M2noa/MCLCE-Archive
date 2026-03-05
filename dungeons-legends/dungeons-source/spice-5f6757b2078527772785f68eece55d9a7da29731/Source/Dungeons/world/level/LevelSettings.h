/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/

#pragma once

#include "CommonTypes.h"

#include "world/Difficulty.h"
//#include "network/NetworkBlockPosition.h"
#include "world/level/dimension/Dimension.h"
#include "world/level/GameType.h"
#include "world/level/GeneratorType.h"
#include "BlockPos.h"

class LevelData;
struct PackIdVersion;

class LevelSettings {

public:

	LevelSettings();

	LevelSettings(const LevelData& data, GameType gameType, DimensionId dimension);

	LevelSettings(RandomSeed seed, GameType gameType, Difficulty gameDifficulty,
				  DimensionId dimension, GeneratorType generator, const BlockPos& defaultSpawn,
				  bool achievementsDisabled, int dayCycleStopTime, bool isEduWorld,
				  float rainLevel, float lightningLevel, bool multiplayerGame,
				  bool lanBroadcast, bool xblBroadcast, bool commandsEnabled, bool texturePackRequired);

	const BlockPos& getDefaultSpawn() const;
	RandomSeed getSeed() const;
	GameType getGameType() const;
	Difficulty getGameDifficulty() const;
	int getDayCycleStopTime() const;
	void setForceGameType(bool value);
	const bool forceGameType() const;
	const bool hasAchievementsDisabled() const;
	const float getRainLevel() const;
	const float getLightningLevel() const;
	const GeneratorType getGenerator() const;
	const bool isEduWorld() const;
	void setIsEduWorld(bool isEdu);
	//
	// Those two should actually not be here
	// @todo: Move out when we add LevelSettings.cpp :p

	static GameType validateGameType(GameType gameType);
	static Difficulty validateGameDifficulty(Difficulty gameDifficulty);
	static std::string gameTypeToString(GameType gameType);
	static std::string generatorTypeToString(GeneratorType generatorType);
	static RandomSeed parseSeedString(const std::string& seed, RandomSeed defaultSeed);

	const DimensionId getDimension() const;

	bool isMultiplayerGame() const;
	bool hasLANBroadcast() const;
	bool hasXBLBroadcast() const;
	bool hasCommandsEnabled() const;
	bool isTexturepacksRequired() const;

	void setOverrideSavedSettings(bool overrideSaved);
	bool shouldOverrideSavedSettings() const;
	void overrideSavedSettings(LevelData& data) const;

	// if the level has been, or *will be*, loaded in creative or with cheats enabled
	bool achievementsWillBeDisabledOnLoad() const;

	void setNewWorldBehaviorPackIdentities(const std::vector<PackIdVersion>& identities);
	const std::vector<PackIdVersion>& getNewWorldBehaviorPackIdentities() const;

	void setNewWorldResourcePackIdentities(const std::vector<PackIdVersion>& identities);
	const std::vector<PackIdVersion>& getNewWorldResourcePackIdentities() const;

private:

	RandomSeed mSeed;
	GameType mGameType;
	Difficulty mGameDifficulty;
	bool mForceGameType;
	GeneratorType mGenerator;
	bool mAchievementsDisabled;
	DimensionId mDimension;
	int mDayCycleStopTime;
	bool mIsEduWorld;
	float mRainLevel;
	float mLightningLevel;

	bool mMultiplayerGame = true;
	bool mLANBroadcast = true;
	bool mXBLBroadcast = true;

	bool mCommandsEnabled;
	bool mTexturePacksRequired;
	bool mOverrideSettings;

	BlockPos mDefaultSpawn;
	std::vector<PackIdVersion> mNewWorldBehaviorPackIdentities;
	std::vector<PackIdVersion> mNewWorldResourcePackIdentities;
};


//specialization for serialization
template<>
struct serialize<LevelSettings>{
	void static write(const LevelSettings& val, BinaryStream& stream){
		stream.writeVarInt(val.getSeed());
		stream.writeVarInt(val.getDimension());
		stream.writeVarInt(enum_cast(val.getGenerator()));
		stream.writeVarInt(enum_cast(val.getGameType()));
		stream.writeVarInt(enum_cast(val.getGameDifficulty()));
		//stream.writeType(NetworkBlockPosition(val.getDefaultSpawn()));
		stream.writeBool(val.hasAchievementsDisabled());
		stream.writeVarInt(val.getDayCycleStopTime());
		stream.writeBool(val.isEduWorld());
		stream.writeFloat(val.getRainLevel());
		stream.writeFloat(val.getLightningLevel());
		stream.writeBool(val.hasCommandsEnabled());
		stream.writeBool(val.isTexturepacksRequired());
	}

	LevelSettings static read(ReadOnlyBinaryStream& stream){
		RandomSeed levelSeed;
		DimensionId dimensionId;
		GeneratorType levelGenerator;
		BlockPos defaultSpawn;
		GameType gameType;
		Difficulty gameDifficulty;
		bool loadedInCreative;
		bool isEduWorld;
		float rainLevel;
		float lightningLevel;
		int dayCycleStopTime;
		bool commandsEnabled;
		bool texturePackRequired;
		
		levelSeed = stream.getVarInt();
		dimensionId = static_cast<DimensionId>(stream.getVarInt());
		levelGenerator = static_cast<GeneratorType>(stream.getVarInt());
		gameType = static_cast<GameType>(stream.getVarInt());
		gameDifficulty = static_cast<Difficulty >(stream.getVarInt());
		stream.readType(defaultSpawn);
		loadedInCreative = stream.getBool();
		dayCycleStopTime = stream.getVarInt();
		isEduWorld = stream.getBool();
		rainLevel = stream.getFloat();
		lightningLevel = stream.getFloat();
		commandsEnabled = stream.getBool();
		texturePackRequired = stream.getBool();

		return LevelSettings(
			levelSeed,
			gameType,
			gameDifficulty,
			dimensionId,
			levelGenerator,
			defaultSpawn,
			loadedInCreative,
			dayCycleStopTime,
			isEduWorld,
			rainLevel,
			lightningLevel,
			true,
			true,
			true,
			commandsEnabled,
			texturePackRequired
		);
	}
};