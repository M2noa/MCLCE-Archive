/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/
#pragma once

#include "common_header.h"

#include "util/GridArea.h"
#include "CommonTypes.h"

#include "world/level/chunk/ChunkRefCount.h"
#include "world/level/biome/MobSpawnerData.h"
#include "world/level/levelgen/structure/StructureFeatureType.h"
#include "ChunkSourceView.h"

class LevelChunk;
class Level;
class ChunkViewSource;
class BlockSource;
class BlockVolume;
class Dimension;
class CompoundTag;

class ChunkSource {
public:

	static std::string getFeatureName(StructureFeatureType f);
	static StructureFeatureType getFeatureId(const std::string& s);

	enum class LoadMode {
		None,
		Deferred
	};

	typedef Unique<LevelChunk> ChunkPtr;
	typedef std::vector<ChunkPtr> ChunkPtrList;
	typedef std::vector<MobSpawnerData> MobList;

	ChunkSource(Level* level, Dimension* dimension, int side);
	explicit ChunkSource(ChunkSource& parent);
	explicit ChunkSource(Unique<ChunkSource> parent);

	virtual ~ChunkSource();

	Level& getLevel() const;
	Dimension& getDimension() const;
	ChunkSource* getParent() const;
	int getChunkSide() const;

	///gets a cached chunk from this level
	LevelChunk* getAvailableChunk(const ChunkPos& cp);
	LevelChunk* getGeneratedChunk(const ChunkPos& cp);
	LevelChunk* getAvailableChunkAt(const BlockPos& pos);
	virtual LevelChunk* getExistingChunk(const ChunkPos& cp);
	virtual LevelChunk* requestChunk(const ChunkPos& cp, LoadMode lm);
	virtual bool releaseChunk(LevelChunk& lc);
	LevelChunk* getOrLoadChunk(const ChunkPos& cp, LoadMode lm);

	virtual bool postProcess(ChunkViewSource& neighborhood);

	///loads data into a new chunk
	virtual void loadChunk(LevelChunk& c, bool forceImmediateReplacementDataLoad);

	virtual void postProcessMobsAt(BlockSource* blockSource, int chunkWestBlock, int chunkNorthBlock, Random& random);

	virtual const MobList& getMobsAt(BlockSource& blockSource, EntityType category, const BlockPos& pos);

	virtual Unique<CompoundTag> getStructureTag() const = 0;
	virtual void readStructureTag(const CompoundTag& tag) = 0;

	///save a chunk that is still being used on the main thread (do something to sync!
	virtual bool saveLiveChunk(LevelChunk& lc);

	virtual void hintDiscardBatchBegin();
	virtual void hintDiscardBatchEnd();

	virtual void acquireDiscarded(ChunkPtr&& ptr);

	virtual const ChunkRefCount::Map& getStoredChunks() const;
	// Non-const version for writing chunks
	virtual ChunkRefCount::Map& getStoredChunks();

	virtual void compact();

	virtual void waitDiscardFinished();

	virtual ChunkSourceView getView(
		const BlockPos& min,
		const BlockPos& max,
		LoadMode lm,
		bool circle,
		const ChunkSourceView::AddCallback& add = ChunkSourceView::AddCallback());

	ChunkSourceView getView(
		const BlockPos& pos,
		int radius,
		LoadMode lm,
		bool circle,
		const ChunkSourceView::AddCallback& add = ChunkSourceView::AddCallback());

	void discard(ChunkPtr& ptr);
	void discard(LevelChunk& ptr);

	virtual bool findNearestFeature(StructureFeatureType feature, Player& player, BlockPos& pos);
	virtual void prepareHeights(BlockVolume& box, int xOffs, int zOffs);

protected:

	int mChunkSide;

	Level* mLevel = nullptr;
	Dimension* mDimension = nullptr;
	ChunkSource* mParent = nullptr;
	Unique<ChunkSource> mOwnedParent;

	void _startPostProcessingArea(LevelChunk& center);
	void _startPostProcessing(LevelChunk& lc);
	int _getChunkPriority(const LevelChunk& lc);

	void _fireChunkLoaded(LevelChunk& lc);

};
