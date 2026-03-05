/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/
#include "Dungeons.h"

#include "world/level/chunk/PlayerChunkSource.h"

#include "world/level/Level.h"
#include "world/entity/player/Player.h"

#include "world/level/chunk/LevelChunk.h"
#include "nbt/CompoundTag.h"

PlayerChunkSource::PlayerChunkSource(Player& origin, ChunkSource& parent, int radius) 
	: ChunkViewSource(parent)
	, mOrigin(origin)
	, mRadius(radius)
	, mSuspended(false){
}

PlayerChunkSource::~PlayerChunkSource() {
	//pre-clear the area in a batch
	mParent->hintDiscardBatchBegin();
	mArea.clear();
	mParent->hintDiscardBatchEnd();
}

int PlayerChunkSource::getRadius() const {
	return mRadius;
}

void PlayerChunkSource::centerAt(const Vec3& pos, float minDist) {
	if (mSuspended) {
		return;
	}

	//flatten!
	Vec3 flatPos(pos.x, 0, pos.z);
	if (minDist > 0 && flatPos.distanceToSqr(mLastChunkUpdatePos) < minDist * minDist) {
		return;
	}

	BlockPos blockPos(pos);

	if (mArea.empty()) {	//create the view

		//first, create it with a smaller radius to push on the queue the "priority" chunks needed for loading
		mArea = mParent->getView(blockPos, mRadius, mParentLoadMode, true,
			[this] (LevelChunk*& lc) {
			if (lc->getState() == ChunkState::Loaded) {
				mLevel->onNewChunkFor(mOrigin, *lc);
			}
		});
	}
	else {
		//then expand or move the area to its normal size
		BlockPos max = blockPos + mRadius;
		BlockPos min = blockPos - mRadius;
		max.y = min.y = 0;

		mParent->hintDiscardBatchBegin();
		mArea.move(min, max);
		mParent->hintDiscardBatchEnd();
	}

	mLastChunkUpdatePos = flatPos;
}

void PlayerChunkSource::center(float minDist){
	centerAt(mOrigin.getPos(), minDist);
}

void PlayerChunkSource::setRadius(int newRadius){
	DEBUG_ASSERT(newRadius > 1, "Radius is too small");

	mRadius = newRadius;
	center(0);
}

void PlayerChunkSource::suspend() {
	mSuspended = true;
	mParent->hintDiscardBatchBegin();
	clear();
	mParent->hintDiscardBatchEnd();
}

Unique<CompoundTag> PlayerChunkSource::getStructureTag() const {
	return make_unique<CompoundTag>();
}

void PlayerChunkSource::readStructureTag(const CompoundTag& tag) {
	UNUSED_PARAMETER(tag)
}
