/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/
#pragma once

#include "common_header.h"

#include "CommonTypes.h"
#include "world/level/chunk/DataLayer.h"
#include "world/level/LevelConstants.h"
#include "world/level/BlockPos.h"
#include "world/level/block/entity/BlockEntity.h"
#include "world/level/ChunkPos.h"
#include "world/level/SubChunkBlockPos.h"
#include "world/level/Tick.h"
#include "world/level/ChunkBlockPos.h"
#include "world/level/chunk/ChunkState.h"
#include "world/level/chunk/LevelChunkFormat.h"
//#include "world/entity/EntityLink.h"
#include "world/entity/EntityTypes.h"
//#include "world/level/BlockTickingQueue.h"
#include "util/SpinLock.h"
#include "Core/Utility/buffer_span.h"
#include "util/NibblePair.h"
#include "Core/Utility/static_vector.h"
#include "world/entity/Entity.h"

class Level;
class Dimension;
class LightLayer;
class AABB;
class BlockSource;
class ChunkSource;
class IDataOutput;
class IDataInput;
class KeyValueInput;
class Biome;
class BlockTickingQueue;

const auto CUBIC_CHUNK_VOLUME = CHUNK_WIDTH * CHUNK_WIDTH *CHUNK_WIDTH;

const uint32_t MAX_CHUNK_HEIGHT = 256; //TODO this is only needed to preallocate subchunk slots inline... if there was another way it wouldn't be needed

struct BiomeChunkData {
	unsigned char biome;
};

struct BiomeChunkDataLegacy {
	unsigned char biome, r, g, b;
};

// Not serialized
struct ColumnCachedData {
	int grassColor;
};

struct BiomeChunkState {
	byte snowLevel;
	BiomeChunkState(byte snowLevel = 0) 
		: snowLevel(snowLevel) {
	}

};

//the types of the data that is serialized in the underlying Key-value storage as different keys
enum class LevelChunkDataField {
	BiomeState,
	BlockEntity,
	BlockExtraData,
	Entity,
	PendingTicks,

	_count
};

class LevelChunk {
public:

	struct DirtyTicksCounter {
		static DirtyTicksCounter max();

		DirtyTicksCounter();

		void touch();

		DirtyTicksCounter& operator++();

		int getTicksSinceLastChange() const;
		int getTotalDirtyTicks() const;

		bool isDirty() const {
			return totalTime >= 0;
		}

	protected:
		int totalTime, lastChange;
	};

	struct SubChunk {
		std::array<BlockID, CUBIC_CHUNK_VOLUME> mBlocks;
		DataLayer<CUBIC_CHUNK_VOLUME>
			mData,
			mSkyLight,
			mBlockLight;

		DirtyTicksCounter mDirtyTicksCounter;

		SubChunk(bool maxSkyLight);
		explicit SubChunk(IDataInput& stream);
		SubChunk(const SubChunk&) = delete;
		SubChunk& operator=(const SubChunk&) = delete;

		DataLayer<CUBIC_CHUNK_VOLUME>& getLayer(const LightLayer& layer);

		bool scanIsEmpty(bool hasNoSkyLight) const;
		bool hasBlocks() const;

		void serialize(IDataOutput& stream);

		static void* operator new(size_t size);
		static void operator delete (void* p);
		static bool trimMemoryPool();
	};

	enum class Finalization {
		NeedsInstaticking,
		NeedsPopulation,
		Done
	};

	typedef std::vector<BlockPos> LightList;
	typedef std::vector<BlockPos> BlockList;

	typedef std::unordered_map<ChunkBlockPos, Unique<BlockEntity> > OwnedBlockEntityMap;

	static void* operator new(size_t size);
	static void operator delete (void* p);
	static bool trimMemoryPool();

	//WARNING: SERIALIZED
	// the numeric order of the tags is important as tags are loaded in that order.
	// Tags have dependencies on each other so for example, Data2D and SubChunks need to be loaded
	// before entities.
	enum class Tag : char {
		Data2D = 45,
		Data2DLegacy,
		SubChunkPrefix,
		LegacyTerrain,
		BlockEntity,
		Entity,
		PendingTicks,
		BlockExtraData,
		BiomeState,
		FinalizedState,
		Version = 118
	};

