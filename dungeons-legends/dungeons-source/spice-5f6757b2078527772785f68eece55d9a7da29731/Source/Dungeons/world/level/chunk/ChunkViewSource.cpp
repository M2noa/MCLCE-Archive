/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/
#include "Dungeons.h"

#include "world/level/chunk/ChunkViewSource.h"
#include "world/level/ChunkPos.h"
#include "nbt/CompoundTag.h"

ChunkViewSource::ChunkViewSource(ChunkSource& parent, LoadMode parentLoadMode) 
	: ChunkSource(parent)
	, mParentLoadMode(parentLoadMode) {

}

LevelChunk* ChunkViewSource::getExistingChunk(const ChunkPos& pos) {
	//retrieve a chunk from the area if any, don't delegate
	Pos p(pos.x, 0, pos.z);

	return mArea.size() && mArea.getBounds().contains(p) ? mArea.at(p) : nullptr;
}

void ChunkViewSource::move(const BlockPos& blockMin, const BlockPos& blockMax) {
	BlockPos min(blockMin), max(blockMax);
	min.y = max.y = 0;
	if (mArea) {
		Bounds newBounds(min, max, CHUNK_WIDTH);
		mArea.move(min, max);
	}
	else{
		mArea = mParent->getView(min, max, mParentLoadMode, false);
	}
}

void ChunkViewSource::move(const BlockPos& center, int radius) {
	move(center - radius, center + radius);
}

void ChunkViewSource::clear() {
	mArea.clear();
}

ChunkSourceView& ChunkViewSource::getArea() {
	return mArea;
}

Unique<CompoundTag> ChunkViewSource::getStructureTag() const {
	DEBUG_ASSERT(false, "This should never be called");
	return make_unique<CompoundTag>();
}

void ChunkViewSource::readStructureTag(const CompoundTag& tag) {
	UNUSED_PARAMETER(tag)
}
