#pragma once

#include "Biome.h"
class ProgressListener;
class TilePos;

// The maximum number of chunks that we can store
// 4J Stu - INCREASED WORLD SIZE LIMIT FOR INFINITE-LIKE WORLDS
// Original: 54 chunks (864 blocks), Large Worlds: 320 chunks (5120 blocks)
// New: 2048 chunks = 32,768 blocks (practically infinite, Java edition is 30,000 blocks)
#ifdef _LARGE_WORLDS
#define LEVEL_MAX_WIDTH 2048  // 32,768×32,768 blocks - essentially infinite
#else
#define LEVEL_MAX_WIDTH 2048  // Increased from 54 for unlimited creative building
#endif
#define LEVEL_MIN_WIDTH 54
#define LEVEL_LEGACY_WIDTH 54

// Scale was 8 in the Java game, but that would make our nether tiny
// Every 1 block you move in the nether maps to HELL_LEVEL_SCALE blocks in the overworld
// 4J - Increased scale to match larger overworld
#define HELL_LEVEL_MAX_SCALE 8  // Increased from 3 to keep nether manageable
#define HELL_LEVEL_MIN_SCALE 3
#define HELL_LEVEL_LEGACY_SCALE 3

#define HELL_LEVEL_MAX_WIDTH (LEVEL_MAX_WIDTH / HELL_LEVEL_MAX_SCALE)  // 256 chunks = 4,096 blocks
#define HELL_LEVEL_MIN_WIDTH 18

#define END_LEVEL_SCALE 3
// 4J Stu - The End stays fixed size (no need for infinite end dimension)
#define END_LEVEL_MAX_WIDTH 18
#define END_LEVEL_MIN_WIDTH 18

class ChunkSource
{
public:
	// 4J Added so that we can store the maximum dimensions of this world
	int m_XZSize;

public:
	virtual ~ChunkSource() {}

    virtual bool hasChunk(int x, int y) = 0;
	virtual bool reallyHasChunk(int x, int y) { return hasChunk(x,y); }	// 4J added
    virtual LevelChunk *getChunk(int x, int z) = 0;
	virtual void lightChunk(LevelChunk *lc) {}	// 4J added
    virtual LevelChunk *create(int x, int z) = 0;
    virtual void postProcess(ChunkSource *parent, int x, int z) = 0;
	virtual bool saveAllEntities() { return false; } // 4J Added
    virtual bool save(bool force, ProgressListener *progressListener) = 0;
    virtual bool tick() = 0;
    virtual bool shouldSave() = 0;

	virtual LevelChunk **getCache() { return NULL; }		// 4J added
	virtual void dataReceived(int x, int z) {}				// 4J added

    /**
     * Returns some stats that are rendered when the user holds F3.
     */
    virtual wstring gatherStats() = 0;

	virtual vector<Biome::MobSpawnerData *> *getMobsAt(MobCategory *mobCategory, int x, int y, int z) = 0;
    virtual TilePos *findNearestMapFeature(Level *level, const wstring& featureName, int x, int y, int z) = 0;
};