	LevelChunk(Level& level, Dimension& dimension, const ChunkPos& cp, bool readOnly = false);
	LevelChunk(const LevelChunk&) = delete;

	~LevelChunk();

	bool isBlockInChunk(const BlockPos& block) const;
	bool isAABBOverlappingChunk(const BlockPos& bbmin, const BlockPos& bbmax) const;
	bool isAABBFullyInChunk(const BlockPos& bbmin, const BlockPos& bbmax) const;

	///tells if the chunk needs to/can be saved
	bool needsSaving(int wait, int maxWait) const;
	bool isDirty() const;

	void setUnsaved();
	void setSaved();

	void onBlockEntityChanged();
	//Entities are assumed dirty
	void onLoaded(BlockSource &region);

	Height getHeightmap(const ChunkBlockPos& pos) const;

	bool getBorder(const ChunkBlockPos& pos) const;
	void setBorder(const ChunkBlockPos& pos, bool val);

	void recalcHeightmap();

	//BlockTickingQueue& getTickQueue() {
	//	return mTickQueue;
	//}

	//const BlockTickingQueue& getTickQueue() const {
	//	return mTickQueue;
	//}

	Brightness getBrightness(const LightLayer& layer, const ChunkBlockPos& pos);
	bool setBrightness(const LightLayer& layer, const ChunkBlockPos& pos, Brightness brightness);
	Brightness getRawBrightness(const ChunkBlockPos& pos, Brightness skyDampen);
	void addEntity(Unique<Entity> e);
	bool hasEntity(Entity& e);

	Unique<Entity> removeEntity(Entity& e);
	//Unique<BlockEntity> removeBlockEntity(const BlockPos& blockPos);

	LightList && moveLightSources();

	//
	// BlockEntity
	//
	//BlockEntity* getBlockEntity(const ChunkBlockPos& localPos);
	
	bool hasBlockEntity(const ChunkBlockPos& cp );

	//
	// BlockExtraData
	//
	bool setBlockExtraData(const ChunkBlockPos& localPos, uint16_t extraData);
	uint16_t getBlockExtraData(const ChunkBlockPos& localPos);

	bool hasAnyBlockExtraData() const;

	void setAllBlockIDs(buffer_span<BlockID> ids, Height sourceColumnHeight);
	void setAllSkyLight(buffer_span<NibblePair> brightness, Height sourceColumnHeight);
	void setAllBlockLight(buffer_span<NibblePair> brightness, Height sourceColumnHeight);
	void setAllBlockData(buffer_span<NibblePair> data, Height sourceColumnHeight);

	bool isSkyLit(const ChunkBlockPos& pos);

	void deferLightEmitter(const BlockPos& pos);

	void recalcBlockLights();

	const OwnedEntitySet& getEntities() const;
	const OwnedBlockEntityMap& getBlockEntities() const;

	void getEntities(Entity* except, const AABB& bb, std::vector<Entity*>& es);
	void getEntities(EntityType type, const AABB& bb, std::vector<Entity*>& es, bool ignoreTargetType = false) const;

	BlockID getBlock(const ChunkBlockPos& pos) const {
		size_t idx = pos.y >> 4;
		if (idx >= mSubChunks.size()) {
			return BlockID::AIR;
		}
		return mSubChunks[idx]->mBlocks[SubChunkBlockPos(pos).index()];
	}

	DataID getData(const ChunkBlockPos& pos) const {
		size_t idx = pos.y >> 4;
		if (idx >= mSubChunks.size()) {
			return 0;
		}
		return mSubChunks[idx]->mData.get(SubChunkBlockPos(pos).index());
	}

	FullBlock getBlockAndData(const ChunkBlockPos& pos) const {
		size_t idx = pos.y >> 4;
		if (idx >= mSubChunks.size()) {
			return FullBlock::AIR;
		}
		auto blockIdx = SubChunkBlockPos(pos).index();
		return{
			mSubChunks[idx]->mBlocks[blockIdx],
			mSubChunks[idx]->mData.get(blockIdx)
		};
	}

	FullBlock setBlockAndData(const ChunkBlockPos& pos, FullBlock block, BlockSource* issuingSource = nullptr, Unique<BlockEntity> blockEntity= nullptr);

	bool isSubChunkKey(const std::string& key) const;

