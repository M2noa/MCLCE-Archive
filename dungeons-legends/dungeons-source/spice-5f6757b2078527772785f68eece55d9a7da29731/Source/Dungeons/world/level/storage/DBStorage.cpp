/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/
#include "Dungeons.h"

#include "world/level/storage/DBStorage.h"

#include <leveldb/cache.h>
#include <leveldb/db.h>
#include <leveldb/env.h>
#include <leveldb/filter_policy.h>
#include <leveldb/snappy_compressor.h>
#include <leveldb/write_batch.h>
#include <leveldb/zlib_compressor.h>
#include <raknet/BitStream.h>

#include "Core/Utility/Filepath.h"
#include "LevelStorageState.h"
#include "nbt/NbtIo.h"
// #include "platform/AppPlatform.h"
#include "platform/file.h"
// #include "platform/threading/BackgroundWorker.h"
// #include "platform/threading/WorkerPool.h"
#include "SharedConstants.h"
#include "util/DebugStats.h"
#include "util/KeySpan.h"
#include "util/KeyValueInput.h"
// #include "util/PerfTimer.h"
#include "util/RakDataIO.h"
#include "util/StringByteInput.h"
#include "util/StringByteOutput.h"
#include "util/StringUtils.h"
#include "world/entity/player/Player.h"
#include "world/level/storage/DBChunkStorage.h"
#include "world/level/storage/ExternalFileLevelStorage.h"
// #include "world/level/storage/LegacyChunkStorage.h"
#include "world/level/storage/LevelData.h"
// #include "BackupProxyEnv.h"
// #include "world/level/saveddata/maps/MapItemSavedData.h"

#include <leveldb/db.h>
#include "Core/Debug/Log.h"
#include "Core/Debug/DebugEventLogger.h"

string_span to_span(leveldb::Slice slice) {
	return{ slice.data(), static_cast<std::ptrdiff_t>(slice.size()) };
}

leveldb::Slice to_slice(string_span span) {
	return{ span.data(), static_cast<size_t>(span.size()) };
}

static const int MB = 1024 * 1024;

class DBStorage::Options {
public:
	leveldb::Options options;
	leveldb::ReadOptions read;
	leveldb::WriteOptions write;
};

LevelStorageState _convertError(const leveldb::Status& status) {
	switch (status.code()) {
	case leveldb::Status::kOk:
		return LevelStorageState::Open;
	case leveldb::Status::kNotFound:
		return LevelStorageState::NotFound;
	case leveldb::Status::kCorruption:
		return LevelStorageState::Corrupted;
	case leveldb::Status::kNotSupported:
		return LevelStorageState::NotSupported;
	case leveldb::Status::kInvalidArgument:
		return LevelStorageState::InvalidArguments;
	case leveldb::Status::kIOError:
		return LevelStorageState::IOError;
	default:
		return LevelStorageState::Unknown;
	}
}

size_t _writeBufferSize(int64_t memsize) {
	return ((memsize > 230 * 1024 * 1024) ? 4 : 1) * MB;
}

size_t _cacheSizeMb(int64_t memsize) {
	if (memsize > 490 * MB) {
		return 20;
	}
	else if (memsize > 230 * MB) {
		return 8;
	}
	else{
		return 1;
	}
}

Unique<leveldb::Cache> _createCache(int64_t memsize) {
	return Unique<leveldb::Cache>(leveldb::NewLRUCache(_cacheSizeMb(memsize) * MB));
}

bool DBStorage::tryRepair(const std::string& path) const {
	ALOGI(LOG_AREA_DATABASE, "World is corrupted! Trying a restore\r\n");

	if (!leveldb::RepairDB(path, mOptions->options).ok()) {
		ALOGE(LOG_AREA_DATABASE, "Well at least we tried\r\n");
		return false;
	}
	else {
		ALOGI(LOG_AREA_DATABASE, "Success!\r\n");
		return true;
	}
}

class NullLogger : public leveldb::Logger {
public:
	static NullLogger instance;
	void Logv(const char* format, va_list ap) override {
#ifndef PUBLISH
		char szBuf[1024];
		vsnprintf(szBuf, sizeof(szBuf) -1, format, ap);
		ALOGV(LOG_AREA_DATABASE, "%s\r\n", szBuf);
#else
		UNUSED_PARAMETER(format, ap);
#endif
	}
};

NullLogger NullLogger::instance;

