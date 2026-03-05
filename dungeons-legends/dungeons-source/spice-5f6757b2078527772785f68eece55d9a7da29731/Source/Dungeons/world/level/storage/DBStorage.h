/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/
#pragma once

#include "common_header.h"
#include "util/KeySpan.h"
#include "world/level/storage/LevelStorage.h"
// #include "AppPlatformListener.h"

class BackgroundWorker;
class CompoundTag;
class KeyValueInput;
// class BackupProxyEnv;

namespace leveldb {
	class Slice;
	class DB;
	class FilterPolicy;
	class Cache;
	class Compressor;
	class WriteBatch;
	class Status;
}

class DBStorage :
	public LevelStorage
// 	, public AppPlatformListener 
{
public:

	friend class DBChunkStorage;

	typedef Unique<CompoundTag> TagPtr;

	///attempts repairing the world at path. The world needs to not be running!

	DBStorage(const std::string& levelID, const std::string& fullPath);

	virtual ~DBStorage();

	virtual LevelStorageResult getState() const override;

	virtual Unique<ChunkSource> createChunkStorage(Unique<ChunkSource> generator, StorageVersion v /* = SharedConstants::CurrentStorageVersion */) override;

	virtual const std::string& getFullPath() const override {
		return mFullPath;
	}
	
	TagPtr readTag(const std::string& key);

	bool hasKey(string_span key);

	void writeTag(const std::string& key, CompoundTag& tag);

	void asyncWriteTag(const std::string& key, CompoundTag& tag);

	bool hasAnyKeyWithPrefix(const leveldb::Slice& prefix) const;
	virtual bool loadLevelData(LevelData& data) override;

	virtual void saveLevelData(LevelData& levelData) override;

	virtual void savePlayerData(const std::string& key, std::string&& data) override;

	virtual void saveData(const std::string& key, std::string&& data) override;
	virtual std::string loadData(const std::string& key) override;

	virtual bool isCorrupted() const override;

	virtual Unique<CompoundTag> loadPlayerData(const std::string& clientUniqueName, const std::string& oldIdentifier = Util::EMPTY_STRING) override;
	virtual Unique<CompoundTag> loadLocalPlayerData() override;
	virtual void save(Player& player) override;
	virtual void save(Entity& entity) override;

	virtual std::vector<std::string> loadAllPlayerIDs() override;

	bool tryRepair(const std::string& fullPath) const;

	virtual std::string getLevelId() const override {
		return mLevelID;
	}

	virtual std::vector<std::string> pauseModificationsAndGetFiles() override;
	virtual void resumeModifications() override;

	virtual void compactStorage() override;
	virtual void suspendStorage() override;
	virtual void resumeStorage() override;
	virtual void onLowMemory() /*override*/;

protected:
	virtual void _savePlayerDataSync(const std::string& key, std::string&& data) override;

	class Options;

	// Unique<BackupProxyEnv> mBackupEnv;
	Unique<leveldb::Cache> mCache;
	Unique<const leveldb::FilterPolicy> mFilterPolicy;
	Unique<leveldb::Compressor> mCompressor;
	Unique<Options> mOptions;

	Unique<leveldb::DB> mDb;

	LevelStorageResult mState;

	std::string mLevelID;
	std::string mFullPath;
	std::string mBuffer;
	std::string mDbPath;
	bool mTearingDown = false;

	bool _read(string_span key, std::string& buffer ) const;
	bool _read(string_span prefix, KeyValueInput& input) const;
	bool _read(string_span key);

	void _write(const std::string& key, const std::string& data) const;
	void _write(leveldb::WriteBatch& batch) const;
	void _writeAsync(const std::string& key, std::string&& data) const;

	std::string _playerKey(const std::string& str);
	std::string _playerKey(const Player& p);

	Unique<CompoundTag> _legacyLoadClientPlayer(const std::string& uniqueName);

	Unique<CompoundTag> _legacyLoadPlayer(const std::string& clientUniqueName);
	bool _checkStatusValid(const leveldb::Status& status) const;

};
