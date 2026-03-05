/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/
#pragma once

#include "common_header.h"

class RegionFile {
public:

	RegionFile(const std::string& basePath);
	virtual ~RegionFile();

	bool open();
	bool readChunk(int x, int z, RakNet::BitStream** destChunkData);
	bool writeChunk(int x, int z, RakNet::BitStream& chunkData);

private:

	bool _write(int sector, RakNet::BitStream& chunkData);
	void _close();

	static const int SECTOR_BYTES = 4096;
	static const int SECTOR_INTS = SECTOR_BYTES / 4;
	static const int SECTOR_COLS = 32;

	FILE* mFile;
	std::string mFileName;
	std::array<int, SECTOR_INTS> mOffsets;
	std::array<int, SECTOR_INTS> mEmptyChunk;

	typedef std::map<int, bool> FreeSectorMap;
	FreeSectorMap mSectorFree;
};