DBStorage::DBStorage(const std::string& levelID, const std::string& fullPath)
// 	: AppPlatformListener(false)
	: mLevelID(levelID)
	, mFullPath(fullPath)
	, mState({ LevelStorageState::Open, Util::EMPTY_STRING })
	, mOptions(make_unique<Options>()) {

// 	initListener(0.5f);

	//create folders
	mDbPath = fullPath + "/db";

// 	// mBackupEnv = make_unique<BackupProxyEnv>(leveldb::Env::Default());

// 	ALOGI(LOG_AREA_DATABASE, "Trying to create folder %s\n", mDbPath.c_str());
	// here the DB storage wrapper is attempting to create folders and delete files, outside of the actual leveldb storage
	File::createFolder(mDbPath);
	//delete lock to be sure
	std::remove((mDbPath + "/LOCK").c_str());
	//delete log left over by old versions
	std::remove((mDbPath + "/LOG").c_str());

// 	auto memsize = AppPlatform::singleton().getAvailableMemory();
 	auto memsize = 1;

	//create the bloom filter
	mFilterPolicy = Unique<const leveldb::FilterPolicy>(leveldb::NewBloomFilterPolicy(10));

	//create the cache
	mCache = _createCache(memsize);

	mOptions->options.block_size = 160 * 1024;//average size of serialized chunks
	mOptions->options.create_if_missing = true;
	mOptions->options.info_log = &NullLogger::instance;
	mOptions->options.filter_policy = mFilterPolicy.get();
	mOptions->options.block_cache = mCache.get();
	mOptions->options.write_buffer_size = _writeBufferSize(memsize);
	// mOptions->options.env = mBackupEnv.get();

	mCompressor = make_unique<leveldb::ZlibCompressor>(-1);

	//compression quality from 1 to 9 (best), -1 default
	mOptions->options.compressors[0] = mCompressor.get();

	// Open database
	//google, why can't you C++ properly
	leveldb::DB* ptr = nullptr;
	leveldb::Status status = leveldb::DB::Open(mOptions->options, mDbPath, &ptr);

	//if the status is ok before or after repair, open it
	if (status.ok() || tryRepair(mDbPath)) {
		mDb = Unique<leveldb::DB>(ptr);
	}

	ALOGI(LOG_AREA_DATABASE, "LevelDB status: %s\n", status.ToString().c_str());
	mState.state = _convertError(status);
	mState.extra = status.ToString();
}

DBStorage::~DBStorage() {
	ALOGI(LOG_AREA_DATABASE, "Shutting down LevelDB\r\n");

	if (isCorrupted()) {
		tryRepair(mFullPath + "/db");
	}
}

LevelStorageResult DBStorage::getState() const {
	return mState;
}

bool DBStorage::loadLevelData(LevelData& data){
	return ExternalFileLevelStorage::readLevelDataFromFile(mFullPath, data);
}

void DBStorage::saveLevelData(LevelData& levelData){
	ExternalFileLevelStorage::saveLevelData(mFullPath, levelData);
}

void DBStorage::_savePlayerDataSync(const std::string& key, std::string&& data) {
	_write(_playerKey(key), std::move(data));
}

bool DBStorage::_read(string_span prefix, KeyValueInput& input) const {
	if (mDb.get() == nullptr) {
		return false;
	}

	leveldb::Iterator* it = mDb->NewIterator(mOptions->read);
	auto dbPrefix = to_slice(prefix);

	for (it->Seek(dbPrefix); it->Valid(); it->Next()) {

		if ( !it->key().starts_with(dbPrefix)) {
			break;
		}

		auto key = to_span(it->key()).subspan(prefix.size());

		input.add(gsl::to_string(key), it->value().data(), it->value().size());
	}
	delete it;

	return !input.empty();
}

bool DBStorage::_read(string_span key, std::string& buffer) const {
	return mDb->Get(mOptions->read, to_slice(key), &buffer).ok() && buffer.size() > 0;
}

bool DBStorage::_read(string_span key){
	DEBUG_ASSERT_MAIN_THREAD;	//TODO make buffer thread local? is TL even faster than allocating?
	mBuffer.clear();
	return _read(key, mBuffer);
}

std::string DBStorage::loadData(const std::string& key) {
	if (_read(key)) {
		return mBuffer;
	}
	else{
		return Util::EMPTY_STRING;
	}
}

bool DBStorage::isCorrupted() const {
	return mState.state == LevelStorageState::Corrupted;
}

bool DBStorage::hasKey(string_span key) {
	auto dbKey = to_slice(key);

	auto itr = Unique<leveldb::Iterator>(mDb->NewIterator(mOptions->read));
	itr->Seek(dbKey);

	return itr->Valid() && itr->key() == dbKey;
}

bool DBStorage::_checkStatusValid(const leveldb::Status& status) const {
	if (!status.ok()) {
		LOGE("Write failed, all is lost!\n");
		LOGE("%s\n", status.ToString().c_str());

		//screw sanity, we're corrupted
		auto& self = *(DBStorage*)this;

		self.mState.state = LevelStorageState::Corrupted;
		self.mState.extra = status.ToString();

		return false;
	}
	return true;
}

