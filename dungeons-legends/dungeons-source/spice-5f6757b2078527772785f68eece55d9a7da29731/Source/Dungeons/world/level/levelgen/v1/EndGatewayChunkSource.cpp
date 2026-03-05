/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/
#include "Dungeons.h"

#include "world/level/levelgen/v1/EndGatewayChunkSource.h"
#include "nbt/CompoundTag.h"

EndGatewayChunkSource::EndGatewayChunkSource(ChunkSource& parent, const BlockPos& entryPortal, const BlockPos& exitOrigin, int radius)
	: ChunkViewSource(parent)
	, mEntryPortal(entryPortal)
	, mOrigin(exitOrigin)
	, mRadius(radius) {

	// Create the area in the provided location, with the provided radius.
	mArea = mParent->getView(mOrigin, mRadius, mParentLoadMode, true);
}

EndGatewayChunkSource::~EndGatewayChunkSource() {
	// Clear the area in a batch
	mParent->hintDiscardBatchBegin();
	mArea.clear();
	mParent->hintDiscardBatchEnd();
}

const BlockPos& EndGatewayChunkSource::getEntryPos() const {
	return mEntryPortal;
}

const BlockPos& EndGatewayChunkSource::getExitOrigin() const {
	return mOrigin;
}

int EndGatewayChunkSource::getRadius() const {
	return mRadius;
}

Unique<CompoundTag> EndGatewayChunkSource::getStructureTag() const {
	return make_unique<CompoundTag>();
}

void EndGatewayChunkSource::readStructureTag(const CompoundTag& tag) {
	UNUSED_PARAMETER(tag)
}
