/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/
#include "Dungeons.h"

#include "world/level/chunk/MainChunkSource.h"

#include "world/level/chunk/LevelChunk.h"
#include "world/level/Level.h"
#include "world/level/levelgen/structure/StructureFeatureType.h"

#include "nbt/CompoundTag.h"

MainChunkSource::MainChunkSource(Unique<ChunkSource> storage) 
	: ChunkSource(std::move(storage)){

}

MainChunkSource::~MainChunkSource(){

}

LevelChunk* MainChunkSource::requestChunk(const ChunkPos& cp, LoadMode lm){
	DEBUG_ASSERT_MAIN_THREAD;

	//if it's not in the map, create the first reference, else grab
	auto elem = mChunkMap.find(cp);
	if (elem == mChunkMap.end()) {
		auto lc = mParent->requestChunk(cp, lm);
		if (!lc) {
			return nullptr;
		}

		mChunkMap[cp] = ChunkRefCount(*lc);	//acquire 2 because in background, it is loading
		return lc;
	}
	else{
		return elem->second.grab();
	}
}

bool MainChunkSource::releaseChunk(LevelChunk& lc){
	DEBUG_ASSERT_MAIN_THREAD;

	auto elem = mChunkMap.find(lc.getPosition());

	if (elem != mChunkMap.end() && elem->second.release() == 0) {
		//TODO move this to a better place in the future?
		mLevel->onChunkDiscarded(elem->second.getChunk());

		//pass it up, to the cache or something
		mParent->acquireDiscarded(std::move(elem->second.getChunkPtr()));

		mChunkMap.erase(elem);

		return true;
	}
	else{
		return false;
	}
}

const ChunkRefCount::Map& MainChunkSource::getStoredChunks() const {
	return mChunkMap;
}

ChunkRefCount::Map& MainChunkSource::getStoredChunks() {
	return mChunkMap;
}

LevelChunk* MainChunkSource::getExistingChunk(const ChunkPos& cp) {
	auto elem = mChunkMap.find( cp );
	return (elem != mChunkMap.end()) ? elem->second.get() : nullptr;
}

void MainChunkSource::acquireDiscarded(ChunkPtr&& ptr){
	//someone under passed up this thing, grab it
	auto& pos = ptr->getPosition();
	auto itr = mChunkMap.find(pos);
	if (itr == mChunkMap.end()) {
		mChunkMap[pos] = ChunkRefCount(std::move(ptr));
	}
	else {
		itr->second.grab();
		ptr.release();
	}
}

bool MainChunkSource::findNearestFeature(StructureFeatureType feature, Player& player, BlockPos& pos) {
	return mParent->findNearestFeature(feature, player, pos);
}

Unique<CompoundTag> MainChunkSource::getStructureTag() const {
	if (mParent != nullptr) {
		return mParent->getStructureTag();
	}
	return make_unique<CompoundTag>();
}

void MainChunkSource::readStructureTag(const CompoundTag& tag) {
	if (mParent != nullptr) {
		mParent->readStructureTag(tag);
	}
}
