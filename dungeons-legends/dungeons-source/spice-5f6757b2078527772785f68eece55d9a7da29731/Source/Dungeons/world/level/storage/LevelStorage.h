/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/
#pragma once

#include "common_header.h"
#include "CommonTypes.h"
#include "EntityUniqueID.h"
#include "LevelStorageState.h"
#include "SharedConstants.h"
#include "util/StringUtils.h"
#include "world/level/storage/StorageVersion.h"

class ChunkSource;
class LevelData;
class Player;
class Level;
class LevelChunk;
class CompoundTag;
class MapItemSavedData;

class LevelStorage {
public:

	virtual ~LevelStorage();

	virtual bool loadLevelData(LevelData& data ) = 0;

	virtual Unique<ChunkSource> createChunkStorage(Unique<ChunkSource> generator, StorageVersion v = SharedConstants::CurrentStorageVersion ) = 0;

	virtual void saveLevelData(LevelData& levelData) = 0;

	virtual const std::string& getFullPath() const = 0;

	virtual void savePlayerData(const std::string& key, std::string&& data) = 0;

	virtual void saveData(const std::string& key, std::string&& data) = 0;

	// Clones tag data to another key.  Returns true on success
	virtual bool clonePlayerData(const std::string& fromKey, const std::string& toKey);

	virtual bool isCorrupted() const = 0;

	virtual std::string loadData(const std::string& key);

	virtual LevelStorageResult getState() const = 0;

	static const std::string LOCAL_PLAYER_TAG;
	virtual Unique<CompoundTag> loadPlayerData(const std::string& clientUniqueName, const std::string& oldIdentifier = Util::EMPTY_STRING) = 0;
	virtual Unique<CompoundTag> loadLocalPlayerData() = 0;

	virtual std::vector<std::string> loadAllPlayerIDs() = 0;

	virtual void save(Player& player) = 0;
	virtual void save(Entity& entity) = 0;

	virtual std::string getLevelId() const = 0;

	virtual std::vector<std::string> pauseModificationsAndGetFiles() = 0;
	virtual void resumeModifications() = 0;

	virtual void compactStorage() = 0;
	virtual void suspendStorage() = 0;
	virtual void resumeStorage() = 0;

	Unique<CompoundTag> loadServerPlayerData(const Player& client);

	void saveData(const std::string& key, const CompoundTag& tag);
	void savePlayerData(const std::string& key, const CompoundTag& tag);

private:
	virtual void _savePlayerDataSync(const std::string& key, std::string&& data) = 0;
	
};
