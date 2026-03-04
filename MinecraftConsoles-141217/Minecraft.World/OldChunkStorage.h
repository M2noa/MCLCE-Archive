#pragma once
#include "ChunkStorage.h"
#include "LevelChunk.h"
#include "File.h"
#include "CompoundTag.h"
#include "com.mojang.nbt.h"

class Level;

class OldChunkStorage : public ChunkStorage
{
private:
	// 4J added so we can have separate storage arrays for different threads
	class ThreadStorage
	{
	public:
		byteArray blockData;
		byteArray dataData;
		byteArray skyLightData;
		byteArray blockLightData;

		ThreadStorage();
		~ThreadStorage();
	};
	static DWORD tlsIdx;
	static ThreadStorage *tlsDefault;
public:
	// Each new thread that needs to use Compression will need to call one of the following 2 functions, to either create its own
	// local storage, or share the default storage already allocated by the main thread
	static void CreateNewThreadStorage();
	static void UseDefaultThreadStorage();
	static void ReleaseThreadStorage();

private:
	File	dir;
	bool	create;

public:
    OldChunkStorage(File dir, bool create);
private:
    File getFile(int x, int z);
    LevelChunk *load(Level *level, int x, int z);

public:
    virtual void save(Level *level, LevelChunk *levelChunk);
	
	static bool saveEntities(LevelChunk *lc, Level *level, CompoundTag *tag); // 4J Added
	static void save(LevelChunk *lc, Level *level, DataOutputStream *dos); // 4J Added
    static void save(LevelChunk *lc, Level *level, CompoundTag *tag);
	static void loadEntities(LevelChunk *lc, Level *level, CompoundTag *tag);
    static LevelChunk *load(Level *level, CompoundTag *tag);
	static LevelChunk *load(Level *level, DataInputStream *dis); // 4J Added

    virtual void tick();
    virtual void flush();
    virtual void saveEntities(Level *level, LevelChunk *levelChunk);
};
