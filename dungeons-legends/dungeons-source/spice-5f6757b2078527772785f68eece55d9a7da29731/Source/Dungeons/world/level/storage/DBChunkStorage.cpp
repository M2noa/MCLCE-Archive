/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/
#include "Dungeons.h"

#include "world/level/storage/DBChunkStorage.h"

 #include <leveldb/write_batch.h>
 #include <leveldb/compressor.h>
 #include <leveldb/options.h>

#include "nbt/NbtIo.h"
#include "nbt/CompoundTag.h"
// #include "platform/threading/WorkerPool.h"
#include "util/DebugStats.h"
#include "util/KeyValueInput.h"
// #include "util/PerfTimer.h"
#include "util/StringByteInput.h"
#include "util/StringByteOutput.h"
#include "world/entity/Entity.h"
#include "world/level/biome/Biome.h"
#include "world/level/BlockSource.h"
#include "world/level/chunk/ChunkViewSource.h"
#include "world/level/chunk/LevelChunk.h"
#include "world/level/chunk/PostprocessingManager.h"
#include "world/level/FoliageColor.h"
#include "world/level/GeneratorType.h"
#include "world/level/Level.h"
#include "world/level/levelgen/structure/StructureFeatureType.h"
#include "world/level/storage/DBStorage.h"
#include "world/level/storage/LevelData.h"
#include "world/level/dimension/Dimension.h"
#include "../SourceDeps/Core/Debug/Log.h"

#include <leveldb/db.h>

#define DISABLE_SAVING 0

class ChunkKey {
public:

	ChunkKey(const ChunkPos& pos, const int id)
		: pos(pos)
		, id(id) {
	}

	explicit ChunkKey(const LevelChunk& lc) :
		ChunkKey(lc.getPosition(), lc.getDimensionId()) {
	}

	string_span asSpan() const {
		// ChunkKey for id==0 (overworld dimension) does not contain the id as part of the key.
		// This is for backward compatibility with levels that don't know about dimensions.
		size_t sizeOfId = (id == 0) ? 0 : sizeof(id);

		return string_span(reinterpret_cast<const char*>(this), sizeof(pos) + sizeOfId);
	}

private:

	const ChunkPos pos;
	const int id;
};

// ThreadLocal<leveldb::WriteBatch> DBChunkStorage::threadBatch([] (){
// 	return make_unique<leveldb::WriteBatch>();
// });
// 
// ThreadLocal<std::string> DBChunkStorage::threadBuffer([] (){
// 	return make_unique<std::string>();
// });

DBChunkStorage::DBChunkStorage(Unique<ChunkSource> parent, DBStorage* storage) 
	: ChunkSource(std::move(parent))
	, MemoryTracker("DBChunkStorage")
	, mStorage(storage)
	, mPending(0) {

}

DBChunkStorage::~DBChunkStorage() {
	waitDiscardFinished();
}

void DBChunkStorage::waitDiscardFinished() {
	//flush out all batched chunks
	if(mDiscardBatch.size() > 0) {
		_writeBatch();
	}

// 	for (auto&& queue : WorkerPool::getWorkersFor(WorkerRole::Disk)) {
// 		queue->sync();
// 	}
// 
// 	ALOGI(LOG_AREA_STORAGE, "Saved pending chunks\n");
}

std::string join(string_span prefix, LevelChunk::Tag tag) {
	return prefix + static_cast<char>(tag);
}

std::string join(string_span prefix, LevelChunk::Tag tag, uint32_t i) {
	return join(prefix, tag) + (char)i;
}

bool DBChunkStorage::_hasChunk(const ChunkKey &key) {
	return mStorage->hasKey(join(key.asSpan(), LevelChunk::Tag::Version)) ||
		   mStorage->hasKey(join(key.asSpan(), LevelChunk::Tag::LegacyTerrain));
}

