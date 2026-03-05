/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/
#include "Dungeons.h"

#include "world/level/storage/RegionFile.h"

#include "Core/Utility/Filepath.h"
#include "Core/Debug/Log.h"

static const char* const REGION_DAT_NAME = "chunks.dat";

static void logAssert(int actual, int expected) {
	if (actual != expected) {
		LOGI("ERROR: I/O operation failed (%d vs %d)\n", actual, expected);
	}
}

RegionFile::RegionFile(const std::string& basePath)
	: mFile(nullptr) {

	mFileName = basePath;
	mFileName += "/";
	mFileName += REGION_DAT_NAME;

	mOffsets.fill(0);
	mEmptyChunk.fill(0);
}

RegionFile::~RegionFile(){
	_close();
}

bool RegionFile::open(){
	_close();

	mOffsets.fill(0);

	// attempt to open file
	mFile = port::fopen_mb(mFileName.c_str(), _FILE_STR("r+b"));
	if (mFile) {
		// read offset table
		logAssert(fread(mOffsets.data(), sizeof(int), mOffsets.size(), mFile), mOffsets.size());

		// mark initial sector as blocked
		mSectorFree[0] = false;

		// setup blocked sectors
		for (size_t sector = 0; sector < mOffsets.size(); sector++) {
			int offset = mOffsets[sector];
			if (offset) {
				int base = offset >> 8;
				int count = offset & 0xff;

				for (int i = 0; i < count; i++) {
					mSectorFree[base + i] = false;
				}
			}
		}
	}
	else{
		// new world
		mFile = port::fopen_mb(mFileName.c_str(), _FILE_STR("w+b"));
		if (!mFile) {
			LOGI("Failed to create chunk file %s\n", mFileName.c_str());
			return false;
		}

		// write sector header (all zeroes)
		logAssert(fwrite(mOffsets.data(), sizeof(int), mOffsets.size(), mFile), mOffsets.size());

		// mark initial sector as blocked
		mSectorFree[0] = false;
	}

	return mFile != nullptr;
}

void RegionFile::_close(){
	if (mFile) {
		fclose(mFile);
		mFile = nullptr;
	}
}

bool RegionFile::readChunk(int x, int z, RakNet::BitStream** destChunkData){
	int offset = mOffsets[x + z * SECTOR_COLS];

	if (offset == 0) {
		// not written to file yet
		return false;
	}

	int sectorNum = offset >> 8;

	fseek(mFile, sectorNum * SECTOR_BYTES, SEEK_SET);
	int length = 0;
	fread(&length, sizeof(int), 1, mFile);

	DEBUG_ASSERT(length < ((offset & 0xff) * SECTOR_BYTES), "Sector is too long" );

	length -= sizeof(int);
	if (length <= 0) {
		*destChunkData = nullptr;
		return false;
	}

	unsigned char* data = new unsigned char[length];
	logAssert(fread(data, 1, length, mFile), length);
	*destChunkData = new RakNet::BitStream(data, length, false);

	return true;
}

bool RegionFile::writeChunk(int x, int z, RakNet::BitStream& chunkData){
	int size = chunkData.GetNumberOfBytesUsed() + sizeof(int);

	int offset = mOffsets[x + z * SECTOR_COLS];
	int sectorNum = offset >> 8;
	int sectorCount = offset & 0xff;
	int sectorsNeeded = (size / SECTOR_BYTES) + 1;

	if (sectorsNeeded > 256) {
		LOGI("ERROR: Chunk is too big to be saved to file\n");
		return false;
	}

	if (sectorNum != 0 && sectorCount == sectorsNeeded) {
		// the sector can be written on top of its old data
		_write(sectorNum, chunkData);
	}
	else{
		// we need a new location

		// mark old sectors as free
		for (int i = 0; i < sectorCount; i++) {
			mSectorFree[sectorNum + i] = true;
		}

		// find an available slot with enough run length
		int slot = 0;
		int runLength = 0;
		bool extendFile = false;

		while (runLength < sectorsNeeded) {
			if (mSectorFree.find(slot + runLength) == mSectorFree.end()) {
				extendFile = true;
				break;
			}
			if (mSectorFree[slot + runLength] == true) {
				runLength++;
			}
			else{
				slot = slot + runLength + 1;
				runLength = 0;
			}
		}

		if (extendFile) {
			fseek(mFile, 0, SEEK_END);

			for (int i = 0; i < (sectorsNeeded - runLength); i++) {
				fwrite(mEmptyChunk.data(), sizeof(int), SECTOR_INTS, mFile);
				mSectorFree[slot + i] = true;
			}
		}
		mOffsets[x + z * SECTOR_COLS] = (slot << 8) | sectorsNeeded;

		// mark slots as taken
		for (int i = 0; i < sectorsNeeded; i++) {
			mSectorFree[slot + i] = false;
		}

		// write!
		_write(slot, chunkData);

		// write sector data
		fseek(mFile, (x + z * SECTOR_COLS) * sizeof(int), SEEK_SET);
		fwrite(&mOffsets[x + z * SECTOR_COLS], sizeof(int), 1, mFile);
	}

	return true;
}

bool RegionFile::_write(int sector, RakNet::BitStream& chunkData){
	fseek(mFile, sector * SECTOR_BYTES, SEEK_SET);
	//LOGI("writing %d B to sector %d\n", chunkData.GetNumberOfBytesUsed(), sector);
	int size = chunkData.GetNumberOfBytesUsed() + sizeof(int);
	logAssert(fwrite(&size, sizeof(int), 1, mFile), 1);
	logAssert(fwrite(chunkData.GetData(), 1, chunkData.GetNumberOfBytesUsed(), mFile), chunkData.GetNumberOfBytesUsed());

	return true;
}
