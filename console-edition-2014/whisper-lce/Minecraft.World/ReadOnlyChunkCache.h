#pragma once
#include "ChunkSource.h"
#include "LevelChunk.h"
#include "EmptyLevelChunk.h"
#include "ChunkStorage.h"

class ProgressListener;

class ReadOnlyChunkCache : public ChunkSource
{
private:
    static const int LEN = 16;
    static const int LEN_MASK = LEN - 1;
    LevelChunkArray chunks;
    Level *level;
    ChunkStorage *storage;

public:
    ReadOnlyChunkCache(Level *level, ChunkStorage *storage);
	virtual ~ReadOnlyChunkCache();

    virtual bool hasChunk(int x, int z);
    byteArray emptyPixels;
    virtual LevelChunk *create(int x, int z);
    virtual LevelChunk *getChunk(int x, int z);

private:
    LevelChunk *load(int x, int z);	// 4J - TODO - was synchronized

public:
    virtual void postProcess(ChunkSource *parent, int x, int z);
    virtual bool save(bool force, ProgressListener *progressListener);
    virtual bool tick();
    virtual bool shouldSave();
    virtual wstring gatherStats();

	virtual vector<Biome::MobSpawnerData *> *getMobsAt(MobCategory *mobCategory, int x, int y, int z);
    virtual TilePos *findNearestMapFeature(Level *level, const wstring& featureName, int x, int y, int z);
};