void DBStorage::_write(leveldb::WriteBatch& batch) const {
	if (_checkStatusValid(mDb->Write(mOptions->write, &batch))) {
#ifdef DEBUG_STATS
		static int info = 0;
		if (++info % 10 == 0) {
			ALOGI(LOG_AREA_DATABASE, "	LevelDB: average compressed size is %f%%\r\n", mCompressor->getAverageCompression() * 100.);
		}
#endif
	}
}

void DBStorage::_write(const std::string& key, const std::string& data) const {
	_checkStatusValid(mDb->Put(mOptions->write, key, data));
}

bool DBStorage::hasAnyKeyWithPrefix(const leveldb::Slice& prefix) const {
	auto it = std::unique_ptr<leveldb::Iterator>(mDb->NewIterator(mOptions->read));

	it->Seek(prefix);
	return it->Valid() && it->key().starts_with(prefix);
}

void DBStorage::_writeAsync(const std::string& key, std::string&& data) const {
	//TODO when using C++14, just capture the string by move
// 	auto dataPtr = make_shared<std::string>(std::move(data));
// 	ALOGV(LOG_AREA_DATABASE, "writeAsync key %s sz %d\r\n", key.c_str(), dataPtr->size());
// 	WorkerPool::getFor(WorkerRole::Disk).queue(
// 		[this, key, dataPtr] () {
// 		ScopedProfile("_writeAsync");
// 
// 		if (mTearingDown) {
// 			return true;
// 		}
// 
// 		// Keep trying until our DB is back
// 		if (mDb.get() == nullptr) {
// 			return false;
// 		}
// 
// 		_write(key, *dataPtr);
// 		return true;
// 	});
}

void DBStorage::savePlayerData(const std::string& key, std::string&& data) {
	_writeAsync(_playerKey(key), std::move(data));
}

void DBStorage::saveData(const std::string& key, std::string&& data) {
	_writeAsync(key, std::move(data));
}

void DBStorage::writeTag(const std::string& key, CompoundTag& tag){
	mBuffer.clear();
	StringByteOutput out(mBuffer);

	NbtIo::write(&tag, out);

	_write(key, mBuffer);
}

DBStorage::TagPtr DBStorage::readTag(const std::string& key) {
	DEBUG_ASSERT_MAIN_THREAD;
	if (_read(key)) {

		RakNet::BitStream stream((unsigned char*)mBuffer.data(), mBuffer.size(), false);
		RakDataInput input(stream);

		return NbtIo::read(input);
	}
	else{
		return nullptr;
	}
}

void DBStorage::asyncWriteTag(const std::string& key, CompoundTag& tag) {
	std::string data;

	StringByteOutput out(data);

	NbtIo::write(&tag, out);
	_writeAsync(key, std::move(data));
}

Unique<ChunkSource> DBStorage::createChunkStorage(Unique<ChunkSource> generator, StorageVersion v /* =  SharedConstants::CurrentStorageVersion */) {
	auto storage = make_unique<DBChunkStorage>(std::move(generator), this);

	//wrap a read-only compatibility source for chunks that were never saved in the new format
 	if ( v < StorageVersion::LevelDB1 ) {
		// TODO: rherlitz fix
 		//return make_unique<LegacyChunkStorage>(std::move(storage), *this, v);
		return std::move(storage);
	}
 	else{
		return std::move(storage);
 	}
}

std::string getPlayerFilename(const std::string& fullPath, const std::string& fn) {
	std::string lowername = fn;
	std::transform(lowername.begin(), lowername.end(), lowername.begin(), ::tolower);
	return fullPath + "/players/" + lowername + ".dat";
}

Unique<CompoundTag> DBStorage::_legacyLoadClientPlayer(const std::string& uniqueName) {

// 	std::string directory = mFullPath + "/players/";
// 	std::string filename = getPlayerFilename(mFullPath, uniqueName);
// 
// 	FILE* fp = port::fopen_mb(filename.c_str(), _FILE_STR("rb"));
// 	if (!fp) {
// 		return nullptr;
// 	}
// 
// 	char header[5];
// 	int version, numBytes;
// 	fread(header, 1, 4, fp);
// 	fread(&version, sizeof(int), 1, fp);
// 	fread(&numBytes, sizeof(int), 1, fp);
// 
// 	//bool success = false;
// 	int left = File::getRemainingSize(fp);
// 	if (numBytes <= left && numBytes > 0) {
// 		std::string buf(numBytes, 0);
// 
// 		fread((char*)buf.data(), 1, numBytes, fp);
// 		fclose(fp);
// 
// 		{
// 			//throw away the old file and the directory if empty
// 			std::remove(filename.c_str());
// 			File::deleteEmptyDirectory(directory);
// 		}
// 
// 		StringByteInput dis(buf);
// 		return NbtIo::read(dis);
// 	}
// 
// 	fclose(fp);
	return nullptr;
}

