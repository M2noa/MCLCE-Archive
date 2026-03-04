#pragma once

#include "FileHeader.h"
#include "ConsoleSavePath.h"
#include "ConsoleSaveFile.h"

class ProgressRenderer;

class ConsoleSaveFileSplit : public ConsoleSaveFile
{
private:
	FileHeader header;

	static const int WRITE_BANDWIDTH_BYTESPERSECOND = 1048576;						// Average bytes per second we will cap to when writing region files during the tick() method
	static const int WRITE_BANDWIDTH_MEASUREMENT_PERIOD_SECONDS = 10;				// Time period over which the bytes per second average is calculated
	static const int WRITE_TICK_RATE_MS = 500;										// Time between attempts to work out which regions we should write during the tick
	static const int WRITE_MAX_WRITE_PER_TICK = WRITE_BANDWIDTH_BYTESPERSECOND;		// Maximum number of bytes we can add in a single tick

	class WriteHistory
	{
	public:
		int64_t			writeTime;
		unsigned int	writeSize;
	} ;

	class DirtyRegionFile
	{
	public:
		int64_t			lastWritten;
		unsigned int	fileRef;
		bool			operator<(const DirtyRegionFile& rhs) { return lastWritten < rhs.lastWritten; }
	};

	class RegionFileReference
	{
	public:
		RegionFileReference(int index, unsigned int regionIndex, unsigned int length = 0, unsigned char *data = NULL);
		~RegionFileReference();
		void			Compress();				// Compress from data to dataCompressed
		void			Decompress();			// Decompress from dataCompressed -> data
		unsigned int	GetCompressedSize();	// Gets byte size for what this region will compress to
		void			ReleaseCompressed();	// Release dataCompressed
		FileEntry		*fileEntry;
		unsigned char	*data;				
		unsigned char   *dataCompressed;
		unsigned int	dataCompressedSize;
		int				index;
		bool			dirty;
		int64_t			lastWritten;
	};
	unordered_map<unsigned int, RegionFileReference *> regionFiles;
	vector<WriteHistory> writeHistory;
	int64_t				m_lastTickTime;

	FileEntry *GetRegionFileEntry(unsigned int regionIndex);

	wstring m_fileName;
	bool m_autosave;

//	HANDLE hHeap;
	static void *pvHeap;
	static unsigned int pagesCommitted;
#ifdef _LARGE_WORLDS
	static const unsigned int CSF_PAGE_SIZE = 64 * 1024;
	static const unsigned int MAX_PAGE_COUNT = 32 * 1024; // 2GB virtual allocation
#else
	static const unsigned int CSF_PAGE_SIZE = 64 * 1024;
	static const unsigned int MAX_PAGE_COUNT = 1024;
#endif
	LPVOID pvSaveMem;

	CRITICAL_SECTION m_lock;

	void PrepareForWrite( FileEntry *file, DWORD nNumberOfBytesToWrite );
	void MoveDataBeyond(FileEntry *file, DWORD nNumberOfBytesToWrite);

	bool GetNumericIdentifierFromName(const wstring &fileName, unsigned int *idOut);
	wstring GetNameFromNumericIdentifier(unsigned int idIn);
	void processSubfilesForWrite();
	void processSubfilesAfterWrite();
public:
	static int SaveSaveDataCallback(LPVOID lpParam,bool bRes);
	static int SaveRegionFilesCallback(LPVOID lpParam,bool bRes);
	
private:
	void _init(const wstring &fileName, LPVOID pvSaveData, DWORD fileSize, ESavePlatform plat);

public:
	ConsoleSaveFileSplit(const wstring &fileName, LPVOID pvSaveData = NULL, DWORD fileSize = 0, bool forceCleanSave = false, ESavePlatform plat = SAVE_FILE_PLATFORM_LOCAL);
	ConsoleSaveFileSplit(ConsoleSaveFile *sourceSave, bool alreadySmallRegions = true, ProgressListener *progress = NULL);
	virtual ~ConsoleSaveFileSplit();

	// 4J Stu - Initial implementation is intended to have a similar interface to the standard Xbox file access functions

	virtual FileEntry *createFile( const ConsoleSavePath &fileName );
	virtual void deleteFile( FileEntry *file );

	virtual void setFilePointer(FileEntry *file,LONG lDistanceToMove,PLONG lpDistanceToMoveHigh,DWORD dwMoveMethod);
	virtual BOOL writeFile(	FileEntry *file, LPCVOID lpBuffer, DWORD nNumberOfBytesToWrite, LPDWORD lpNumberOfBytesWritten );
	virtual BOOL zeroFile(FileEntry *file, DWORD nNumberOfBytesToWrite, LPDWORD lpNumberOfBytesWritten);
	virtual BOOL readFile( FileEntry *file, LPVOID lpBuffer, DWORD nNumberOfBytesToRead, LPDWORD lpNumberOfBytesRead );
	virtual BOOL closeHandle( FileEntry *file );

	virtual void finalizeWrite();
	virtual void tick();

	virtual bool doesFileExist(ConsoleSavePath file);

	virtual void Flush(bool autosave, bool updateThumbnail = true);

#ifndef _CONTENT_PACKAGE
	virtual void DebugFlushToFile(void *compressedData = NULL, unsigned int compressedDataSize = 0);
#endif
	virtual unsigned int getSizeOnDisk();

	virtual wstring getFilename();

	virtual vector<FileEntry *> *getFilesWithPrefix(const wstring &prefix);
	virtual vector<FileEntry *> *getRegionFilesByDimension(unsigned int dimensionIndex);

#if defined(__PS3__)
	virtual wstring getPlayerDataFilenameForLoad(const PlayerUID& pUID);
	virtual wstring getPlayerDataFilenameForSave(const PlayerUID& pUID);
	virtual vector<FileEntry *> *getValidPlayerDatFiles();
#endif

	virtual int getSaveVersion();
	virtual int getOriginalSaveVersion();

	virtual void LockSaveAccess();
	virtual void ReleaseSaveAccess();

	virtual ESavePlatform getSavePlatform();
	virtual bool isSaveEndianDifferent();
	virtual void setLocalPlatform();
	virtual void setPlatform(ESavePlatform plat);
	virtual ByteOrder getSaveEndian();
	virtual ByteOrder getLocalEndian();
	virtual void setEndian(ByteOrder endian);

	virtual void ConvertRegionFile(File sourceFile);
	virtual void ConvertToLocalPlatform();
};