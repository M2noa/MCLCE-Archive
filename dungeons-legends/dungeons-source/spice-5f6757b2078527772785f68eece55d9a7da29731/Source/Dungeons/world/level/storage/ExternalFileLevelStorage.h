/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/
#pragma once

#if !defined(DEMO_MODE)

#include "common_header.h"

#include "world/level/chunk/ChunkSource.h"
#include "world/level/storage/LevelStorage.h"
#include "world/level/storage/StorageVersion.h"

class Player;
class RegionFile;
class ChunkSource;

namespace ExternalFileLevelStorage
{
	bool readLevelDataFromFile(const std::string& directory, LevelData& dest);
	bool readLevelDataFromData(const std::string& data, LevelData& dest);
	void saveLevelData(const std::string& directory, const LevelData& levelData);
	void makeReadableLevelnameFile(const std::string& fullPath, const std::string& name);

	std::vector<std::string> getImportantFiles();
};

#endif
