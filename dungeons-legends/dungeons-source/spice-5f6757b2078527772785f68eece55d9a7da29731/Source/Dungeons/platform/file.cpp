/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/

#include "Dungeons.h"

#include "Core/Debug/Log.h"
#include "Core/Utility/Filepath.h"
#include "Core/Utility/utf8proc.h"

// #include "platform/AppPlatform.h"
#include "platform/file.h"
// #include "platform/threading/WorkerPool.h"
// #include "platform/ZipUtils.h"

#include "util/StringUtils.h"

#include <sys/stat.h>

#if defined(ANDROID)
#include "android/AppPlatform_android23.h"
#endif

union FileSizeAndPos {
	uint64_t mUint64;
	struct {
		int32_t mHigh;
		int32_t mLow;
	} mInt32;
};

#ifdef WIN32
#include <sys/types.h>

namespace fs = std::tr2::sys;

int File::forEachIn(const std::string& path, bool recursive, const Action& action) {
	auto cPath = cleanPath(path);
	const auto widePath = Util::toWideString(cPath);
	return forEachIn(widePath, recursive, action);
}


File::Stat File::getFileStat(const std::string& path) {
	auto cPath = cleanPath(path);
	Stat info;
	const auto widePath = Util::toWideString(cPath);
	auto err = _wstati64(widePath.c_str(), &info);
	// Don't assert for missing file, that's ok
	if (err != 0) {
		if (errno != ENOENT) {
			DEBUG_FAIL("Can't read file");
		}

		info = {};
	}
	return info;
}

int File::forEachIn(const std::wstring& path, bool recursive, const Action& action) {
	WIN32_FIND_DATAW fileData;
	HANDLE hFind;
	Stat info;
	int err = 0;

	hFind = FindFirstFileExW((path + L"/*").c_str(),
		FINDEX_INFO_LEVELS::FindExInfoStandard, &fileData,
		FINDEX_SEARCH_OPS::FindExSearchNameMatch, nullptr, 0);
	if (hFind != INVALID_HANDLE_VALUE) {
		do {
			if (wcscmp(fileData.cFileName, L".") == 0 || wcscmp(fileData.cFileName, L"..") == 0) {
				continue;
			}

			const std::wstring fullPath = path + L"/" + fileData.cFileName;

			if (recursive && fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
				forEachIn(fullPath, recursive, action);	//descend

			}
			err = _wstati64(fullPath.c_str(), &info);
			DEBUG_ASSERT(err == 0, "Can't read file");
			if (err == 0) {
				const std::string _fullPath = Util::toString(fullPath);
				action(_fullPath, info);
			}

		} while (FindNextFileW(hFind, &fileData));

		FindClose(hFind);
		return 0;
	}
	else {
		return 1;	// can't open
	}
}

bool File::deleteEmptyDirectory(const std::string& path) {
	auto cPath = cleanPath(path);
	fs::path encodedPath = fs::u8path(cPath);

	return fs::remove(encodedPath);
}

int makeDirectory(const std::string& path) {
	fs::path encodedPath = fs::u8path(path);

	return fs::create_directory(encodedPath) == true ? 0 : -1;
}

bool File::exists(const std::string& path) {
	auto cPath = cleanPath(path);
	fs::path encodedPath = fs::u8path(cPath);

	return fs::exists(encodedPath);
}

bool File::folderExists(const std::string& path) {
	auto cleanPath = File::cleanPath(path);
	fs::path encodedPath = fs::u8path(cleanPath);
	return fs::is_directory(encodedPath);
}

bool File::rename(const std::string& path, const std::string& newPath) {
	auto cPath = cleanPath(path);
	auto cNewPath = cleanPath(newPath);

	ALOGV(LOG_AREA_FILE, "File::rename WIN32\n");
	std::error_code error;
	fs::rename(fs::u8path(cPath), fs::u8path(cNewPath), error);
	ALOGV(LOG_AREA_FILE, "File::rename returned %d\n", error.value());
	return (error.value() == 0);
}

bool File::copyFile(const std::string& path, const std::string& newPath) {
	auto cPath = cleanPath(path);
	auto cNewPath = cleanPath(newPath);
	std::ifstream src(fs::u8path(cPath), std::ios::binary);
	std::ofstream dst(fs::u8path(cNewPath), std::ios::binary);

	if (src.fail() || dst.fail()) {
		return false;
	}

	dst << src.rdbuf();
	return true;
}