void DBChunkStorage::loadChunk(LevelChunk& lc, bool forceImmediateReplacementDataLoad) {

	DEBUG_ASSERT(lc._getTerrainDataState() == ChunkTerrainDataState::NoData, "Attempting to call DBChunkStorage::loadChunk() on a chunk that already has been processed!");

	DEBUG_ASSERT(lc.getGenerator() == nullptr, "DBChunkStorage should be the first to attempt to set the levelchunk's generator!!!");
	lc._setGenerator(this);

	// if we are in the DB, we need to check the 8 neighbors.  If they are in the DB, we do not generate the chunk as it's
	// fully saved and doesn't bleed original data to the neighbors (as they are loaded too)
	ChunkKey key(lc);

	if (_hasChunk(key)) {
		int count = 0;

		// check neighbors, unless we're force-loading or have old terrain data
		if (forceImmediateReplacementDataLoad == false && lc.needsUpgradeFix() == false) {
			// this chunk is saved, check the neighbors
			ChunkPos neighbors[8];

			ChunkPos center = lc.getPosition();

			neighbors[0] = center - ChunkPos(-1, -1);
			neighbors[1] = center - ChunkPos(0, -1);
			neighbors[2] = center - ChunkPos(1, -1);
			neighbors[3] = center - ChunkPos(-1, 0);
			neighbors[4] = center - ChunkPos(1, 0);
			neighbors[5] = center - ChunkPos(-1, 1);
			neighbors[6] = center - ChunkPos(0, 1);
			neighbors[7] = center - ChunkPos(1, 1);

			for (int i = 0; i < 8; i++) {
				ChunkKey keyNeighbor(neighbors[i], lc.getDimensionId());
				
				if (!_hasChunk(key)) {
					break;
				}

				count++;
			}
		}

		if (count == 8 || forceImmediateReplacementDataLoad || lc.needsUpgradeFix()) {
			// all neighbors are in the DB, load the chunk and exit
			bool loaded = _loadChunkFromDB(lc);

			if (loaded) {
				lc.changeState(
					ChunkState::Generating,
					lc.needsUpgradeFix() ? ChunkState::Generated : ChunkState::Loaded);

				// TODO: rherlitz fix
				lc.changeState(
					ChunkState::Unloaded,
					lc.needsUpgradeFix() ? ChunkState::Generated : ChunkState::Loaded);

				lc._changeTerrainDataState(
					ChunkTerrainDataState::NoData,
					lc.needsUpgradeFix() ? ChunkTerrainDataState::NeedsFixup : ChunkTerrainDataState::Ready);

				return;
			}
		}
	}

	// generate the chunk, to ensure neighbors are properly spilled over to
	// we will override this chunk's data later if there's saved data
	ChunkSource::loadChunk(lc, forceImmediateReplacementDataLoad);

	lc._changeTerrainDataState(ChunkTerrainDataState::NoData, ChunkTerrainDataState::ReadyForGeneration);
}

bool DBChunkStorage::_replaceChunkDataIfSaved(LevelChunk& lc, ChunkViewSource& neighborhood) {

	// see if there's data to load
	ChunkKey key(lc);

	if (!_hasChunk(key)) {
		return true; // no data to load, done
	}

	// ok, need to lock neighborhood to replace chunk
	auto& nmin = neighborhood.getArea().getBounds().mMin;
	ChunkPos centerPos(nmin.x + 1, nmin.z + 1);

	auto& lcPosition = lc.getPosition();
	DEBUG_ASSERT(centerPos == lcPosition, "the chunk is in the wrong position, something's up mang");
	DEBUG_ASSERT(neighborhood.getArea().getBounds().mVolume == 9, "Wrong number of chunks in the PP neighborhood");

	//the ownership is automatically released (RAII FTW)
	auto ownsNeighborhood = PostprocessingManager::instance.tryLock(lcPosition);
	if (ownsNeighborhood) {
		_loadChunkFromDB(lc);

		// ensure correct light data spills to neighbors
		BlockSource source(neighborhood.getLevel(), neighborhood.getDimension(), neighborhood, false, true);
		lc.updateLightsAndHeights(source);

		return true;
	}
	else {
		return false; // couldn't lock the neighborhood, bail and try again
	}
}

// this will override the generated chunk data with saved data
bool DBChunkStorage::_loadChunkFromDB(LevelChunk& lc) {

	KeyValueInput keyValues;
	ChunkKey key(lc);

	// overwrites the chunk data with the stored data
	if (mStorage->_read( key.asSpan(), keyValues)) {//could we read THE THING?

		if(lc.deserialize(keyValues)) {
			lc.setDebugDisplaySavedState(true);

			//go either to fully loaded or generated waiting for postprocessing
			// not that legacy chunks will get fixed up automatically by the lighting algorithm fixup later that all 
			// modified chunks get
			return true;
		}
		else {
			LOGE("WARNING: corrupted chunk at %d %d\n", lc.getPosition().x, lc.getPosition().z);
		}
	}

	return false;
}

