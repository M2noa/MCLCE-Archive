/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/
#include "Dungeons.h"

#if !defined(DEMO_MODE)

#include "world/level/storage/ExternalFileLevelStorageSource.h"

#include "nbt/CompoundTag.h"
#include "platform/file.h"
#include "Core/Debug/Log.h"
#include "util/StringUtils.h"
// #include "world/level/LevelArchiver.h"
#include "world/level/storage/DBStorage.h"
#include "world/level/storage/ExternalFileLevelStorage.h"
#include "world/level/storage/LevelData.h"

using namespace Util;

static const std::string EXTERNAL_LEVEL_NAME = "External File Level Storage";

ExternalFileLevelStorageSource::ExternalFileLevelStorageSource(const std::string& externalPath)
	: mBasePath(externalPath) {
	File::createFolder(mBasePath);
}

void ExternalFileLevelStorageSource::addLevelSummaryIfExists(LevelSummaryList& dest, const std::string& directory){
 	LevelData levelData;

 	if (ExternalFileLevelStorage::readLevelDataFromFile(directory, levelData)) {
		
 		dest.emplace_back(
 			directory.substr(directory.find_last_of("/") + 1),
 			levelData.getLevelName(),
 			static_cast<int>(levelData.getLastPlayed()),
 			levelData.getGameType(),
 			levelData.getGameDifficulty(),
 			levelData.getSeed(),
 			levelData.getNetworkVersion(),
  			File::getSize(directory),
 			levelData.isMultiplayerGame(),
 			levelData.hasLANBroadcast(),
 			levelData.hasXBLBroadcast(),
 			levelData.hasCommandsEnabled(),
 			levelData.isEduLevel(),
 			levelData.getStorageVersion());
 	}
}

void ExternalFileLevelStorageSource::getLevelList(LevelSummaryList& dest){
 	using namespace File;
 
 	forEachIn(mBasePath, false, [&] (const std::string& path, const Stat& stat){
 		if (isDirectory(stat)) {			
// 			std::string importLock = Util::format("%s/%s", path.c_str(), LevelArchiver::IMPORT_LOCK_FILE.c_str());
			std::string importLock = Util::format("%s/%s", path.c_str(), "import_lock");
 			if (File::exists(importLock)) {
 				// This means we failed to complete the import.  Delete the world
 				File::deleteDirectory(path);
 			}
 			else {
 				addLevelSummaryIfExists(dest, path);
 			}
 		}
 	});
}

Unique<LevelStorage> ExternalFileLevelStorageSource::createLevelStorage(const std::string& levelId) {
	return make_unique<DBStorage>(levelId, _getFullPath(levelId));
}

void ExternalFileLevelStorageSource::deleteLevel( const std::string& levelId ){
	File::deleteDirectory( _getFullPath(levelId));	//this is now guaranteed to succeed in removing all children
}
 
std::string ExternalFileLevelStorageSource::_getFullPath(const std::string& levelID) const {
	return mBasePath + "/" + levelID;
}

bool ExternalFileLevelStorageSource::renameLevel( const std::string& levelId, const std::string& newLevelName ){
	// Rename the level name and write back to file
	LevelData levelData;
	if (ExternalFileLevelStorage::readLevelDataFromFile(_getFullPath(levelId), levelData)) {
		levelData.setLevelName(newLevelName);
		ExternalFileLevelStorage::saveLevelData(_getFullPath(levelId), levelData);

		ExternalFileLevelStorage::makeReadableLevelnameFile(_getFullPath(levelId) + "/levelname.txt", newLevelName);

		return true;
	}
	else{
		return false;
	}
}

void ExternalFileLevelStorageSource::copyLevelFromFilePath(const std::string& levelPath){
	File::deleteDirectory(mBasePath);
	File::createFolder(mBasePath);
	File::copyDirectory(levelPath, mBasePath);
}

bool ExternalFileLevelStorageSource::isConvertible(const std::string& levelId) {
	return false;
}

bool ExternalFileLevelStorageSource::requiresConversion(const std::string& levelId) {
	return false;
}

bool ExternalFileLevelStorageSource::convertLevel(const std::string& levelId, ProgressListener* progress) {
	return false;
}

const std::string& ExternalFileLevelStorageSource::getBasePath() const {
	return mBasePath;
}

const std::string& ExternalFileLevelStorageSource::getName() const {
	return EXTERNAL_LEVEL_NAME;
}

LevelData ExternalFileLevelStorageSource::getLevelData( const std::string& levelId ) {
	LevelData levelData;
	ExternalFileLevelStorage::readLevelDataFromFile(_getFullPath(levelId), levelData);

	return levelData;
}

void ExternalFileLevelStorageSource::setLevelData(const std::string& levelId, const LevelData& data) {
	ExternalFileLevelStorage::saveLevelData(_getFullPath(levelId), data);
}

bool ExternalFileLevelStorageSource::isNewLevelIdAcceptable( const std::string& levelId ) {
	return true;
}

void ExternalFileLevelStorageSource::clearAll() {

}

std::string ExternalFileLevelStorageSource::_getFullPath(const std::string& levelId) {
	return mBasePath + "/" + levelId;
}

#endif	/*DEMO_MODE*/