bool File::deleteFile(const std::string& path) {
	auto cPath = cleanPath(path);

	ALOGV(LOG_AREA_FILE, "File::deleteFile WIN32\n");
	std::error_code error;
	fs::remove(fs::u8path(cPath), error);
	ALOGV(LOG_AREA_FILE, "File::deleteFile returned %d\n", error.value());
	return (error.value() == 0);
}

#else //WIN32

int makeDirectory(const std::string& path) {
	return mkdir(path.c_str(), 0755);
}

int _access(const std::string& path, int mode) {
	return access(path.c_str(), mode);
}

int _errno() {
	return errno;
}

File::Stat File::getFileStat(const std::string& path) {
	auto cPath = cleanPath(path);
	Stat info;
	auto err = stat(cPath.c_str(), &info);
	// Don't assert for missing file, that's ok
	if (err != 0) {
		if (_errno() != ENOENT) {
			DEBUG_FAIL("Can't read file");
		}

		info = { };
	}
	return info;
}

int File::forEachIn(const std::string& path, bool recursive, const Action& action) {
	auto cPath = cleanPath(path);

	DIR* directory;
	struct dirent* dir;
	int err;
	(void)err;  //Android refuses to compile with UNUSED_PARAMETER...

	directory = opendir(cPath.c_str());

#if defined(ANDROID)
	if (!directory) {
		auto assetManager = AppPlatform_android23::getAssetManager();
		if (assetManager != nullptr) {
			AAssetDir* assetDir = AAssetManager_openDir(assetManager, cPath.c_str());
			const char* filename = nullptr;

			std::string pathRoot = path;
			if (path.empty() || (path.back() != '/' && path.back() != '\\')) {
				pathRoot += '/';
			}

			while ((filename = AAssetDir_getNextFileName(assetDir)) != nullptr) {
				Stat info;
				err = stat(filename, &info);

				action(pathRoot + filename, info);
			}
			AAssetDir_close(assetDir);
		}
	}
#endif
	if (directory) {
		while ((dir = readdir(directory)) != nullptr) {

			Stat info;
			int err;

			if (strcmp(dir->d_name, ".") == 0 || strcmp(dir->d_name, "..") == 0) {
				continue;
			}

			std::string fullPath = cPath + "/" + dir->d_name;

			err = stat(fullPath.c_str(), &info);
			DEBUG_ASSERT(err == 0, "stat failed");

			//recursively descend in directories?
			if (recursive && isDirectory(info)) {
				forEachIn(fullPath, recursive, action);
			}

			//execute an action for every element
			action(fullPath, info);
		}

		closedir(directory);

		return 0;
	}
	else {
		return 1;	//error, can't open folder
	}
}

bool File::deleteEmptyDirectory(const std::string& path) {
	auto cPath = cleanPath(path);
	return rmdir(cPath.c_str()) == 0;
}

bool File::exists(const std::string& path) {
	auto cPath = cleanPath(path);
	bool result = _access(cPath.c_str(), 0) == 0;

#if defined(ANDROID)
	// access() won't work for files inside of Android's APK. Attempt to find it through
	// the AssetManager here.
	if (!result) {
		auto assetManager = AppPlatform_android23::getAssetManager();
		if (assetManager != nullptr) {
			AAsset* asset = AAssetManager_open(assetManager, cPath.c_str(), AASSET_MODE_BUFFER);
			if (asset != nullptr) {
				result = true;
				AAsset_close(asset);
			}
		}
	}
#endif

	return result;
}


bool File::folderExists(const std::string& path) {
	auto cPath = cleanPath(path);
	bool result = _access(cPath.c_str(), 0) == 0;

#if defined(ANDROID)
	if (!result) {
		auto assetManager = AppPlatform_android23::getAssetManager();
		if (assetManager != nullptr) {
			AAssetDir* assetDir = AAssetManager_openDir(assetManager, cPath.c_str());
			// AAssetManager_openDir will always return a valid pointer, even if the folder doesn't exist.
			// Best hack is to look if there are any files in the folder.
			// However, this will return a false negative for an empty directory
			if (assetDir) {
				result = AAssetDir_getNextFileName(assetDir) != nullptr;
				AAssetDir_close(assetDir);
			}
		}
	}
#endif

	return result;
}

bool File::rename(const std::string& path, const std::string& newPath) {
	auto cPath = cleanPath(path);
	auto cNewPath = cleanPath(newPath);

	int err = std::rename(cPath.c_str(), cNewPath.c_str());

	return (err == 0);
}