Unique<CompoundTag> DBStorage::_legacyLoadPlayer(const std::string& clientUniqueName) {
	Unique<CompoundTag> playerTag;
	//try legacy
	LevelData data;
	if (clientUniqueName.empty()) {
		if (loadLevelData(data) && !data.getLoadedPlayerTag().isEmpty()) {
			playerTag = data.getLoadedPlayerTag().clone();
		}
	}
	else {	//rummage in the players.dat folder
		playerTag = _legacyLoadClientPlayer(clientUniqueName);
	}

	if (playerTag) {
		//save it right now
		asyncWriteTag(_playerKey(clientUniqueName), *playerTag);
	}

	return playerTag;
}

std::string DBStorage::_playerKey(const std::string& uniqueName) {
	return (uniqueName.empty() || uniqueName == LOCAL_PLAYER_TAG) ? LOCAL_PLAYER_TAG : "player_" + uniqueName;
}

std::string DBStorage::_playerKey(const Player& p) {
// 	return _playerKey(p.isLocalPlayer() ? LOCAL_PLAYER_TAG : p.mUniqueName);
	return "";
}

Unique<CompoundTag> DBStorage::loadPlayerData(const std::string& clientUniqueName, const std::string& oldIdentifier) {
	DEBUG_ASSERT(!clientUniqueName.empty(), "Cannot loadPlayerData with an empty string for a clientUniqueName.");

	auto playerTag = readTag(_playerKey(clientUniqueName));

	if (!playerTag) {
		playerTag = readTag(_playerKey(oldIdentifier));
	}

	if (!playerTag) {
		playerTag = _legacyLoadPlayer(clientUniqueName);
	}

	return playerTag;
}

Unique<CompoundTag> DBStorage::loadLocalPlayerData() {
	auto playerTag = readTag(LOCAL_PLAYER_TAG);

	if (!playerTag) {
		playerTag = _legacyLoadPlayer(Util::EMPTY_STRING);
	}

	return playerTag;
}

void DBStorage::save(Player& player) {
// 	if (!player.isRemoved()) {
// 		CompoundTag playerTag;
// 		player.save(playerTag);
// 
// 		asyncWriteTag(_playerKey(player), playerTag);
// 	}
}

void DBStorage::save(Entity& e) {
// 	if (!e.isRemoved()) {
// 		CompoundTag entityTag;
// 		e.save(entityTag);
// 
// 		asyncWriteTag(EntityTypeToString(e.getEntityTypeId()), entityTag);
// 	}
}

std::vector<std::string> DBStorage::loadAllPlayerIDs() {

	KeyValueInput players;
	std::vector<std::string> names = {
	};

	// Only add local player tag if it exists in the file
	if (hasKey(LOCAL_PLAYER_TAG)) {
		names.push_back(LOCAL_PLAYER_TAG);
	}

	_read("player_", players);

	for (const auto p : players) {
		names.emplace_back(std::move(p.key.name));
	}

	return names;
}

std::vector<std::string> DBStorage::pauseModificationsAndGetFiles() {
	// auto dbFiles = mBackupEnv->pauseModificationsAndGetFiles(mFullPath + "/db");
	// if (dbFiles.empty()) {
	// 	return {};
	// } else {
	// 	for (auto it = dbFiles.begin(); it != dbFiles.end();) {
	// 		auto& file = *it;
	// 		file = "db/" + file;
	// 		//directories shouldn't be seen as files
	// 		if (File::isDirectory(File::getFileStat(mFullPath + "/" + file))) {
	// 			it = dbFiles.erase(it);
	// 		}
	// 		else {
	// 			++it;
	// 		}
	// 	}
	// 	auto filesToAdd = ExternalFileLevelStorage::getImportantFiles();
	// 	for (auto& fileToAdd : filesToAdd) {
	// 		if (File::exists(mFullPath + "/" + fileToAdd)) {
	// 			dbFiles.push_back(fileToAdd);
	// 		}
	// 	}

	// 	for (auto& file : dbFiles) {
	// 		file = mLevelID + "/" + file;
	// 	}
	// 	return dbFiles;
	// }

	return {};
}

void DBStorage::resumeModifications() {
	// mBackupEnv->resumeModifications();
}

void DBStorage::compactStorage() {
	mDb->CompactRange(nullptr, nullptr);
}

void DBStorage::suspendStorage() {
	// tell the storage system not to schedule any background tasks
// 	mDb->SuspendCompaction();
}

void DBStorage::resumeStorage() {
// 	mDb->ResumeCompaction();
}

void DBStorage::onLowMemory() {
	if (mDb) {
		mCache->Prune();
	}
} 

