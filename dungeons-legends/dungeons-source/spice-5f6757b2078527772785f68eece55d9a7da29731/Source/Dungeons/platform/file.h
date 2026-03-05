/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

#include "common_header.h"
#include "Core/Utility/Filepath.h"

namespace File {

#ifdef WIN32
	typedef struct _stati64 Stat;
#else
	typedef struct stat Stat;
#endif

	static_assert(sizeof(Stat().st_size) == 8, "We must handle big files");

	typedef std::function<void (const std::string& path, const Stat& stat)> Action;

	bool isDirectory(const Stat& stat);
	bool isDirectory(const std::string& path);

	bool deleteEmptyDirectory(const std::string& path);
	bool deleteDirectory(const std::string& path, bool noRecycleBin = true);
	bool deleteFile(const std::string& path);

	int forEachIn( const std::wstring& path, bool recursive, const Action& action );
	int forEachIn( const std::string& path, bool recursive, const Action& action );

	Stat getFileStat(const std::string& path);

	std::string cleanPath(const std::string& path);

	std::vector<std::string> splitPath(const std::string& path);

	std::string stringizePath(const std::vector<std::string>& pathElements);

	tm* getLastModifiedDateAsLocalTime(const Stat& stat);

	uint64_t getSize(const std::string& path);
	uint64_t getFileSize(const std::string& path);

	bool exists(const std::string& path);
	bool folderExists(const std::string& path);
	bool createFolder(const std::string& path);

	///copies files or entire directories
	bool copyFile(const std::string& path, const std::string& newPath);
	bool rename(const std::string& path, const std::string& newPath);
	bool copyDirectory(const std::string& path, const std::string& newPath);

	std::vector<std::string> getDirectoryFiles(const std::string& path);
	std::vector<std::string> getDirectoryFilesRecursive(const std::string& path);

	int getRemainingSize(FILE* fp);
	uint64_t getFileData(const std::string& path, uint64_t startByte, uint64_t bufferSize, std::vector<unsigned char>& buffer);
	bool createFile(const std::string& path);
	bool writeFileData(const std::string& path, uint64_t offset, uint64_t bufferSize, const std::vector<unsigned char>& buffer, const port::filepath_char* mode);

	std::string getUniqueFilePathForFile(const std::string& path);
	std::string getUniqueFilePathForDirectory(const std::string& path);

	std::string createUniquePathFromSeed(const std::string& basePath, std::function<std::string(int modifier)> seedGenerator);

// 	void getAvailableDiskFreeSpaceAsync(const std::string& path, std::function<void(uint64_t)> callback);
}