bool File::copyFile(const std::string& path, const std::string& newPath) {
	auto cPath = cleanPath(path);
	auto cNewPath = cleanPath(newPath);

	std::ifstream src(cPath.c_str(), std::ios::binary);
	std::ofstream dst(cNewPath.c_str(), std::ios::binary);

	if (src.fail() || dst.fail()) {
		return false;
	}

	dst << src.rdbuf();

	return true;
}

bool File::deleteFile(const std::string& path) {
	auto cPath = cleanPath(path);

	int err = std::remove(cPath.c_str());

	return (err == 0);
}
#endif



tm* File::getLastModifiedDateAsLocalTime(const Stat& stat) {
	tm* localTime;
	// windows created this _localtime64 while other platforms just typedef the input to 
	// a 64 bit value instead.  I put the ifdef here instead of putting the function
	// inside the other, larger ifdef blocks because it would be easy to get them out of
	// sync if they were several hundred lines of code apart
#ifdef WIN32
	localTime = _localtime64(&stat.st_mtime);
#else //WIN32
	time_t t1 = static_cast<time_t>(stat.st_mtime);
	localTime = localtime(&t1);
#endif

	return localTime;
}

bool File::deleteDirectory(const std::string& path, bool noRecycleBin /*true*/) {
	UNUSED_PARAMETER(noRecycleBin);
	auto cPath = cleanPath(path);
	ALOGI(LOG_AREA_FILE, "Deleting %s and all its subfolders\n", cPath.c_str());

	bool deleteFailed = false;
	int err = forEachIn(cPath, true, [&deleteFailed](const std::string& path, const Stat& info) {
		ALOGI(LOG_AREA_FILE, "Deleting: %s\n", path.c_str());
		if (isDirectory(info)) {
			//for each file and directory, execute this
			if (!deleteEmptyDirectory(path)) {
				DEBUG_FAIL(Util::format("Couldn't delete file %s", path.c_str()).c_str());
				deleteFailed = true;
			}
		}
		else {
			if (!deleteFile(path)) {
				DEBUG_FAIL(Util::format("Couldn't delete file %s", path.c_str()).c_str());
				deleteFailed = true;
			}
		}
	});

	if (err != 0) {
		deleteFailed = true;
	}
	//finally, remove the containing dir
	else if (!deleteEmptyDirectory(cPath)) {
		DEBUG_FAIL(Util::format("Couldn't delete empty folder %s", cPath.c_str()).c_str());
		deleteFailed = true;
	}

	return !deleteFailed;
}

bool File::isDirectory(const Stat& s) {
	return (s.st_mode & S_IFDIR) == S_IFDIR;
}

bool File::isDirectory(const std::string& path) {
	// Clean the path to remove the potential trailing slash. 
	auto cPath = cleanPath(path);
	auto fileStat = getFileStat(cPath);
	return isDirectory(fileStat);
}

uint64_t File::getSize(const std::string& path) {
	auto cPath = cleanPath(path);
	uint64_t fullSize = 0;
	if (isDirectory(cPath)) {
		int err = forEachIn(cPath, true, [&fullSize](const std::string& path, const Stat& info) {
			UNUSED_PARAMETER(path);
			if (!isDirectory(info)) {
				fullSize += info.st_size;
			}
		});

		if (err != 0) {
			return -1;
		}
	}
	else {
		fullSize = getFileSize(cPath);
	}
	return fullSize;
}

uint64_t File::getFileSize(const std::string& path) {
	auto cPath = cleanPath(path);
	FILE* fp = port::fopen_mb(cPath.c_str(), _FILE_STR("rb"));
	if (!fp) {
		return -1;
	}

	fseek(fp, 0, SEEK_END);
	const uint64_t size = ftell(fp);
	fclose(fp);
	return size;
}

bool _createOneFolder(const std::string& path) {
	DEBUG_ASSERT(!path.empty(), "Name is empty!");
	if (File::exists(path)) {
		return true;
	}

	int errorCode = 0;
	if ((errorCode = makeDirectory(path)) != 0) {
		LOGE("FAILED to create folder %s, %d! Escape plan?\n", path.c_str(), errorCode);
		return false;
	}
	return true;
}

bool File::createFolder(const std::string& path) {
	auto cPath = cleanPath(path);
	auto segments = splitPath(cPath);

	// Walk backwards through the list until we find something existing (or find nothing at all), then walk forwards
	// creating the missing parts
	//   This approach avoids attempting to query for existence of root paths (such as C:\Users" on Windows) which we
	// may not have access to query.
	auto it = segments.end();

	while (it != segments.begin()) {
		it--;

		const bool fileExists = exists(*it);
		if (fileExists || it == segments.begin()) {
			// found something that exists, or nothing exists, create the rest
			if (fileExists) {
				it++;
			}

			while (it != segments.end()) {
				if (!_createOneFolder(*it)) {
					return false;
				}
				it++;
			}
			break;
		}
	}

	return true;
}