bool DBChunkStorage::postProcess(ChunkViewSource& neighborhood) {

	//here we fix old chunks
	auto& nmin = neighborhood.getArea().getBounds().mMin;
	ChunkPos centerPos(nmin.x + 1, nmin.z + 1);

	auto lc = neighborhood.getExistingChunk(centerPos);

	DEBUG_ASSERT(lc->_getTerrainDataState() != ChunkTerrainDataState::NoData, "Attempting to post-process a terrain chunk with invalid terrain state");

	if (lc->_getTerrainDataState() == ChunkTerrainDataState::NeedsFixup) {
		auto lcfmt = lc->getLoadedFormat();
		if (lcfmt < LevelChunkFormat::v9_05) {
			//fix biomes in flat worlds
			if (mLevel->getLevelData().getGenerator() == GeneratorType::Flat) {
				for (ChunkBlockPos c; c.x < CHUNK_WIDTH; ++c.x) {
					for (c.z = 0; c.z < CHUNK_WIDTH; ++c.z) {
						lc->setBiome(*Biome::plains, c);
					}
				}
			}

		}

		lc->_changeTerrainDataState(ChunkTerrainDataState::NeedsFixup, ChunkTerrainDataState::ReadyForGeneration);
	}

	if (lc->_getTerrainDataState() == ChunkTerrainDataState::ReadyForGeneration) {

		// call parent's postprocess, if it exists
		if (mParent) {
			if (mParent->postProcess(neighborhood) == false) {
				return false; // exit and reschedule
			}
		}

		lc->_changeTerrainDataState(ChunkTerrainDataState::ReadyForGeneration, ChunkTerrainDataState::PostProcessed);
	}

	if (lc->_getTerrainDataState() == ChunkTerrainDataState::PostProcessed) {

		// now check for replacement data
		if (_replaceChunkDataIfSaved(*lc, neighborhood) == false) {
			return false; // exit and reschedule
		}

		lc->_changeTerrainDataState(ChunkTerrainDataState::PostProcessed, ChunkTerrainDataState::Ready);
	}

	// all done!
	return true;
}

void DBChunkStorage::hintDiscardBatchBegin(){
	DEBUG_ASSERT(!mBatch, "Batch already started");

	mBatch = true;
}

