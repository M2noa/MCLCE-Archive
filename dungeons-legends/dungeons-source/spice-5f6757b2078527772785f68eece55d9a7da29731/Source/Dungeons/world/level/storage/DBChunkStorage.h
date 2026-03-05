/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/
#pragma once

#include "common_header.h"

// #include "AppPlatformListener.h"
#include "Core/Utility/MemoryTracker.h"
// #include "platform/threading/ThreadLocal.h"
#include "world/level/chunk/ChunkSource.h"

class DBStorage;
class ChunkViewSource;
class ChunkKey;
enum class StructureFeatureType : int8_t;

namespace leveldb {
	class WriteBatch;
}

class DBChunkStorage : public ChunkSource, 
// 	public AppPlatformListener, 
	public MemoryTracker {
public:
	DBChunkStorage(Unique<ChunkSource> parent, DBStorage* storage);

	virtual ~DBChunkStorage();

	virtual void loadChunk(LevelChunk& c, bool forceImmediateReplacementDataLoad) override;

	virtual bool postProcess(ChunkViewSource& neighborhood) override;

	///save a chunk that is still owned by the main thread
	virtual bool saveLiveChunk(LevelChunk& lc) override;

	virtual void acquireDiscarded(ChunkPtr&& lc) override;

	virtual void hintDiscardBatchBegin() override;
	virtual void hintDiscardBatchEnd() override;

	virtual void onLowMemory() /*override*/;

	virtual void compact() override;

	virtual void waitDiscardFinished() override;

 	virtual MemoryStats getStats() const override;

	virtual bool findNearestFeature(StructureFeatureType feature, Player& player, BlockPos& pos) override;

	Unique<CompoundTag> getStructureTag() const override;
	void readStructureTag(const CompoundTag& tag) override;

private:

	bool _replaceChunkDataIfSaved(LevelChunk& lc, ChunkViewSource& neighborhood);
	bool _loadChunkFromDB(LevelChunk& lc);
	bool _hasChunk(const ChunkKey &key);

// 	static ThreadLocal<leveldb::WriteBatch> threadBatch;
// 	static ThreadLocal<std::string> threadBuffer;

	DBStorage* mStorage;
	std::vector<std::shared_ptr<leveldb::WriteBatch>> mBufferPool;
	std::vector<ChunkPtr> mDiscardBatch;
	std::unordered_set<ChunkPos> mLiveChunksBeingSaved;
	std::unordered_map<ChunkPos, ChunkPtr> mDiscardedWhileLiveSaved;	//TODO maybe we should just have true refcounting?
	bool mBatch = false;

// 	std::shared_ptr<leveldb::WriteBatch> _getBuffer();

	void _writeBatch();
	void _serializeChunk(const LevelChunk& lc, leveldb::WriteBatch& batch);

	std::atomic<int> mPending;
};
