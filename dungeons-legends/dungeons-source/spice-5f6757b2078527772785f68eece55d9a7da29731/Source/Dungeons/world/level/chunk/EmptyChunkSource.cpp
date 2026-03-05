/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/
#include "Dungeons.h"

#include "world/level/chunk/EmptyChunkSource.h"
#include "world/level/chunk/LevelChunk.h"
#include "nbt/CompoundTag.h"

EmptyChunkSource::EmptyChunkSource(Level* level, Dimension* dimension) 
	: ChunkSource(level, dimension, CHUNK_WIDTH) {

}

LevelChunk* EmptyChunkSource::requestChunk(const ChunkPos& cp, LoadMode lm) {
	return lm == LoadMode::None ? nullptr : new LevelChunk(*mLevel, *mDimension, cp);
}

void EmptyChunkSource::loadChunk(LevelChunk& lc, bool forceImmediateReplacementDataLoad) {
	lc.setFinalized(LevelChunk::Finalization::Done);
	lc.setSaved();
	lc.changeState(ChunkState::Generating, ChunkState::Loaded);
}

bool EmptyChunkSource::postProcess(ChunkViewSource& neighborhood) {
	DEBUG_FAIL("Do not run postProcess for flat worlds");

	return true;
}

Unique<CompoundTag> EmptyChunkSource::getStructureTag() const {
	return make_unique<CompoundTag>();
}

void EmptyChunkSource::readStructureTag(const CompoundTag& tag) {
	UNUSED_PARAMETER(tag)
}

