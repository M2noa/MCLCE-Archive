#include "stdafx.h"
#include "net.minecraft.world.level.h"
#include "net.minecraft.world.level.chunk.h"
#include "MemoryChunkStorage.h"

LevelChunk *MemoryChunkStorage::load(Level *level, int x, int z) //throws IOException 
{
	return NULL;
}

void MemoryChunkStorage::save(Level *level, LevelChunk *levelChunk) //throws IOException
{
}

void MemoryChunkStorage::saveEntities(Level *level, LevelChunk *levelChunk) //throws IOException 
{
}

void MemoryChunkStorage::tick() 
{
}

void MemoryChunkStorage::flush() 
{
}