	void serialize2DMaps(IDataOutput& stream) const;
	void serializeBorderBlocks(IDataOutput& stream) const;
	void serializeEntities(IDataOutput& stream) const;
	void serializeBlockEntities(IDataOutput& stream) const;
	void serializeBlockExtraData(IDataOutput& stream) const;
	void serializeBiomeStates(IDataOutput& stream) const;
	void serializeTicks(IDataOutput& stream) const;
	void serializeFinalization(IDataOutput& stream) const;
	void deserialize2DMaps(IDataInput& stream);
	void deserialize2DMapsLegacy(IDataInput& stream);
	void deserializeBlockEntities(IDataInput& stream);
	void deserializeBorderBlocks(IDataInput& stream);
	void deserializeBlockExtraData(IDataInput& stream);
	void deserializeBiomeStates(IDataInput& stream);
	void deserializeTicks(IDataInput& stream);
	void deserializeSubChunk(uint8_t idx, IDataInput& stream);
	void deserializeFinalization(IDataInput& stream);
	bool deserialize(KeyValueInput& keymap);

	void legacyDeserializeTerrain(IDataInput& stream);

	void setPendingEntities(std::string& entitiesData);

	const std::atomic<ChunkState>& getState() const;

	bool tryChangeState(ChunkState from, ChunkState to);

	void changeState(ChunkState from, ChunkState to);

	///after the post processing, if there's still a step to run in the first tick the chunk will be unfinalized -
	// finalize after done
	void setFinalized(Finalization state);

	void tick(Player* player, const Tick& tick );
	void tickBlocks(Player* player);

	BlockPos findLightningTarget(const BlockPos& pos, BlockSource& region);

	void tickBlockEntities(BlockSource& tickRegion);
	//Assume entities are dirty
	void tickEntitiesDirty();
	void tickRedstoneBlocks(BlockSource& tickRegion);
	bool applySeasonsPostProcess(BlockSource& region);
	bool checkSeasonsPostProcessDirty();

	Biome& getBiome(const ChunkBlockPos& pos) const;
	void setBiome(const Biome& biome, const ChunkBlockPos& pos);

	Height getAboveTopSolidBlock(const ChunkBlockPos& pos, bool dimensionShowsSky, bool includeWater = false, bool includeLeaves = false);
	void setGrassColor(int grassColor, const ChunkBlockPos& pos);
	int getGrassColor(const ChunkBlockPos& pos);

	bool wasTickedThisTick(const Tick& tick) const;

	///constructs the heightmap and builds lighting, to be called during post processing after features have been
	// applied
	void updateLightsAndHeights(BlockSource& source);
	const BlockPos getTopRainBlockPos(const ChunkBlockPos& pos);

	///return true if the current version of this chunk needs some fix to be brought to current
	bool needsUpgradeFix() const;

	LevelChunkFormat getLoadedFormat() const;

	DimensionId getDimensionId() const;

	void checkBiomeStates();
	bool hasAnyBiomeStates() const;

	uint64_t key() const;

	Tick const& getLastTick() const;

	const BlockPos& getMin() const;
	const BlockPos& getMax() const;
	const ChunkPos& getPosition() const;
	const bool isReadOnly() const;

	ChunkSource* getGenerator() const;

	Dimension& getDimension() const;

	void setDebugDisplaySavedState(bool saved) { 
		if (saved == true) {
			mDebugDisplaySavedState = ChunkDebugDisplaySavedState::Saved;
		}
		else {
			mDebugDisplaySavedState = ChunkDebugDisplaySavedState::Generated;
		}
	}

	ChunkDebugDisplaySavedState getDebugDisplaySavedState() {
		return mDebugDisplaySavedState;
	}

	ChunkCachedDataState getCachedDataState() const {
		return mCachedDataState;
	}
	void resetCachedDataState() {
		mCachedDataState = ChunkCachedDataState::NotGenerated;
	}

	void updateCachedData(BlockSource& region);

	const DirtyTicksCounter& getDirtyTicksCounter(LevelChunkDataField counter) const {
		return *mDirtyTickCounters[enum_cast(counter)];
	}

	DirtyTicksCounter& getDirtyTicksCounter(LevelChunkDataField counter) {
		return *mDirtyTickCounters[enum_cast(counter)];
	}

	buffer_span<std::unique_ptr<SubChunk>> getSubChunks() const {
		return mSubChunks;
	}