int File::getRemainingSize(FILE* fp) {
	if (!fp) {
		return 0;
	}
	const int current = ftell(fp);
	fseek(fp, 0, SEEK_END);
	const int end = ftell(fp);
	fseek(fp, current, SEEK_SET);
	return end - current;
}

std::string File::cleanPath(const std::string& path) {
	std::string newPath;
	// new path will be at most path.size characters
	newPath.reserve(path.size());

	bool previousWasSlash = false;

	const uint8_t* strPtr = reinterpret_cast<const uint8_t*>(path.c_str());
	int c;
	ssize_t remaining = path.length();
	ssize_t skip;

	while ((skip = utf8proc_iterate(strPtr, remaining, &c)) > 0) {
		strPtr += skip;
		remaining -= skip;

		if (previousWasSlash) {
			if (!(c == '\\' || c == '/')) {
				//add one fixed slash
				newPath.push_back('/');
				Util::appendUtf8Character(newPath, c);
				previousWasSlash = false;
			}
		}
		else {
			if (c == '\\' || c == '/') {
				previousWasSlash = true;
			}
			else {
				Util::appendUtf8Character(newPath, c);
			}
		}
	}
	return newPath;
}

std::vector<std::string> File::splitPath(const std::string& path) {
	// clean path
	auto cPath = cleanPath(path);

	std::vector<std::string> segments = {
		""
	};

	const uint8_t* strPtr = reinterpret_cast<const uint8_t*>(cPath.c_str());
	int c;
	ssize_t remaining = cPath.length();
	ssize_t skip;

	while ((skip = utf8proc_iterate(strPtr, remaining, &c)) > 0) {
		strPtr += skip;
		remaining -= skip;

		if (c == '/' && !segments.back().empty()) {
			segments.emplace_back();//make a copy that will now be grown
			segments.back() = segments[segments.size() - 2];
		}

		Util::appendUtf8Character(segments.back(), c);
	}

	return segments;
}

std::string File::stringizePath(const std::vector<std::string>& pathElements) {
	std::stringstream result;
	for (auto& element : pathElements) {
		result << '/' << element;
	}
	return File::cleanPath(result.str());
}

bool File::copyDirectory(const std::string& fromPath, const std::string& toPath) {
	//warning: empty dirs aren't copied
	auto cleanFromPath = cleanPath(fromPath);
	auto cleanToPath = cleanPath(toPath);

	bool copyFailed = false;
	int err = forEachIn(cleanFromPath, true, [&](const std::string& from, const Stat& info) {

		auto to = cleanToPath + from.substr(cleanFromPath.length(), cleanFromPath.length() - from.length());

		if (!isDirectory(info)) {
			//extract parent folder and create it
			auto subDir = Util::getDirectory(to);
			if (createFolder(subDir)) {
				//finally, copy the file
				if (!copyFile(from, to)) {
					DEBUG_FAIL(Util::format("Couldn't copy file %s to %s", from.c_str(), to.c_str()).c_str());
					copyFailed = true;
				}
			}
			else {
				DEBUG_FAIL(Util::format("Couldn't create folder %s for copy", subDir.c_str()).c_str());
				copyFailed = true;
			}
		}

	});

	if (err != 0) {
		copyFailed = true;
	}

	return !copyFailed;
}

std::vector<std::string> File::getDirectoryFiles(const std::string& path) {
	auto cPath = cleanPath(path);
	std::vector<std::string> files;
	int err = forEachIn(cPath, false, [&](const std::string& from, const Stat& info) {
		if (!isDirectory(info)) {
			files.push_back(from);
		}

	});
	if (err != 0) {
		DEBUG_FAIL(Util::format("Couldn't get files from %s", cPath.c_str()).c_str());
		files.clear();
	}
	return files;
}

std::vector<std::string> File::getDirectoryFilesRecursive(const std::string& path) {
	auto cPath = cleanPath(path);
	std::vector<std::string> files;
	int err = forEachIn(cPath, true, [&](const std::string& from, const Stat& info) {
		if (!isDirectory(info)) {
			files.push_back(from);
		}

	});
	if (err != 0) {
		DEBUG_FAIL(Util::format("Couldn't get files from %s", cPath.c_str()).c_str());
		files.clear();
	}
	return files;
}

