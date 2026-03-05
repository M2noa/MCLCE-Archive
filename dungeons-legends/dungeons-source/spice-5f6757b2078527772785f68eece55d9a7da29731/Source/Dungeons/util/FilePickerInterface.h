/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/

#pragma once

struct FileInfo {
	FileInfo(std::string _filePath = "", int _fileSize = 0, std::string _fileHash = "")
		: filePath(_filePath)
		, fileSize(_fileSize)
		, fileHash(_fileHash) {
	}

	bool valid() const { return !filePath.empty() && !fileHash.empty() && fileSize > 0; };

	std::string filePath;
	uint64_t fileSize;
	std::string fileHash;

};

class IFilePicker {

public:

	virtual ~IFilePicker() { };

	virtual void initFilePick(const std::string& filePath, std::function<void(bool, FileInfo)> callback) = 0;

	virtual uint64_t readBytes(const FileInfo& file, uint64_t offset, uint64_t bufferSize, std::vector<unsigned char>& buffer) = 0;
	virtual bool writeBytes(const FileInfo& file, uint64_t offset, uint64_t bufferSize, const std::vector<unsigned char>& buffer) = 0;
};