	///finds how many chunks would be left if the chunk was pruned
	size_t countSubChunksAfterPrune() const;

	const size_t getMaxSubChunks() const {
		return (mMax.y + 1) / CHUNK_WIDTH;
	}

	void _setGenerator(ChunkSource* generator);

	Height getAllocatedHeight() const {
		return mSubChunks.size() * CHUNK_WIDTH;
	}

	const ChunkTerrainDataState _getTerrainDataState() const;
	void _changeTerrainDataState(ChunkTerrainDataState from, ChunkTerrainDataState to);

protected:

	//statics
	static const int UPDATE_MAP_BIT_SHIFT;

	// constant properties
	Level& mLevel;
	Dimension& mDimension;

	const BlockPos mMin, mMax;
	const ChunkPos mPosition;

	const bool mReadOnly;

	ChunkSource* mGenerator = nullptr;

	//loading time stuff
	LevelChunkFormat mLoadedFormat;

	SpinLock mLightListLock;
	LightList mBlockLightSources;
	std::string mSerializedEntitiesBuffer;

	//runtime stuff
	std::atomic<ChunkState> mLoadState;
	ChunkTerrainDataState mTerrainDataState;
	ChunkDebugDisplaySavedState mDebugDisplaySavedState; // is this chunk saved to the DB? Used by the debug display for chunks
	ChunkCachedDataState mCachedDataState;
	SpinLock mCachedDataStateSpinLock;

	Tick mLastTick;
	//BlockTickingQueue mTickQueue;
	
	static_vector<std::unique_ptr<SubChunk>, MAX_CHUNK_HEIGHT / CHUNK_WIDTH> mSubChunks;
	const Brightness mDefaultSkyBrightness;

	BiomeChunkData mBiomes[CHUNK_WIDTH * CHUNK_WIDTH];
	ColumnCachedData mCachedData[CHUNK_WIDTH * CHUNK_WIDTH];
	Height mHeightmap[CHUNK_COLUMNS];
	std::unordered_map<byte, BiomeChunkState> mBiomeStates;

	typedef uint8_t BBorder;
	BBorder mBorderBlockMap[CHUNK_COLUMNS];
	Finalization mFinalized = Finalization::NeedsInstaticking;
	bool mIsLoaded = false;

	DirtyTicksCounter mFullChunkDirtyTicksCounters[enum_cast(LevelChunkDataField::_count)];
	static_vector<DirtyTicksCounter*, enum_cast(LevelChunkDataField::_count) + (MAX_CHUNK_HEIGHT / CHUNK_WIDTH)> mDirtyTickCounters;

	Height mRainHeights[CHUNK_WIDTH * CHUNK_WIDTH];

	OwnedEntitySet mEntities;
	OwnedBlockEntityMap mBlockEntities;
	std::unordered_map<int, uint16_t> mBlockExtraData;

	void _lightGap(BlockSource& region, const BlockPos& start );
	void _lightGaps(BlockSource& source, const ChunkBlockPos& pos);

	void _recalcHeight(const ChunkBlockPos& startPos, BlockSource* source = nullptr);

	//void _deserializeEntity(BlockSource& source, IDataInput& stream, EntityLink::List& links);

	void _placeBlockEntity(Unique<BlockEntity> te);

	void _placeCallbacks(const ChunkBlockPos& pos, BlockID old, BlockID current, BlockSource* currentSource, Unique<BlockEntity> blockEntity);
	void _removeCallbacks(const ChunkBlockPos& pos, BlockID old, BlockID current, BlockSource* currentSource);

	void _createBlockEntity(const BlockPos& block, BlockSource* currentSource, BlockID current, BlockID old);

	void _createSubChunk(size_t idx, bool initSkyLight);
	
	//TODO //OPTIMIZE remove this and set the light directly on the SubChunk. The lighting algorithms should be subchunk-aware
	void _setLight(const ChunkBlockPos& pos, Brightness br);

	DataLayer<CUBIC_CHUNK_VOLUME>* _lightLayer(const LightLayer& layer, const ChunkBlockPos& pos);

	DirtyTicksCounter& _dirtyTicksCounter(LevelChunkDataField counter) {
		return *mDirtyTickCounters[enum_cast(counter)];
	}
};
