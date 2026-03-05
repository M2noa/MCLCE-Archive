/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/
#include "Dungeons.h"

#include "world/level/storage/LevelSummary.h"

#include "SharedConstants.h"
#include "world/Difficulty.h"
#include "world/level/GameType.h"

LevelSummary::LevelSummary()
	: mId("invalid id")
	, mName("invalid name")
	, mLastPlayed(-1)
	, mGameType(GameType::Undefined)
	, mGameDifficulty(Difficulty::Count)
	, mSeed(-1)
	, mSizeOnDisk(-1)
	, mMultiplayerGame(false)
	, mLANBroadcast(false)
	, mXBLBroadcast(false)
	, mCommandsEnabled(false)
	, mIsEdu(false) {

}

LevelSummary::LevelSummary(const std::string& id, const std::string& name, int lastPlayed, GameType gameType, Difficulty gameDifficulty, int seed, int npv, uint64_t sizeOnDisk, bool multiplayerGame, bool lanBroadcast, bool xblBroadcast, bool commandsEnabled, bool isEdu, StorageVersion version)
	: mId(id)
	, mName(name)
	, mLastPlayed(lastPlayed)
	, mGameType(gameType)
	, mGameDifficulty(gameDifficulty)
	, mSeed(seed)
	, mNetworkProtocolVersion(npv)
	, mSizeOnDisk(sizeOnDisk)
	, mMultiplayerGame(multiplayerGame)
	, mLANBroadcast(lanBroadcast)
	, mXBLBroadcast(xblBroadcast)
	, mCommandsEnabled(commandsEnabled)
	, mIsEdu(isEdu)
	, mStorageVersion(version) {

}

bool LevelSummary::operator<(const LevelSummary& rhs) const {
	return mLastPlayed > rhs.mLastPlayed;
}

bool LevelSummary::isNetworkCompatible() const {
	// Fix for Bug 23391 - EDU Network Protocol Incompatibilities
	// EDU explicitly rev'd the network version extremely high to make the versions
	// incompatible.  We've moved away from that but we need to adjuct for it for now.
	// This code should be removed in 0.17.0 or later (as per User Story 24445)
#ifdef MCPE_EDU
	if (mNetworkProtocolVersion > 65536) {
		return (mNetworkProtocolVersion <= (SharedConstants::NetworkProtocolVersion | 65536));
	}
#endif

	return (mNetworkProtocolVersion <= SharedConstants::NetworkProtocolVersion);
}

bool LevelSummary::isEditionCompatible() const {
#ifdef MCPE_EDU
	// Allow EDU worlds to load vanilla worlds
	return true;
#else
	// Don't allow Vanilla to load edu worlds
	return mIsEdu == false;
#endif
}

bool LevelSummary::operator==(const LevelSummary& rhs) const {
	if (mId != rhs.mId) {
		return false;
	}
	if (mLastPlayed != rhs.mLastPlayed) {
		return false;
	}
	if (mGameType != rhs.mGameType) {
		return false;
	}
	if (mGameDifficulty != rhs.mGameDifficulty) {
		return false;
	}
	if (mSeed != rhs.mSeed) {
		return false;
	}
	if (mNetworkProtocolVersion != rhs.mNetworkProtocolVersion) {
		return false;
	}
	if (mName != rhs.mName) {
		return false;
	}
	if (mMultiplayerGame != rhs.mMultiplayerGame) {
		return false;
	}
	if (mLANBroadcast != rhs.mLANBroadcast) {
		return false;
	}
	if (mXBLBroadcast != rhs.mXBLBroadcast) {
		return false;
	}
	if (mCommandsEnabled != rhs.mCommandsEnabled) {
		return false;
	}
	if (mIsEdu != rhs.mIsEdu) {
		return false;
	}
	if (mStorageVersion != rhs.mStorageVersion) {
		return false;
	}

	return true;
}

bool LevelSummary::operator!=(const LevelSummary& rhs) const {
	return !operator==(rhs);
}
