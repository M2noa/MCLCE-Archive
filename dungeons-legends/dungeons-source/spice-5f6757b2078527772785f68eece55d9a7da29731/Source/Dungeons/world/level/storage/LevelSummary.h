/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/
#pragma once

#include "common_header.h"

#include "world/Difficulty.h"
#include "world/level/GameType.h"
#include "StorageVersion.h"

struct LevelSummary {
public:
	LevelSummary();
	LevelSummary(const std::string& id, const std::string& name, int lastPlayed, GameType gameType, Difficulty gameDifficulty, int seed, int npv, uint64_t sizeOnDisk, bool multiplayerGame, bool lanBroadcast, bool xblBroadcast, bool commandsEnabled, bool isEdu, StorageVersion version);
	LevelSummary(const LevelSummary&) = default;

	bool operator<(const LevelSummary& rhs) const;
	bool operator==(const LevelSummary& rhs) const;
	bool operator!=(const LevelSummary& rhs) const;

	bool isNetworkCompatible() const;
	bool isEditionCompatible() const;

	const std::string& getId() const {
		return mId;
	}

	void setId(std::string Id) {
		mId = Id;
	}

	uint64_t getSizeOnDisk() const {
		return mSizeOnDisk;
	}

	void setSizeOnDisk(uint64_t SizeOnDisk) {
		mSizeOnDisk = SizeOnDisk;
	}

	const std::string& getName() const {
		return mName;
	}

	void setName(std::string Name) {
		mName = Name;
	}

	GameType getGameType() const {
		return mGameType;
	}

	void setGameType(GameType gameType) {
		mGameType = gameType;
	}

	Difficulty getGameDifficulty() const {
		return mGameDifficulty;
	}

	void setGameDifficulty(Difficulty gameDifficulty) {
		mGameDifficulty = gameDifficulty;
	}

	int getSeed() const {
		return mSeed;
	}

	void setSeed(int Seed) {
		mSeed = Seed;
	}

	// No sense in a getter, use isNetworkCompatible
	void setNetworkProtocolVersion(int v) {
		mNetworkProtocolVersion = v;
	}

	int getLastPlayed() const {
		return mLastPlayed;
	}

	void setLastPlayed(int LastPlayed) {
		mLastPlayed = LastPlayed;
	}

	bool isMultiplayerGame() const {
		return mMultiplayerGame;
	}

	void setMultiplayerGame(bool multiplayerGame) {
		mMultiplayerGame = multiplayerGame;
	}

	bool hasLANBroadcast() const {
		return mLANBroadcast;
	}

	void setLANBroadcast(bool broadcast) {
		mLANBroadcast = broadcast;
	}

	bool hasXBLBroadcast() const {
		return mXBLBroadcast;
	}

	void setXBLBroadcast(bool broadcast) {
		mXBLBroadcast = broadcast;
	}

	bool hasCommandsEnabled() const {
		return mCommandsEnabled;
	}

	void setCommandsEnabled(bool commandsEnabled) {
		mCommandsEnabled = commandsEnabled;
	}

	bool isEdu() const {
		return mIsEdu;
	}

	void setEdu(bool isEdu) {
		mIsEdu = isEdu;
	}

	StorageVersion getStorageVersion() const {
		return mStorageVersion;
	}

private:

	std::string mId;
	std::string mName;
	int mLastPlayed;
	GameType mGameType;
	Difficulty mGameDifficulty;
	int mSeed;
	int mNetworkProtocolVersion;
	uint64_t mSizeOnDisk;
	bool mMultiplayerGame;
	bool mLANBroadcast;
	bool mXBLBroadcast;
	bool mCommandsEnabled;
	bool mIsEdu;
	StorageVersion mStorageVersion;
};

typedef std::vector<LevelSummary> LevelSummaryList;
