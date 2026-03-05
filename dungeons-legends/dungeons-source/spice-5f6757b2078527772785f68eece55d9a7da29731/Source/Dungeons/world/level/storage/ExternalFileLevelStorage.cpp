/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/
#include "Dungeons.h"

#if !defined(DEMO_MODE)

#include "world/level/storage/ExternalFileLevelStorage.h"

#include "Core/Utility/Filepath.h"
#include "nbt/NbtIo.h"
#include "platform/file.h"
#include "util/RakDataIO.h"
//#include "world/entity/EntityFactory.h"
#include "world/entity/player/Player.h"
#include "world/level/Level.h"
#include "world/level/LevelConstants.h"
#include "world/level/block/LogBlock.h"
#include "world/level/block/entity/BlockEntity.h"
#include "world/level/chunk/LevelChunk.h"
#include "world/level/storage/LevelData.h"
#include "world/level/storage/LevelData.h"
#include "world/level/storage/LevelStorageSource.h"
#include "world/level/storage/RegionFile.h"

static const char* const fnLevelDatOld = "level.dat_old";
static const char* const fnLevelDatNew = "level.dat_new";
static const char* const fnLevelDat    = "level.dat";

namespace ExternalFileLevelStorage {
	// TODO: rherlitz

	bool _writeLevelDat(const std::string& datFilename, const LevelData& levelData) {
		ALOGI(LOG_AREA_LEVEL, "Writing down level seed as: %d\n", levelData.getSeed());

		// Try to load level.dat
		FILE* file = port::fopen_mb(datFilename.c_str(), _FILE_STR("wb"));

		if (!file) {
			return false;
		}

		//if (levelData.getStorageVersion() == 1) {
		RakNet::BitStream data;
		RakDataOutput buf(data);

		//LOGI("---> Trying to write level with version %d\n", version);

		auto tag = levelData.createTag();
		NbtIo::write(tag.get(), buf);

		//LOGI("<--- Finished writing level data. Size: %d\n", fdout.bytesWritten);

		StorageVersion version = levelData.getStorageVersion();	// 1
		fwrite(&version, sizeof(version), 1, file);
		int size = data.GetNumberOfBytesUsed();
		fwrite(&size, sizeof(size), 1, file);

		fwrite(data.GetData(), 1, size, file);
		fclose(file);

		return true;
	}

	void saveLevelData(const std::string& levelPath, const LevelData& levelData) {
		//std::string directory = levelPath + "/";
		//std::string tmpFile = directory + fnLevelDatNew;
		//std::string datFile = directory + fnLevelDat;
		//std::string oldFile = directory + fnLevelDatOld;

		//if (!_writeLevelDat(tmpFile, levelData)) {
		//	return;
		//}

		//// Remove old backup
		//remove(oldFile.c_str());

		//// If it exists, move the previous save to backup (and possibly delete it)
		//if (File::exists(datFile.c_str())) {
		//	if (!File::rename(datFile, oldFile)) {
		//		LOGE("Error@saveLevelData: Couldn't move savefile to level.dat_old\n");
		//		return;
		//	}
		//	remove(datFile.c_str());
		//}
		//// Move the new save to level.dat
		//if (!File::rename(tmpFile, datFile)) {
		//	LOGE("Error@saveLevelData: Couldn't move new file to level.dat\n");
		//	return;
		//}
		//// Remove the temporary save, if the rename didn't do it
		//remove(tmpFile.c_str());

		//auto levelnameFilePath = directory + "/levelname.txt";
		//if (!File::exists(levelnameFilePath)) {
		//	makeReadableLevelnameFile(levelnameFilePath, levelData.getLevelName());
		//}
	}

	bool readLevelDataFromData(const std::string& dataStr, LevelData& levelData) {
		std::stringstream dataStream(dataStr);

		StorageVersion version = StorageVersion::Unknown;
		int size = 0;

		dataStream.read((char*)&version, sizeof(version));
		if (!dataStream.good()) {
			return false;
		}

		dataStream.read((char*)&size, sizeof(size));
		if (!dataStream.good()) {
			return false;
		}

		std::vector<unsigned char> dataBlock(size, 0);
		dataStream.read(reinterpret_cast<char*>(dataBlock.data()), size);

		RakNet::BitStream bitStream(dataBlock.data(), dataBlock.size(), false);
 		if (version == StorageVersion::OldV1) {
 			levelData.v1_read(bitStream, version);
 		}
 		else if (version >= StorageVersion::OldV2) {
			RakDataInput stream(bitStream);

			if (auto tag = NbtIo::read(stream)) {
				levelData.getTagData(*tag);
			}
			else {
				return false;
			}
 		}

		return true;
	}
	
	bool readLevelDataFromFile(const std::string& directory, LevelData& levelData) {
		// Try to load level.dat
		std::string datFilename = directory + "/" + fnLevelDat;
		FILE* file = port::fopen_mb(datFilename.c_str(), _FILE_STR("rb"));

		// If that fails, try to load level.dat_old
		if (!file) {
			datFilename = directory + "/" + fnLevelDatOld;
			file = port::fopen_mb(datFilename.c_str(), _FILE_STR("rb"));
		}

		if (!file) {
			return false;
		}

		fseek(file, 0, SEEK_END);
		int fsize = static_cast<int>(ftell(file));
		rewind(file);

		std::vector<char> data(fsize, 0);
		if (fread(data.data(), 1, fsize, file) == fsize) {
			readLevelDataFromData(std::string(data.data(), data.size()), levelData);
		}

		fclose(file);

		return true;
	}

	void makeReadableLevelnameFile(const std::string& fullPath, const std::string& name) {
		std::ofstream file(port::toFilePath(fullPath));
		if (file) {
			file << name;
		}
	}

	std::vector<std::string> getImportantFiles() {
		std::vector<std::string> files;

		files.push_back(fnLevelDatOld);
		files.push_back(fnLevelDatNew);
		files.push_back(fnLevelDat);
		files.push_back("levelname.txt");

		return files;
	}

} // ExternalFileLevelStorage

#endif	/*DEMO_MODE*/
