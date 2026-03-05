#pragma once

class Level;

class ChunkStorage
{
public:
    virtual LevelChunk *load(Level *level, int x, int z) = 0;
    virtual void save(Level *level, LevelChunk *levelChunk) = 0;
    virtual void saveEntities(Level *level, LevelChunk *levelChunk) = 0;
    virtual void tick() = 0;
    virtual void flush() = 0;
	virtual void WaitForAll() {}; // 4J Added
	virtual void WaitIfTooManyQueuedChunks() {}; // 4J Added
};
