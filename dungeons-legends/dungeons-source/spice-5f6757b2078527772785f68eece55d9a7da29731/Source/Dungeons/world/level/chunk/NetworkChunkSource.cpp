/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/
#include "Dungeons.h"
#include "world/level/chunk/NetworkChunkSource.h"

#include "world/level/chunk/LevelChunk.h"
#include "world/level/Level.h"
// #include "network/packet/FullChunkDataPacket.h"
#include "nbt/CompoundTag.h"
// #include "platform/threading/BackgroundWorker.h"
// #include "platform/threading/WorkerPool.h"

LevelChunk* NetworkChunkSource::getExistingChunk(const ChunkPos& cp) {
	//retrieve a chunk from the area if any

	auto element = mChunkMap.find(cp);

	return element != mChunkMap.end()
		   ? element->second.get()
		   : nullptr;
}

NetworkChunkSource::NetworkChunkSource(Level* level, Dimension* dimension) 
	: ChunkSource(level, dimension, CHUNK_WIDTH) {
}

LevelChunk* NetworkChunkSource::requestChunk(const ChunkPos& cp, LoadMode lm) {
	auto elem = mChunkMap.find(cp);
	if (elem == mChunkMap.end()) {
		auto incoming = mIncomingChunks.find(cp);

		//check if this chunk arrived before the player needed it
		ChunkPtr lc;
		if (incoming != mIncomingChunks.end()) {
			lc = std::move(incoming->second);
			mIncomingChunks.erase(cp);
		}
		else if (lm != LoadMode::None) {
			lc = make_unique<LevelChunk>(*mLevel, *mDimension, cp);
		}
		else{
			return nullptr;
		}

		return (mChunkMap[cp] = ChunkRefCount(std::move(lc), 1)).get();
	}
	else{
		return elem->second.grab();
	}
}

bool NetworkChunkSource::releaseChunk(LevelChunk& lc) {
	auto itr = mChunkMap.find(lc.getPosition());

	DEBUG_ASSERT(itr != mChunkMap.end(), "Releasing a chunk not in the source");

	if (itr->second.release() == 0) {
		mLevel->onChunkDiscarded(itr->second.getChunk());

		if (mParent) {
			mParent->acquireDiscarded( std::move(itr->second.getChunkPtr()));
		}

		mChunkMap.erase(itr);
		return true;
	}
	else{
		return false;
	}
}

const ChunkRefCount::Map& NetworkChunkSource::getStoredChunks() const {
	return mChunkMap;
}

ChunkRefCount::Map& NetworkChunkSource::getStoredChunks() {
	return mChunkMap;
}


void NetworkChunkSource::acquireDiscarded(ChunkPtr&& discarded) {
	//someone under passed up this thing, grab it
	auto& pos = discarded->getPosition();
	auto itr = mChunkMap.find(pos);
	if (itr == mChunkMap.end()) {
		mChunkMap[pos] = ChunkRefCount(std::move(discarded));
	}
	else {
		itr->second.grab();
		discarded.release();
	}
}

LevelChunk& NetworkChunkSource::getIncomingChunk(const ChunkPos& cp) {
	if(auto lc = getExistingChunk(cp)) {
		return *lc;
	}

	//no chunk was loaded - query one in the incoming queue
	return *(mIncomingChunks[cp] = make_unique<LevelChunk>(*mLevel, *mDimension, cp));
}

void NetworkChunkSource::onLowMemory() {
	//TODO nicer place to put this?

}

Unique<CompoundTag> NetworkChunkSource::getStructureTag() const {
	DEBUG_ASSERT(false, "This should never be called");
	return make_unique<CompoundTag>();
}

void NetworkChunkSource::readStructureTag(const CompoundTag& tag) {
	DEBUG_ASSERT(false, "This should never be called" );
	UNUSED_PARAMETER(tag)
}

const ChunkPos neighborOffsets[] = {
	{ 0, 0 },
	{ -1, -1 },
	{ 0, -1 },
	{ 1, -1 },
	{ -1, 0 },
	{ 1, 0 },
	{ -1, 1 },
	{ 0, 1 },
	{ 1, 1 }
};

void NetworkChunkSource::updateCachedData(LevelChunk* lc) {
	// This needs to be done on a worker thread so that it will keep retrying
	// until all of it's neighbors make it to at least the generated state.
// 	WorkerPool::getFor(WorkerRole::Streaming).queue(
// 		[lc, this]() {
// 			for (auto& offset : neighborOffsets) {
// 				auto elem = mChunkMap.find(lc->getPosition() + offset);
// 				if (elem == mChunkMap.end() || elem->second->getState() < ChunkState::Generated) {
// 					return false;
// 				}
// 			}
// 
// 			return true;
// 		},
// 		[this, lc]() {
// 			lc->tryChangeState(ChunkState::Generated, ChunkState::Loaded);
// 			lc->resetCachedDataState();
// 			_fireChunkLoaded(*lc);
// 		},
// 		32 + _getChunkPriority(*lc)
// 	);
}