void DBChunkStorage::_serializeChunk(const LevelChunk& lc, leveldb::WriteBatch& batch){
	DEBUG_ASSERT(lc.getState() == ChunkState::Loaded, "It is unsafe to write an incomplete chunk");

// 	auto& buffer = threadBuffer.getLocal();
// 	StringByteOutput stream(buffer);
// 	ChunkKey lcKey(lc);
// 
// 	auto prefix = lcKey.asSpan();
// 
// 	//store the level chunk format name for future compatibility - with leveldb we can update chunks one by one!
// 	buffer.clear();
// 	stream.writeByte((char)SharedConstants::CurrentLevelChunkFormat);
// 
// 	batch.Put(join(prefix, LevelChunk::Tag::Version), buffer);
// 
// 	//make sure that the chunk was saved in the proper format
// 	//don't save only the minimal changeset if not
// 	bool alreadySavedWithSubChunks = mStorage->hasKey(join(prefix, LevelChunk::Tag::Data2D)) || mStorage->hasKey(join(prefix, LevelChunk::Tag::Data2DLegacy));
// 
// 	bool anyTerrainSaved = false;
// 
// 	//remove all the top chunks that are empty
// 	auto subChunkCount = lc.countSubChunksAfterPrune();
// 
// 	uint8_t scIndex = 0;
// 	for(auto i : range(subChunkCount)) {
// 		auto& sc = lc.getSubChunks()[i];
// 
// 		if (!alreadySavedWithSubChunks || sc->mDirtyTicksCounter.isDirty()) {
// 			buffer.clear();
// 			sc->serialize(stream);
// 
// 			batch.Put(join(prefix, LevelChunk::Tag::SubChunkPrefix, scIndex), buffer);
// 			
// 			anyTerrainSaved = true;
// 		}
// 		++scIndex;
// 	}
// 
// 	if (anyTerrainSaved) {
// 		buffer.clear();
// 		lc.serialize2DMaps(stream);
// 		batch.Put(join(prefix, LevelChunk::Tag::Data2D), buffer);
// 
// 		if (alreadySavedWithSubChunks) {
// 			//delete the keys that aren't there anymore, they might be subchunks that were emptied
// 			for (; scIndex < lc.getMaxSubChunks(); ++scIndex) {
// 				batch.Delete(join(prefix, LevelChunk::Tag::SubChunkPrefix, scIndex));
// 			}
// 		}
// 	}
// 	
// 	if (lc.getDirtyTicksCounter(LevelChunkDataField::Entity).isDirty()) {
// 		auto& list = lc.getEntities();
// 		if (list.size() > 0) {
// 			buffer.clear();
// 			lc.serializeEntities(stream);
// 			batch.Put(join(prefix, LevelChunk::Tag::Entity), buffer);
// 		}
// 		else {
// 			batch.Delete(join(prefix, LevelChunk::Tag::Entity));	//no entities anymore, remove the tag altogether
// 		}
// 	}
// 
// 	if (lc.getDirtyTicksCounter(LevelChunkDataField::BlockEntity).isDirty() || !alreadySavedWithSubChunks) {
// 		if (lc.getBlockEntities().empty()) {
// 			batch.Delete(join(prefix, LevelChunk::Tag::BlockEntity));
// 		}
// 		else {
// 			buffer.clear();
// 			lc.serializeBlockEntities(stream);
// 
// 			batch.Put(join(prefix, LevelChunk::Tag::BlockEntity), buffer);
// 		}
// 	}
// 
// 	if (lc.getDirtyTicksCounter(LevelChunkDataField::BlockExtraData).isDirty()) {
// 		if (!lc.hasAnyBlockExtraData()) {
// 			batch.Delete(join(prefix, LevelChunk::Tag::BlockExtraData));
// 		}
// 		else {
// 			buffer.clear();
// 			lc.serializeBlockExtraData(stream);
// 
// 			batch.Put(join(prefix, LevelChunk::Tag::BlockExtraData), buffer);
// 		}
// 	}
// 
// 	if (lc.getDirtyTicksCounter(LevelChunkDataField::BiomeState).isDirty()) {
// 		if (!lc.hasAnyBiomeStates()) {
// 			batch.Delete(join(prefix, LevelChunk::Tag::BiomeState));
// 		}
// 		else {
// 			buffer.clear();
// 			lc.serializeBiomeStates(stream);
// 
// 			batch.Put(join(prefix, LevelChunk::Tag::BiomeState), buffer);
// 		}
// 	}
// 
// 	if (lc.getDirtyTicksCounter(LevelChunkDataField::PendingTicks).isDirty()) {
// 		if (lc.getTickQueue().isEmpty()) {
// 			batch.Delete(join(prefix, LevelChunk::Tag::PendingTicks));
// 		}
// 		else {
// 			buffer.clear();
// 			lc.serializeTicks(stream);
// 
// 			batch.Put(join(prefix, LevelChunk::Tag::PendingTicks), buffer);
// 		}
// 	}
// 
// 	// serialize the finalization state
// 	buffer.clear();
// 	lc.serializeFinalization(stream);
// 	batch.Put(join(prefix, LevelChunk::Tag::FinalizedState), buffer);
// 
// 	//delete deprecated tags to free up space
// 	if (lc.getLoadedFormat() < LevelChunkFormat::v17_0) {
// 		batch.Delete(join(prefix, LevelChunk::Tag::LegacyTerrain));
// 	}
}

void DBChunkStorage::_writeBatch() {
// 
// 	//transfer the ownership into a shared pointer because lambdas don't capture uniques :(
// 	auto list = make_shared<ChunkPtrList>(std::move(mDiscardBatch));
// 
// #ifndef PUBLISH
// 	mPending += list->size();
// #endif
// 	// REVIEW:  Possibly restore change for QueueRole here to STREAMING instead of "SAVE"
// 	WorkerPool::getFor(WorkerRole::Disk).queue([this, list] () {
// 		ScopedProfile("saveToDisk");
// 		//save to disk using the thread-local buffer
// 		auto& batch = threadBatch.getLocal();
// 		batch.Clear();
// /*
// 		adors - If we keep getting crashes on save while riding a pig, I think this might fix it
// 		for (auto& chunk : *list) { 
// 			_serializeChunk(*chunk, batch);
// 
// #ifndef PUBLISH
// 			--mPending;
// #endif
// 		}
// 		list->clear();*/
// 
// 		while (!list->empty()) {
// 
// 			list->back()->setDebugDisplaySavedState(true); // we're saving this level chunk, so mark it as so
// 
// 			_serializeChunk(*list->back(), batch);
// 			list->pop_back();	//this will actually return the memory to the pool
// 
// #ifndef PUBLISH
// 			--mPending;
// #endif
// 		}
// 
// 		mStorage->_write(batch);
// 		return true;
// 	},
// 		BackgroundWorker::NOP,
// 		70);
}