uint64_t File::getFileData(const std::string& path, uint64_t startByte, uint64_t bufferSize, std::vector<unsigned char>& buffer) {
	auto cPath = cleanPath(path);
	// open file
	FILE* fp = port::fopen_mb(cPath.c_str(), _FILE_STR("rb"));
	if (!fp) {
		return -1;
	}

	// validate file size
	fseek(fp, 0, SEEK_END);
	const uint64_t fileSize = ftell(fp);
	rewind(fp);
	if (startByte + bufferSize > fileSize) {
		bufferSize = static_cast<unsigned int>(fileSize - startByte);
	}

	// validate vector size
	size_t bufferSize_t = static_cast<size_t>(bufferSize);
	buffer.resize(bufferSize_t);

	// move file pointer
	FileSizeAndPos pos;
	pos.mUint64 = startByte;
	fseek(fp, pos.mInt32.mHigh | pos.mInt32.mLow, SEEK_SET);

	// read data
	fread(buffer.data(), sizeof(unsigned char), bufferSize_t, fp);

	// close file
	fclose(fp);

	return bufferSize;
}

bool File::createFile(const std::string& path) {
	FILE* fp = port::fopen_mb(path.c_str(), _FILE_STR("w+b"));
	if (!fp) {
		return false;
	}
	fclose(fp);
	return true;
}

bool File::writeFileData(const std::string& path, uint64_t offset, uint64_t bufferSize, const std::vector<unsigned char>& buffer, const port::filepath_char* mode) {
	auto cPath = cleanPath(path);
	// open file
	FILE* fp = port::fopen_mb(cPath.c_str(), mode);
	if (!fp) {
		return false;
	}

	// validate file size
	fseek(fp, 0, SEEK_END);
	const uint64_t fileSize = ftell(fp);
	rewind(fp);
	if (offset > fileSize) {
		// This should not happen
		return false;
	}

	// move file pointer
	FileSizeAndPos pos;
	pos.mUint64 = offset;
	fseek(fp, pos.mInt32.mHigh | pos.mInt32.mLow, SEEK_SET);

	// write data
	size_t bufferSize_t = static_cast<size_t>(bufferSize);
	fwrite(buffer.data(), sizeof(unsigned char), bufferSize_t, fp);

	// close file
	fclose(fp);

	// validate write
	uint64_t minExpectedSize = offset + bufferSize;
	uint64_t validationSize = getFileSize(cPath);
	return (validationSize >= minExpectedSize);
}

std::string File::getUniqueFilePathForFile(const std::string& path) {
	auto cPath = cleanPath(path);

	const std::string filename = Util::getFileName(cPath);
	const std::string fileExtension = Util::getExtension(cPath);
	const std::string parentDirectory = Util::getDirectory(cPath);

	std::string file = cPath;

	int index = 0;
	while (exists(file)) {
		file = Util::format("%s/%s(%d).%s", parentDirectory.c_str(), filename.c_str(), ++index, fileExtension.c_str());
	}

	return file;
}

std::string File::getUniqueFilePathForDirectory(const std::string& path) {
	auto cPath = cleanPath(path);

	const std::string directoryName = Util::getFileName(cPath);
	const std::string parentDirectory = Util::getDirectory(cPath);

	std::string file = cPath;

	int index = 0;
	while (folderExists(file)) {
		file = Util::format("%s/%s(%d)", parentDirectory.c_str(), directoryName.c_str(), ++index);
	}

	return file;
}

std::string File::createUniquePathFromSeed(const std::string& basePath, std::function<std::string(int modifier)> seedGenerator) {
	std::string uniquePath;
	std::string cleanBase = File::cleanPath(basePath);
	static std::atomic_int modifier{0};
	do {
		uniquePath = cleanBase + "/" + seedGenerator(modifier++);
	} while (File::folderExists(uniquePath));

	return uniquePath;
}

// void File::getAvailableDiskFreeSpaceAsync(const std::string& path, std::function<void(uint64_t)> callback) {
// 	auto cPath = cleanPath(path);
// 	std::shared_ptr<uint64_t> shared_size = std::make_shared<uint64_t>();
// 	WorkerPool::getFor(WorkerRole::Disk).queue([cPath, shared_size]() {
// 		*shared_size = AppPlatform::singleton().calculateAvailableDiskFreeSpace(cPath);
// 		return true;
// 	}, [callback, shared_size] {
// 		DEBUG_ASSERT_MAIN_THREAD;
// 		callback(*shared_size);
// 	});
// }