void DBChunkStorage::acquireDiscarded(ChunkPtr&& ptr){
	DEBUG_ASSERT_MAIN_THREAD;

#if DISABLE_SAVING > 0
	return mParent->acquireDiscarded(std::move(ptr));
#endif

	//if this chunk is being live saved, don't save it again/destroy it
	if (mLiveChunksBeingSaved.count(ptr->getPosition()) > 0) {
		mDiscardedWhileLiveSaved[ptr->getPosition()] = std::move(ptr);
	}
	else if (ptr->getState() == ChunkState::Loaded) {
		if (ptr->isDirty()) {
			mDiscardBatch.emplace_back(std::move(ptr));

			if (!mBatch || mDiscardBatch.size() > 20) {
				_writeBatch();	//don't let the batches be too huge
			}
		}
		else {	//just "save" it by destroying it
			ptr.reset();
		}
	}
}

void DBChunkStorage::hintDiscardBatchEnd(){
	DEBUG_ASSERT(mBatch, "Not in a discard batch!");

	mBatch = false;
	if (!mDiscardBatch.empty()) {
		_writeBatch();
	}
}

// std::shared_ptr<leveldb::WriteBatch> DBChunkStorage::_getBuffer(){
// 	if (mBufferPool.empty()) {
// 		return make_shared<leveldb::WriteBatch>();
// 	}
// 	else {
// 		auto ptr = std::move(mBufferPool.back());
// 		mBufferPool.pop_back();
// 		ptr->Clear();
// 		return ptr;
// 	}
// }

bool DBChunkStorage::saveLiveChunk(LevelChunk& lc){
// 
// #if DISABLE_SAVING > 0
// 	return true;
// #endif
// 
// 	//same, but serialize on the main thread and don't acquire the chunk
// 	std::shared_ptr<leveldb::WriteBatch> batch = _getBuffer();
// 
// 	DEBUG_ASSERT(batch, "There has to be a buffer here");
// 
// 	if (mLiveChunksBeingSaved.count(lc.getPosition()) == 0) {
// 		ScopedProfile("SaveLiveChunk-serialize");
// 
// 		mLiveChunksBeingSaved.insert(lc.getPosition());
// 
// 		_serializeChunk(lc, *batch);
// 
// 		lc.setDebugDisplaySavedState(true);
// 		lc.setSaved();
// 
// 		WorkerPool::getFor(WorkerRole::Disk).queue(
// 			//only write on the helper thread
// 			[batch, this] () {
// 			ScopedProfile("SaveLiveChunk-write");
// 			mStorage->_write(*batch);
// 			return true;
// 		},
// 			[this, batch, &lc] (){
// 			ScopedProfile("putBackBuffer");
// 			//then put back the used buffer
// 			mBufferPool.emplace_back(std::move(batch));
// 			mLiveChunksBeingSaved.erase(lc.getPosition());
// 			mDiscardedWhileLiveSaved.erase(lc.getPosition());	//this deletes the chunk if it was discarded while saving
// 		},
// 			60);
// 
// 		return true;
// 	}
// 	else{
// 		return false;
// 	}

	return false;
}

void DBChunkStorage::onLowMemory() {
	mBufferPool.clear();
}

void DBChunkStorage::compact() {

// 	WorkerPool::getFor(WorkerRole::Disk).queue([this] (){
// 		ScopedProfile("compact");
// 		LOGI("Compacting LevelDB...\n");
// 		//trigger a full compaction of the database
// 		mStorage->mDb->CompactRange(nullptr, nullptr);
// 
// 		ChunkSource::compact();
// 		return true;
// 	},
// 		BackgroundWorker::NOP,
// 		10 );
}

MemoryStats DBChunkStorage::getStats() const {
	MemoryStats stats;

// 	for (auto& b : mBufferPool) {
// 		stats.totalBytes += b->Capacity();
// 		++stats.elements;
// 	}
// 
// 	for (auto& b : threadBatch) {
// 		stats.totalBytes += b->Capacity();
// 		++stats.elements;
// 	}
// 
// 	for (auto& b : threadBuffer) {
// 		stats.totalBytes += b->capacity();
// 		++stats.elements;
// 	}
// 
// 	int pendingN = mPending;
// 	LOGI("DBStorage pending chunks: %d\n", pendingN);

	return stats;
}

bool DBChunkStorage::findNearestFeature(StructureFeatureType feature, Player& player, BlockPos& pos) {
	return mParent->findNearestFeature(feature, player, pos);
}

Unique<CompoundTag> DBChunkStorage::getStructureTag() const {
	if (mParent != nullptr) {
		return mParent->getStructureTag();
	}
	return make_unique<CompoundTag>();
}

void DBChunkStorage::readStructureTag(const CompoundTag& tag) {
	if (mParent != nullptr) {
		mParent->readStructureTag(tag);
	}
}

