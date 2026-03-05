/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/
#include "Dungeons.h"

#include "world/level/chunk/LevelChunk.h"
#include "world/level/LightLayer.h"
#include "world/level/block/Block.h"
#include "world/level/Level.h"
#include "world/phys/AABB.h"
#include "world/entity/Entity.h"
#include "world/level/BlockPos.h"
#include "world/level/block/EntityBlock.h"
#include "world/level/LevelConstants.h"
#include "world/level/chunk/ChunkSource.h"
#include "world/level/levelgen/feature/Feature.h"
// #include "platform/threading/BackgroundWorker.h"
#include "world/Facing.h"
// #include "world/level/Spawner.h"
#include "util/PoolAllocator.h"
#include "world/level/BlockSource.h"
#include "world/level/biome/Biome.h"
#include "world/level/dimension/Dimension.h"
#include "world/level/Seasons.h"
#include "world/level/FoliageColor.h"

//TODO move this to another serializer/deserializer class?
#include "nbt/NbtIo.h"
// #include "world/entity/EntityFactory.h"
#include "util/StringByteInput.h"
#include "util/KeyValueInput.h"
// #include "util/PerfTimer.h"

#include "world/entity/player/Player.h"
#include "world/level/material/Material.h"
// #include "network/PacketSender.h"
// #include "network/packet/SetEntityDataPacket.h"
#include "util/StringUtils.h"
// #include "world/entity/global/LightningBolt.h"
#include "world/level/Weather.h"
// #include "world/entity/animal/Horse.h"
#include "world/entity/EntityClassTree.h"


// #include "world/redstone/circuit/CircuitSystem.h"

//the weird value of this comes from the fact that the original value was 20 for a height of 128
//this just divides it by the original number of subchunks
const float RANDOM_TICKS_PER_TICK_PER_SUBCHUNK = 20.f / (128.f / 16.f); 

static PoolAllocator levelChunkPool(sizeof(LevelChunk));
static PoolAllocator levelSubChunkPool(sizeof(LevelChunk::SubChunk));

const int LevelChunk::UPDATE_MAP_BIT_SHIFT = 4;

void* LevelChunk::operator new(size_t size) {
	DEBUG_ASSERT_MAIN_THREAD;
	DEBUG_ASSERT(size == levelChunkPool.BLOCK_SIZE, "Wrong allocation size!");
	UNUSED_PARAMETER(size);
	return levelChunkPool.get();
}

void LevelChunk::operator delete(void* p) {
	levelChunkPool.release(p);
}

bool LevelChunk::trimMemoryPool() {   
	return levelChunkPool.trim();
}

void* LevelChunk::SubChunk::operator new(size_t size) {
	DEBUG_ASSERT(size == levelSubChunkPool.BLOCK_SIZE, "Wrong allocation size!");
	UNUSED_PARAMETER(size);
	return levelSubChunkPool.get();
}

void LevelChunk::SubChunk::operator delete(void* p) {
	levelSubChunkPool.release(p);
}

bool LevelChunk::SubChunk::trimMemoryPool() {
	return levelSubChunkPool.trim();
}

LevelChunk::SubChunk::SubChunk(bool maxSkyLight) {
	memset(mBlocks.data(), 0, sizeof(mBlocks));
	memset(mData.data(), 0, sizeof(mData));
	memset(mSkyLight.data(), maxSkyLight ? INT_MAX : 0, sizeof(mSkyLight));
	memset(mBlockLight.data(), 0, sizeof(mBlockLight));
}

LevelChunk::SubChunk::SubChunk(IDataInput& stream) {
	auto format = static_cast<SubLevelChunkFormat>(stream.readByte());
	UNUSED_VARIABLE(format); //not used yet

	stream.readBytes(mBlocks.data(), sizeof(mBlocks));
	stream.readBytes(mData.data(), sizeof(mData));
	stream.readBytes(mSkyLight.data(), sizeof(mSkyLight));
	stream.readBytes(mBlockLight.data(), sizeof(mBlockLight));
}

void LevelChunk::SubChunk::serialize(IDataOutput& stream) {
	//always serialize in the current format
	stream.writeByte(enum_cast(SharedConstants::CurrentSubLevelChunkFormat));

	//write all the data as is
	stream.writeBytes(mBlocks.data(), sizeof(mBlocks));
	stream.writeBytes(mData.data(), sizeof(mData));
	stream.writeBytes(mSkyLight.data(), sizeof(mSkyLight));
	stream.writeBytes(mBlockLight.data(), sizeof(mBlockLight));
}

//template glue because VS doesn't seem to be able to convert automatically
template<size_t S>
bool isAllOne(const DataLayer<S>& buf) {
	return Util::isAllOne(buf.span());
}
template<size_t S>
bool isAllZero(const DataLayer<S>& buf) {
	return Util::isAllZero(buf.span());
}
template<typename T, size_t S>
bool isAllZero(const std::array<T, S>& buf) {
	return Util::isAllZero(buffer_span<T>(buf));
}

bool LevelChunk::SubChunk::hasBlocks() const {
	return !isAllZero(mBlocks);
}

bool LevelChunk::SubChunk::scanIsEmpty(bool hasNoSkyLight) const {
	return 
		(hasNoSkyLight ? isAllZero(mSkyLight) : isAllOne(mSkyLight)) && 
		isAllZero(mBlockLight) && 
		!hasBlocks() && 
		isAllZero(mData);
}

DataLayer<CUBIC_CHUNK_VOLUME>& LevelChunk::SubChunk::getLayer(const LightLayer& layer) {
	if (layer.isSky()) {
		return mSkyLight;
	}
	DEBUG_ASSERT(layer == LightLayer::BLOCK, "Invalid layer");
	
	return mBlockLight;
}


LevelChunk::LevelChunk(Level& level, Dimension& dimension, const ChunkPos& cp, bool readOnly)
	: mLevel(level)
	, mDimension(dimension)
	, mPosition(cp)
	, mMin(cp)
	, mMax(BlockPos(cp) + BlockPos(CHUNK_WIDTH - 1, dimension.getHeight() - 1, CHUNK_DEPTH - 1))
	, mLoadState(ChunkState::Unloaded)
	, mTerrainDataState(ChunkTerrainDataState::NoData)
	, mDebugDisplaySavedState(ChunkDebugDisplaySavedState::Generated) // generated until overridden
	, mCachedDataState(ChunkCachedDataState::NotGenerated)
	, mReadOnly(readOnly)
	, mLastTick(0)
	, mLoadedFormat(SharedConstants::CurrentLevelChunkFormat)
	, mDefaultSkyBrightness(dimension.getSkyLight()) {
	memset(mHeightmap, 0, sizeof(mHeightmap));
	memset(mBiomes, 0, sizeof(mBiomes));
	memset(mBorderBlockMap, 0, sizeof(mBorderBlockMap));

	for (auto i : range(CHUNK_WIDTH * CHUNK_WIDTH)) {
		mRainHeights[i] = -999;
	}

	//register all dirty tick counters
	for (auto&& counter : mFullChunkDirtyTicksCounters) {
		mDirtyTickCounters.push_back(&counter);
	}
}

LevelChunk::~LevelChunk() {

}

void LevelChunk::_placeBlockEntity(Unique<BlockEntity> te) {
	DEBUG_ASSERT(te, "Cannot place a null tile entity");

	mBlockEntities.emplace(ChunkBlockPos(te->getPosition()), std::move(te));

	_dirtyTicksCounter(LevelChunkDataField::BlockEntity).touch();
}

bool _needsToCreateBlockEntity(const Block* current, const Block* old) {
	return (current && current->hasBlockEntity()) &&
		(old == nullptr || (old && old->getBlockEntityType() != current->getBlockEntityType()));
}

bool _needsToRemoveBlockEntity(const Block* current, const Block* old) {
	return (old && old->hasBlockEntity()) &&
		(current == nullptr || (old->getBlockEntityType() != current->getBlockEntityType()));
}

void LevelChunk::_createBlockEntity(const BlockPos& block, BlockSource* currentSource, BlockID current, BlockID old) {

	if (current != 0) {

		auto newBlock = Block::mBlocks[current];

		//check if the block entity changed and needs to be placed
		if (_needsToCreateBlockEntity(newBlock, Block::mBlocks[old])) {	//create the block entity

			auto te = static_cast<const EntityBlock*>(newBlock)->newBlockEntity(block, current);
			auto ptr = te.get();
			_placeBlockEntity(std::move(te));

			if (currentSource) {
				currentSource->fireBlockEntityChanged(*ptr);
			}
		}
	}
}

void LevelChunk::_createSubChunk(size_t idx, bool initSkyLight) {
	while (mSubChunks.size() <= idx) {
		// SubChunks are unique pointers, but get allocated through a PoolAllocator
		mSubChunks.push_back(make_unique<SubChunk>(initSkyLight && getDimension().getSkyLight() > Brightness::MIN));
		mDirtyTickCounters.push_back(&mSubChunks.back()->mDirtyTicksCounter);
	}
}

void LevelChunk::_setLight(const ChunkBlockPos& pos, Brightness br) {

	//TODO //OPTIMIZE since 99% of the brightness updates come from LightUpdates,
	//we can remove this check here and enforce that LightUpdates cause the chunks to be created
	//if needed? not that this if is that expensive since it's false 99% of the time
	size_t idx = pos.y >> 4;
	if (mSubChunks.size() <= idx) {
		if (br == mDefaultSkyBrightness) {
			return; //just do nothing, setting default on a null chunk is a waste
		}
		_createSubChunk(idx, true);
	}

	auto& sc = mSubChunks[idx];
	sc->mSkyLight.set(SubChunkBlockPos(pos).index(), br);
}

void LevelChunk::deferLightEmitter(const BlockPos& pos) {
	//TODO let's make this a lockless stack will we?
	std::lock_guard<SpinLock> lock(mLightListLock);
	mBlockLightSources.emplace_back(pos);
}

void LevelChunk::_lightGap(BlockSource& region, const BlockPos& start) {
// 	BlockPos end(start.x, region.getHeightmap(start), start.z);
// 
// 	region.runLightUpdates(
// 		mDimension.getSkyLightLayer(),
// 		BlockPos::min(start, end),
// 		BlockPos::max(start, end)
// 	);
}

void LevelChunk::_lightGaps(BlockSource& source, const ChunkBlockPos& pos) {
	BlockPos start = pos + mMin;
	start.y = getHeightmap(pos);

	_lightGap(source, start.west());
	_lightGap(source, start.east());
	_lightGap(source, start.north());
	_lightGap(source, start.south());
}

void LevelChunk::_placeCallbacks(const ChunkBlockPos& pos, BlockID old, BlockID current, BlockSource* currentSource, Unique<BlockEntity> blockEntity) {
	BlockPos block = pos + mMin;
	if (mLoadState == ChunkState::Loaded) {	//don't do this when loading ffs
		if (currentSource) {
			DEBUG_ASSERT_MAIN_THREAD;

			int oldHeight = getHeightmap(pos);
			bool newBlocks = Block::mLightBlock[current] > 0;
			//bool oldBlocks = Block::lightBlock[old] > 0;

			if (!mDimension.hasCeiling()) {
				if (newBlocks) {//could be higher than before
					if (pos.y >= oldHeight) {
						_recalcHeight(pos.above(), currentSource);
					}
				}
				else if (pos.y >= oldHeight - 1) {	//may have destroyed the top block
					_recalcHeight(pos, currentSource);
				}
			}

			//Update border blocks
			{
				currentSource->setBorderBlock(block, Block::mBlocks[current]->isType(Block::mBorder));
			}

			//update light
// 			{
// 				currentSource->runLightUpdates(mDimension.getSkyLightLayer(), block, block);
// 				_lightGaps(*currentSource, pos);
// 				bool updateBlockLight = (Block::mLightEmission[current] != Block::mLightEmission[old]) || (Block::mLightBlock[current] != Block::mLightBlock[old]);
// 
// 				if (updateBlockLight) {
// 					currentSource->runLightUpdates(LightLayer::BLOCK, block, block);
// 				}
// 			}

			if (current && !mLevel.isClientSide()) {
				currentSource->setPlaceChunkPos(mPosition);
				Block::mBlocks[current]->onPlace(*currentSource, block);
// 				mDimension.getCircuitSystem().invalidatePos(block);
			}
		}
	}
	else if (Block::mLightEmission[current]) {	//schedule for lighting up
		deferLightEmitter(block);
	}

	if (blockEntity) {
		//DEBUG_ASSERT(current == blockEntity->getblock().id);

		BlockPos bp = pos + mMin;
		blockEntity->moveTo(bp);
		_placeBlockEntity(std::move(blockEntity));
	}
	else {
		_createBlockEntity(block, currentSource, current, old);
	}
}

void LevelChunk::_removeCallbacks(const ChunkBlockPos& pos, BlockID old, BlockID current, BlockSource* currentSource) {
	auto oldBlock = Block::mBlocks[old];
	bool removeBlockEntity = _needsToRemoveBlockEntity(Block::mBlocks[current], oldBlock);

	if (currentSource && mLoadState == ChunkState::Loaded) {
		if (old != 0) {
			DEBUG_ASSERT_MAIN_THREAD;

			BlockPos bp = pos + mMin;
			oldBlock->onRemove(*currentSource, bp);

			//manage block entities
			if (removeBlockEntity) {
				auto elem = mBlockEntities.find(pos);
				if (elem != mBlockEntities.end()) {
					auto& te = *elem->second;
					te.onRemoved(*currentSource);

					currentSource->fireBlockEntityRemoved(std::move(elem->second));

					mBlockEntities.erase(elem);

					_dirtyTicksCounter(LevelChunkDataField::BlockEntity).touch();
				}
			}
		}
	}
	else {
		if (removeBlockEntity) {
			mBlockEntities.erase(pos);
		}
	}
}

FullBlock LevelChunk::setBlockAndData(const ChunkBlockPos& pos, FullBlock block, BlockSource* issuingSource, Unique<BlockEntity> blockEntity) {
	FullBlock old = getBlockAndData(pos);

	if (old != block) {
		int slot = pos.index2D();
		if (pos.y >= mRainHeights[slot] - 1) {
			mRainHeights[slot] = -999;
		}

		auto blockIndex = SubChunkBlockPos(pos).index();
		size_t subChunkIndex = pos.y >> 4;

		//TODO don't create the chunk if setting air
		_createSubChunk(subChunkIndex, true);
		
		auto& sc = mSubChunks[subChunkIndex];

		if (old.id != block.id) {
			// TODO: rherlitz
// 			_removeCallbacks(pos, old.id, block.id, issuingSource);
			sc->mBlocks[blockIndex] = block.id;
		}

		sc->mData.set(blockIndex, block.data);

		// TODO: rherlitz
		//if (old.id != block.id) {
		//	_placeCallbacks(pos, old.id, block.id, issuingSource, std::move(blockEntity));
		//}

		if (issuingSource) {
			sc->mDirtyTicksCounter.touch();
		}
	}

	return old;
}

/*public?*/
void LevelChunk::recalcHeightmap() {
	ChunkBlockPos p(0, 0, 0);

	for (p.x = 0; p.x < CHUNK_WIDTH; p.x++) {
		for (p.z = 0; p.z < CHUNK_WIDTH; p.z++) {
			p.y = getAllocatedHeight();

			mRainHeights[p.x + (p.z << 4)] = -999;

			while (p.y > 0 && Block::mLightBlock[getBlock(p.below())] == 0) {
				p.y--;
			}
			mHeightmap[p.index2D()] = p.y;

			//also recompute light
			//TODO //LIGHT optimize
			//TODO skip empty subchunks here
			if (mDefaultSkyBrightness > Brightness::MIN) {
				auto br = Brightness::MAX;
				ChunkBlockPos cur(p.x, getAllocatedHeight(), p.z);

				do {
					auto opaque = Block::mLightBlock[getBlock(cur)];
					if (br <= opaque) { //the next block will be black, stop descending
						break;
					}
					br -= opaque;
					
					//TODO //OPTIMIZE
					_setLight(cur, br);
					--cur.y;
				} while (cur.y > 0);
			}
		}
	}
}

/*private*/
void LevelChunk::_recalcHeight(const ChunkBlockPos& start, BlockSource* source) {
	//TODO //OPTIMIZE

// 	auto index = start.y <= mMax.y ? start.index2D() : start.below().index2D();
// 	auto yOld = mHeightmap[index] & 0xff;
// 
// 	ChunkBlockPos p = start;
// 	p.y = (start.y > yOld) ? start.y : yOld;
// 
// 	p.y = std::min(p.y, getAllocatedHeight()); //start from the lowest exitsting height
// 
// 	//int idx = start.index()-p.y;
// 	while (p.y > 0 && (Block::mLightBlock[getBlock(p.below())]) == 0) {
// 		p.y--;
// 	}
// 	if (p.y == yOld) {
// 		return;
// 	}
// 
// 	const int xOffs = mMin.x + start.x;
// 	const int zOffs = mMin.z + start.z;
// 
// 	if (source) {
// 		source->_fireColumnDirty(xOffs, zOffs, p.y, yOld, Block::UPDATE_NONE);	//TODO is this the correct flag?
// 
// 	}
// 	mHeightmap[index] = std::min(p.y, (Height)mMax.y);
// 
// 	if (mDefaultSkyBrightness>Brightness::MIN) {
// 		if (p.y < yOld) {
// 			for (ChunkBlockPos tmp = start; tmp.y < yOld; tmp.y++) {
// 				//TODO //OPTIMIZE
// 				_setLight(tmp, Brightness::MAX);
// 			}
// 		}
// 		else {
// // 			if (source) {
// // 				source->runLightUpdates(mDimension.getSkyLightLayer(), BlockPos(xOffs, yOld, zOffs), BlockPos(xOffs, p.y, zOffs));
// // 			}
// 
// 			for (ChunkBlockPos tmp(start.x, yOld, start.z); tmp.y < p.y; tmp.y++) {
// 				//TODO //OPTIMIZE
// 				_setLight(tmp, Brightness::MIN);
// 			}
// 		}
// 
// 		auto br = Brightness::MAX;
// 		auto y0 = p.y;
// 
// 		while (p.y > 0 && br > 0) {
// 			p.y--;
// 			auto block = Block::mLightBlock[getBlock(p)];
// 			if (block == 0) {
// 				block = Brightness(1);
// 			}
// 			if (br <= block) {
// 				br = Brightness::MIN;
// 			}
// 			else {
// 				br -= block;
// 			}
// 			//TODO //OPTIMIZE
// 			_setLight(p, br);
// 		}
// 
// 		while (p.y > 0 && Block::mLightBlock[getBlock((ChunkBlockPos)p.below())] == 0) {
// 			p.y--;
// 		}
// 		if (p.y != y0) {
// 			if (source) {
// 				source->runLightUpdates(mDimension.getSkyLightLayer(), BlockPos(xOffs - 1, p.y, zOffs - 1), BlockPos(xOffs + 1, y0, zOffs + 1));
// 			}
// 		}
// 	}
}

bool LevelChunk::isBlockInChunk(const BlockPos& block) const {
	//only actually check x and z
	return block.x >= mMin.x && block.z >= mMin.z &&
		block.x <= mMax.x &&  block.z <= mMax.z;
}

bool LevelChunk::getBorder(const ChunkBlockPos& pos) const {
	return mBorderBlockMap[pos.index2D()] != false;
}

void LevelChunk::setBorder(const ChunkBlockPos& pos, bool val) {
#ifdef MCPE_EDU
	if (!val) {
		//checks if there are any other border blocks
		//if we have any we can't turn this off yet
		for (int i = 0; i <= getAllocatedHeight(); ++i) {
			if (i == pos.y) {
				continue;
			}

			if (getBlock(ChunkBlockPos(pos.x, i, pos.z)) == Block::mBorder->getId()) {
				return;
			}
		}

		mBorderBlockMap[pos.index2D()] = false;
	}
	else {
		mBorderBlockMap[pos.index2D()] = true;
	}
#else
	UNUSED_PARAMETER(val,pos);
#endif
}

Height LevelChunk::getHeightmap(const ChunkBlockPos& pos) const {
	return mHeightmap[pos.index2D()];
}

void LevelChunk::recalcBlockLights() {
}

Brightness LevelChunk::getBrightness(const LightLayer& layer, const ChunkBlockPos& pos) {
	if (auto l = _lightLayer(layer, pos)) {
		return (Brightness)l->get(SubChunkBlockPos(pos).index());
	}
	return layer.getSurrounding();
}

DataLayer<CUBIC_CHUNK_VOLUME>* LevelChunk::_lightLayer(const LightLayer& layer, const ChunkBlockPos& pos) {
	size_t idx = pos.y >> 4;
	if (idx >= mSubChunks.size()) {
		return nullptr;
	}

	auto& sc = mSubChunks[idx];

	if (layer.isSky()) {
		return &sc->mSkyLight;
	}
	else if (layer == LightLayer::BLOCK) {
		return &sc->mBlockLight;
	}
	
	DEBUG_FAIL("Invalid light layer");
	return nullptr;
}

bool LevelChunk::setBrightness(const LightLayer& layerID, const ChunkBlockPos& pos, Brightness brightness) {
	size_t idx = pos.y >> 4;
	if (idx >= mSubChunks.size()) { 
		if (brightness == layerID.getSurrounding()) {
			return false; //no need to do anything
		}
		_createSubChunk(idx, true);
	}

	auto& sc = *mSubChunks[idx];
	auto& layer = sc.getLayer(layerID);

	auto index = SubChunkBlockPos(pos).index();
	auto old = layer.get(index);
	if (old == brightness) {
		return false;
	}

	layer.set(index, brightness);

	// only save out chunks due to lighting that actually were previously saved, as the generated chunks will
	// get lit correctly on next load
	if (mLoadState == ChunkState::Loaded && mDebugDisplaySavedState == ChunkDebugDisplaySavedState::Saved) {
		sc.mDirtyTicksCounter.touch();
	}
	return true;
}

Brightness LevelChunk::getRawBrightness(const ChunkBlockPos& pos, Brightness skyDampen) {
	size_t idx = pos.y >> 4;
	if (idx >= mSubChunks.size()) {
		return mDefaultSkyBrightness;
	}

	auto& sc = mSubChunks[idx];
	auto blockIdx = SubChunkBlockPos(pos).index();

	auto light = (Brightness)sc->mSkyLight.get(blockIdx);
	if (light > skyDampen) {
		light -= skyDampen;
	}
	else {
		light = Brightness::MIN;
	}

	auto block = (Brightness)sc->mBlockLight.get(blockIdx);

	return std::max(block, light);
}

bool LevelChunk::hasEntity(Entity& e) {
	return OwnedEntitySet::find(mEntities, e) != mEntities.end();
}

void LevelChunk::addEntity(Unique<Entity> e) {
	DEBUG_ASSERT(e, "Cannot add a null entity");
// 	DEBUG_ASSERT(!e->hasCategory(EntityCategory::Player), "Players are managed by the level");
	DEBUG_ASSERT(!hasEntity(*e), "The chunk already contains this entity");

	e->mChunkPos = mPosition;
	mEntities.emplace(std::move(e));

	_dirtyTicksCounter(LevelChunkDataField::Entity).touch();
}

Unique<Entity> LevelChunk::removeEntity(Entity& toRemove) {

	auto itr = OwnedEntitySet::find(mEntities, toRemove);
	if (itr != mEntities.end()) {
		Unique<Entity> temp = std::move(*itr);
		mEntities.erase(itr);

		_dirtyTicksCounter(LevelChunkDataField::Entity).touch();
		return temp;
	}
	else {
		DEBUG_FAIL("The chunk doesn't contain this entity");
		return nullptr;
	}
}

// Unique<BlockEntity> LevelChunk::removeBlockEntity(const BlockPos& blockPos) {
// 	auto blockEntity = mBlockEntities.find(ChunkBlockPos(blockPos));
// 	if (blockEntity != mBlockEntities.end()) {
// 		auto tmp = std::move(blockEntity->second);
// 
// 		mBlockEntities.erase(blockEntity);
// 
// 		return tmp;
// 	}
// 
// 	return nullptr;
// }

LevelChunk::LightList && LevelChunk::moveLightSources() {
	std::lock_guard<SpinLock> lock(mLightListLock);
	return std::move(mBlockLightSources);
}

bool LevelChunk::isSkyLit(const ChunkBlockPos& pos) {
	return pos.y >= (mHeightmap[pos.index2D()] & 0xff);
}

bool LevelChunk::tryChangeState(ChunkState from, ChunkState to) {
	//try to see if we can execute this transition
// 	bool success = mLoadState.compare_exchange_strong(from, to);

// 	return success;

	mLoadState.exchange(to);
	return true;
}

void LevelChunk::changeState(ChunkState from, ChunkState to) {
#ifdef PUBLISH
	tryChangeState(from, to);
#else
	DEBUG_ASSERT(tryChangeState(from, to), "Cannot change state, when using this call the initial state should be known");
#endif
}

void LevelChunk::_changeTerrainDataState(ChunkTerrainDataState from, ChunkTerrainDataState to) {
	DEBUG_ASSERT(mTerrainDataState == from, "Initial state for changeTerrainDataState is wrong!");
	UNUSED_VARIABLE(from);
	mTerrainDataState = to;
}

void LevelChunk::getEntities(Entity* except, const AABB& bb, std::vector<Entity*>& es) {
	for (auto& e : mEntities) {
		if (e.get() != except && e->mBB.intersects(bb)) {
			es.push_back(e.get());
		}
	}
}

void LevelChunk::getEntities(EntityType type, const AABB& bb, std::vector<Entity*>& es, bool ignoreTargetType) const {
	for (auto& e : mEntities) {
		bool isOfType = EntityClassTree::isInstanceOf(*e, type);
		if (isOfType != ignoreTargetType && e->mBB.intersects(bb)) {
			es.push_back(e.get());
		}
	}
}

const OwnedEntitySet& LevelChunk::getEntities() const {
	return mEntities;
}

const LevelChunk::OwnedBlockEntityMap& LevelChunk::getBlockEntities() const {
	return mBlockEntities;
}

// BlockEntity* LevelChunk::getBlockEntity(const ChunkBlockPos& localPos) {
// 	OwnedBlockEntityMap::iterator cit = mBlockEntities.find(localPos);
// 
// 	return (cit == mBlockEntities.end()) ? nullptr : cit->second.get();
// }

bool LevelChunk::hasBlockEntity(const ChunkBlockPos& cp) {
	return mBlockEntities.find(cp) != mBlockEntities.end();
}

bool LevelChunk::setBlockExtraData(const ChunkBlockPos& localPos, uint16_t extraData) {

	int key = localPos.index();
	auto i = mBlockExtraData.find(key);

	if (extraData == 0) {
		if (i == mBlockExtraData.end()) {
			return false;	// nothing has changed

		}
		mBlockExtraData.erase(key);
	}
	else {
		// Value Changed at all?
		if (i == mBlockExtraData.end()) {
			mBlockExtraData[key] = extraData;
		}
		else if (i->second == extraData) {
			return false;	// nothing has changed
		}
		else {
			i->second = extraData;
		}
	}

	if (mLoadState == ChunkState::Loaded) {
		_dirtyTicksCounter(LevelChunkDataField::BlockExtraData).touch();
	}

	return true;
}

uint16_t LevelChunk::getBlockExtraData(const ChunkBlockPos& localPos) {

	auto cit = mBlockExtraData.find(localPos.index());
	if (cit != mBlockExtraData.end()) {
		return cit->second;
	}
	return 0;
}

bool LevelChunk::hasAnyBlockExtraData() const {
	return !mBlockExtraData.empty();
}

void memcpy_strided(void* dst, const void* src, size_t dataLen, size_t srcStride, size_t count) {
	auto d = (uint8_t*)dst;
	auto s = (uint8_t*)src;

	for (auto i : range(count)) {
		UNUSED_VARIABLE(i);

		memcpy(d, s, dataLen);
		d += dataLen;
		s += srcStride;
	}
}

template<typename T>
uint32_t findHighestNonZeroByte(buffer_span<T> ids, size_t stride, size_t count) {
	//TODO there's countless ways to make this faster
	auto ptr = reinterpret_cast<const uint8_t*>(ids.data());
	uint32_t highest = 0;

	for(auto i : range(count)) {
		UNUSED_VARIABLE(i);
		for(uint32_t j = stride - 1; j > highest; --j) {
			if(ptr[j]) {
				highest = j;
			}
		}

		ptr += stride;
	}
	return highest;
}

void LevelChunk::setAllBlockIDs(buffer_span<BlockID> ids, Height sourceColumnHeight) {
	//TODO don't copy paste this
	auto hidx = findHighestNonZeroByte(ids, sourceColumnHeight, CHUNK_COLUMNS);
	if(hidx == 0) {
		return;
	}

	_createSubChunk(hidx >> 4, false);

	auto dstStride = CHUNK_WIDTH;
	auto srcStride = sourceColumnHeight;

	auto offset = 0ul;
	for (auto&& sc : mSubChunks) {
		memcpy_strided(
			sc->mBlocks.data(),
			ids.data() + offset,
			dstStride,
			srcStride,
			CHUNK_COLUMNS
		);

		offset += dstStride;
	}
}

void LevelChunk::setAllBlockData(buffer_span<NibblePair> data, Height sourceColumnHeight) {
	//TODO don't copy paste this
	auto hidx = findHighestNonZeroByte(data, sourceColumnHeight / 2, CHUNK_COLUMNS);
	if (hidx == 0) {
		return;
	}

	_createSubChunk(hidx >> 3, false);
	auto dstStride = CHUNK_WIDTH / 2;
	auto srcStride = sourceColumnHeight / 2;

	auto offset = 0ul;
	for (auto&& sc : mSubChunks) {
		memcpy_strided(
			sc->mData.data(),
			data.data() + offset,
			dstStride,
			srcStride,
			CHUNK_COLUMNS
		);

		offset += dstStride;
	}
}

void LevelChunk::setAllSkyLight(buffer_span<NibblePair> brightness, Height sourceColumnHeight) {
	//TODO don't copy paste this
	auto hidx = findHighestNonZeroByte(brightness, sourceColumnHeight / 2, CHUNK_COLUMNS);
	if (hidx == 0) {
		return;
	}

	_createSubChunk(hidx >> 3, false);
	auto dstStride = CHUNK_WIDTH / 2;
	auto srcStride = sourceColumnHeight / 2;

	auto offset = 0ul;
	for (auto&& sc : mSubChunks) {
		memcpy_strided(
			sc->mSkyLight.data(),
			brightness.data() + offset,
			dstStride,
			srcStride,
			CHUNK_COLUMNS
		);

		offset += dstStride;
	}
}

void LevelChunk::setAllBlockLight(buffer_span<NibblePair> brightness, Height sourceColumnHeight) {
	//TODO don't copy paste this
	auto hidx = findHighestNonZeroByte(brightness, sourceColumnHeight / 2, CHUNK_COLUMNS);
	if (hidx == 0) {
		return;
	}

	_createSubChunk(hidx >> 3, false);
	auto dstStride = CHUNK_WIDTH / 2;
	auto srcStride = sourceColumnHeight / 2;

	auto offset = 0ul;
	for (auto&& sc : mSubChunks) {
		memcpy_strided(
			sc->mBlockLight.data(),
			brightness.data() + offset,
			dstStride,
			srcStride,
			CHUNK_COLUMNS
		);

		offset += dstStride;
	}
}

void LevelChunk::tick(Player* player, const Tick& tick) {
// 	DEBUG_ASSERT_MAIN_THREAD;
// 
// 	if (mReadOnly) {
// 		return;
// 	}
// 
// 	Level& level = player->getLevel();
// 	auto& random = level.getRandom();
// 	auto& tickRegion = player->getRegion();
// 
// 
// 	if (mFinalized == Finalization::NeedsInstaticking) {
// 		if (tickRegion.hasChunksAt(AABB(Vec3(mMin.x - 1.0f, (float)mMin.y, mMin.z - 1.0f), Vec3(mMax.x + 1.0f, (float)mMax.y, mMax.z + 1.0f)))) {
// 			if (!mTickQueue.tickPendingTicks(tickRegion, tick, 100, true)) {
// 				mFinalized = Finalization::NeedsPopulation;
// 			}
// 		}
// 	}
// 	else if (!mSerializedEntitiesBuffer.empty()) {
// 
// 		//deserialize the entities
// 		StringByteInput in(mSerializedEntitiesBuffer);
// 		EntityLink::List links;
// 
// 		while (in.numBytesLeft() > 0) {
// 			_deserializeEntity(tickRegion, in, links);
// 		}
// 
// 		//bind together all the entities
// 		for (auto&& link : links) {
// 
// 			//HACK some worlds can store an entity riding itself??
// 			if (link.A == link.B) {
// 				continue;
// 			}
// 
// 			//for now there's just riding
// 			auto ride = level.fetchEntity(link.A);
// 			auto rider = level.fetchEntity(link.B);
// 
// 			if (rider && ride) {
// 				rider->startRiding(*ride);
// 			}
// 		}
// 
// 		Util::freeStringMemory(mSerializedEntitiesBuffer);
// 	}
// 	else {
// 		if (mFinalized == Finalization::NeedsPopulation) {
// 			//spawn first-time mobs
// 			level.getSpawner().postProcessSpawnMobs(tickRegion, mMin.x, mMin.z, random);
// 			mDimension.getChunkSource().postProcessMobsAt(&tickRegion, mMin.x, mMin.z, random);
// 
// 			setUnsaved();	// save chunks after the first time they're ticked
// 			mFinalized = Finalization::Done;
// 		}
// 
// 		if (!level.isClientSide()) {
// 			tickBlocks(player);
// 			//tick scheduled ticks and set them dirty if anything was ticked
// 			if (mTickQueue.tickPendingTicks(tickRegion, tick)) {
// 				_dirtyTicksCounter(LevelChunkDataField::PendingTicks).touch();
// 			}
// 
// 			// We store the entities we are deleting here to do it later so we don't
// 			// try to use any entity that was deleted later.
// 			std::vector<Entity *> entitiesToDelete;
// 
// 			//tick entities in the chunk
// 			for (int i = 0; i < (int)mEntities.size(); ++i) {
// 				auto& e = mEntities[i];
// 
// 				DEBUG_ASSERT(!e->hasCategory(EntityCategory::Player), "Players are not added to chunks");
// 
// 				if (!e->isRemoved()) {
// 					if (!e->mRiding && !e->isGlobal()) {
// 						ProfileLiteCounter s(ProfilerLite::Scope::TickEntity, 1);
// 						e->tick(tickRegion);
// 					}
// 				}
// 				else {
// 					entitiesToDelete.emplace_back(e.get());
// 				}
// 			}
// 
// 			for (auto &entity : entitiesToDelete) {
// 				//garbage collect removed entities
// 				auto ptr = removeEntity(*entity);
// 				if (ptr.get() != nullptr) {
// 					level.queueEntityRemoval(std::move(ptr));	//pass the entity to the level for removing
// 				}
// 			}
// 
// 			bool anyoneListening = level.getNetEventCallback() != nullptr;
// 
// 			for (auto& e : mEntities) {
// 				if (e != nullptr) {
// 					auto& data = e->getEntityData();
// 					if (data.isDirty()) {
// 						if (anyoneListening) {
// 							SetEntityDataPacket packet(e->getRuntimeID(), data);
// 							level.getPacketSender()->send(packet);
// 						}
// 						//do this in single player as well as it's needed to save
// 						_dirtyTicksCounter(LevelChunkDataField::PendingTicks).touch();
// 					}
// 				}
// 			}
// 
// 			//send all motion packets for this chunk
// 			if (anyoneListening) {
// 				for (auto& e : mEntities) {
// 					//global entities motion packet is sent manually by level tickEntities
// 					if (e->isGlobal()) {
// 						continue;
// 					}
// 					e->sendMotionPacketIfNeeded();
// 				}
// 			}
// 		}
// 
// 		tickBlockEntities(tickRegion);
// 
// 		tickRedstoneBlocks(tickRegion);
// 
// 		tickEntitiesDirty();
// 
// 		for (auto& counter : mDirtyTickCounters) {
// 			++(*counter);
// 		}
// 	}
// 	mLastTick = tick;
}

void LevelChunk::tickRedstoneBlocks(BlockSource& tickRegion) {

// 	//only on the server
// 	if (mLevel.isClientSide() == true) {
// 		return;
// 	}
// 
// 	auto &circuit = mDimension.getCircuitSystem();
// 	circuit.updateBlocks(tickRegion, mPosition);

}

static const int MAX_BLOCKS_UPDATED_PER_TICK = 1;

BlockPos LevelChunk::findLightningTarget(const BlockPos& pos, BlockSource& region) {
	BlockPos center = region.getTopRainBlockPos(pos);

	AABB searchArea(mMin, mMax);
	std::vector<Entity*> entities;

	getEntities(EntityType::Animal, searchArea, entities);

	while (!entities.empty()) {
		return entities[region.getLevel().getRandom().nextInt(entities.size())]->getPos();
	}

	if (center.y == -1) {
		center = center.above(2);
	}

	return center;
}

static void _spawnLightning(BlockSource &region, const Vec3 &pos) {
// 	Unique<Entity> bolt = EntityFactory::createSpawnedEntity(EntityType::LightningBolt, nullptr, pos, Vec2::ZERO);
// 	if (bolt) {
// 		bolt->moveTo(pos, Vec2(0, 0));
// 		region.getLevel().addGlobalEntity(region, std::move(bolt));
// 	}
}

void LevelChunk::tickBlocks(Player* player) {
	auto& level = player->getLevel();
	auto& region = player->getRegion();
	auto& random = level.getRandom();
	auto& tickRegion = player->getRegion();

	static int _randValue = 42184323, _addend(1013904223);

	int xo = mPosition.x * CHUNK_WIDTH;
	int zo = mPosition.z * CHUNK_DEPTH;

	//@jasonmaj WEATHER - Want to move this to the weather system direction so that it can decide when lighting should
	// be spawned.
	// However, We also want the determination to be chunk based so for now it stays here.
	Weather& weather = region.getDimension().getWeather();
	float lightningLevel = weather.getLightningLevel(1);
	int odds = static_cast<int>(lightningLevel * 3000.0f + (1.0f - lightningLevel) * 100000.0f);
	if (random.nextInt(odds) == 0 && weather.isLightning()) {
		_randValue = _randValue * 3 + _addend;
		int val = (_randValue >> 2);
		BlockPos pos(xo + (val & 15), mMax.y + 1, zo + ((val >> 8) & 15));
		pos = region.getTopRainBlockPos(pos);

		_spawnLightning(region, pos);
	}
	if (weather.isRaining() && weather.isLightning() && random.nextInt(100000) == 0)
	{
		// for now, weather is not data-driven so setting the chance for skeleton trap spawns has to be manual
		Difficulty difficulty = mLevel.getDifficulty();
		bool trySpawnSkeletonTrap = false;
		switch (difficulty) {
			case Difficulty::Easy:
				trySpawnSkeletonTrap = (random.nextInt(134) == 0);	// ~0.75%
				break;
			case Difficulty::Normal:
				trySpawnSkeletonTrap = (random.nextInt(67) == 0);	// ~1.5%
				break;
			case Difficulty::Hard:
				trySpawnSkeletonTrap = (random.nextInt(45) == 0);	// ~2.25%
				break;
			default:
				break;
		};

		//if (trySpawnSkeletonTrap) {
		//	BlockPos pos(xo + (_randValue & 15), 0, zo + ((+_randValue >> 8) & 15));
		//	pos = findLightningTarget(pos, region);
		//	if (weather.isRainingAt(region, pos)) {
		//		Unique<Entity> trapHorse = EntityFactory::createSpawnedEntity(EntityDefinitionIdentifier("minecraft", EntityTypeToString(EntityType::SkeletonHorse), "minecraft:set_trap"), nullptr, pos, Vec2::ZERO);

		//		if (trapHorse != nullptr) {
		//			trapHorse->setPersistent();
		//			region.getLevel().addEntity(region, std::move(trapHorse));

		//			_spawnLightning(region, pos);
		//		}
		//	}
		//}
	}

	for (int i = 0; i < MAX_BLOCKS_UPDATED_PER_TICK; ++i) {
		_randValue = _randValue * 3 + _addend;
		int val = (_randValue >> 2);
		BlockPos pos((val & 15), mMax.y + 1, ((val >> 8) & 15));
		pos.x += xo;
		pos.z += zo;
		pos = region.getTopRainBlockPos(pos);

		if (random.nextInt(16) == 0) {
			BlockPos freezePos = pos.below();

			if (region.shouldFreezeIgnoreNeighbors(freezePos)) {
				region.removeBlock(freezePos);
				region.setBlock(freezePos.x, freezePos.y, freezePos.z, Block::mIce->mID, Block::UPDATE_CLIENTS);
			}
		}

		Biome& b = region.getBiome(pos);
		float downfallAmount = weather.getRainLevel(0) * b.getDownfall();
		if (downfallAmount > 0.0f) {

			int updateFrequency = Math::lerp(256, -256, downfallAmount);
			if (updateFrequency < 16) {
				updateFrequency = 16;
			}

			if (random.nextInt(updateFrequency) == 0) {
				weather.tryToPlaceTopSnow(region, pos, true, true);
			}

			if (b.mTemperature > Biome::RAIN_TEMP_THRESHOLD) {
				region.getBlock(pos.below()).handleRain(region, pos.below(), downfallAmount);
			}
		}
	}

	auto tickCount = static_cast<int>(RANDOM_TICKS_PER_TICK_PER_SUBCHUNK * getSubChunks().size());
	auto height = getAllocatedHeight(); 

	for (auto i : range(tickCount)) {
		UNUSED_VARIABLE(i);
		_randValue = _randValue * 3 + _addend;
		int val = (_randValue >> 2);

		ChunkBlockPos ctp(
			val & 15,
			(val >> 16) % height,
			(val >> 8) & 15);

		BlockID id = getBlock(ctp);
		if (Block::mShouldTick[id]) {
			BlockPos tp = ctp + mMin;
			Block::mBlocks[id]->tick(tickRegion, tp, random);
		}
	}
}

void LevelChunk::serialize2DMaps(IDataOutput& stream) const {
	stream.writeBytes(mHeightmap, sizeof(mHeightmap));
	stream.writeBytes(mBiomes, sizeof(mBiomes));
}

void LevelChunk::serializeBorderBlocks(IDataOutput& stream) const {
	std::vector<byte> borderBlockIndexes;
	for (int i = 0; i < CHUNK_COLUMNS; ++i) {
		if (mBorderBlockMap[i]) {
			borderBlockIndexes.push_back(i);
		}
	}
	stream.writeByte(static_cast<char>(borderBlockIndexes.size()));
	for (auto& index : borderBlockIndexes) {
		stream.writeByte(index);
	}
}

void LevelChunk::serializeTicks(IDataOutput& stream) const {
	CompoundTag tag;
// 	mTickQueue.save(tag);
	NbtIo::write(&tag, stream);
}

void LevelChunk::serializeFinalization(IDataOutput& stream) const {
	stream.writeInt(static_cast<int>(mFinalized));
}

void LevelChunk::serializeBlockEntities(IDataOutput& stream) const {
	for (auto& entry : mBlockEntities) {
		BlockEntity* te = entry.second.get();
		CompoundTag tag;

#ifdef DEBUG_STATS
		auto t = getBlock(ChunkBlockPos(te->getPosition()));
		DEBUG_ASSERT(t && Block::mBlocks[t]->hasBlockEntity(), "Saving a misplaced TE");
#endif

		if (te->save(tag)) {
			NbtIo::write(&tag, stream);	//write down the tag
		}
	}
}

void LevelChunk::serializeBlockExtraData(IDataOutput& stream) const {
	stream.writeInt(mBlockExtraData.size());
	for (auto& entry : mBlockExtraData) {
		stream.writeInt(entry.first);
		stream.writeShort(entry.second);
	}
}

void LevelChunk::serializeBiomeStates(IDataOutput& stream) const {
	// Biome's don't change their location so we can rely on the biome order being the same
	stream.writeByte(static_cast<char>(mBiomeStates.size()));
	auto b = mBiomeStates.begin(), bend = mBiomeStates.end();

	for (; b != bend; ++b) {
		stream.writeByte(b->first);
		stream.writeByte(b->second.snowLevel);
	}
}

void LevelChunk::serializeEntities(IDataOutput& stream) const {
	//for (auto& e : getEntities()) {
	//	CompoundTag tag;
	//	if (e->save(tag)) {
	//		NbtIo::write(&tag, stream);	//write down the tag
	//	}
	//}
}

// void LevelChunk::_deserializeEntity(BlockSource& source, IDataInput& stream, EntityLink::List& links) {
// 	auto et = NbtIo::read(stream);
// 	if (auto entity = EntityFactory::loadEntity(et.get())) {
// 		auto& e = *entity;
// 
// 		//	If we're attempting to deserialize an entity, we want to make sure that if it's a monster that we'll attempt
// 		//	to force load it (a.k.a. Ignore the check in level's addEntity) due to the fact that if we're in peaceful
// 		//  monsters
// 		//	won't get loaded and won't properly be removed from the level.
// 
// 		if (entity->hasCategory(EntityCategory::Monster)) {
// 			e.mForcedLoading = true;
// 		}
// 
// 		bool isAdded = false;
// 		if (e.isAutonomous()) {
// 			// Don't want to add autonomous, they're already added when the level starts
// 		}
// 		else if (e.isGlobal()) {
// 			isAdded = mLevel.addGlobalEntity(source, std::move(entity)) != nullptr;
// 		}
// 		else {
// 			isAdded = mLevel.addEntity(source, std::move(entity)) != nullptr;
// 		}
// 
// 		if (isAdded) {
// 
// 			//new graph-based code
// 			e.loadLinks(*et, links);
// 
// 			//old blobbing code
// 
// 			Entity* rider = &e;
// 			const CompoundTag* riderTag = et.get();
// 
// 			while (riderTag->contains(Entity::RIDING_TAG, ::Tag::Type::Compound)) {
// 				auto mount = EntityFactory::loadEntity(riderTag->getCompound(Entity::RIDING_TAG));
// 				auto m = mount.get();
// 				if (mount) {
// 					mLevel.addEntity(source, std::move(mount));
// 					rider->startRiding(*m);
// 				}
// 				rider = m;
// 				riderTag = riderTag->getCompound(Entity::RIDING_TAG);
// 			}
// 		}
// 	}
// 	else {
// 		// Try patching up an old serialized Entity and adding it to the level
// 		EntityFactory::fixLegacyEntity(source, et.get());
// 	}
// }

void LevelChunk::legacyDeserializeTerrain(IDataInput& stream) {

	const int LEGACY_CHUNK_HEIGHT = 128;
	const int LEGACY_CHUNK_BLOCK_COUNT = 16 * 16 * LEGACY_CHUNK_HEIGHT;

	//TODO put this memory somewhere else? This will most likely trigger a heap allocation
	//but on the other hand, it's a legacy function.

	{
		BlockID blocks[LEGACY_CHUNK_BLOCK_COUNT];
		stream.readBytes(blocks, sizeof(blocks));
		setAllBlockIDs(blocks, LEGACY_CHUNK_HEIGHT);
	}

	{
		NibblePair nibbles[LEGACY_CHUNK_BLOCK_COUNT / 2];
		stream.readBytes(nibbles, sizeof(nibbles));
		setAllBlockData(nibbles, LEGACY_CHUNK_HEIGHT);

		stream.readBytes(nibbles, sizeof(nibbles));
		setAllSkyLight(nibbles, LEGACY_CHUNK_HEIGHT);

		stream.readBytes(nibbles, sizeof(nibbles));
		setAllBlockLight(nibbles, LEGACY_CHUNK_HEIGHT);
	}

	//the heightmap was serialized at 8 bpp, upscale to 16
	uint8_t h8[CHUNK_COLUMNS];
	stream.readBytes(h8, CHUNK_COLUMNS);

	for (auto i : range(CHUNK_COLUMNS)) {
		mHeightmap[i] = h8[i];
	}

	BiomeChunkDataLegacy legacyBiomes[CHUNK_WIDTH * CHUNK_WIDTH];
	stream.readBytes(legacyBiomes, sizeof(legacyBiomes));

	for (int columnIndex : range(CHUNK_WIDTH * CHUNK_WIDTH)) {
		mBiomes[columnIndex].biome = legacyBiomes[columnIndex].biome;
	}

	checkBiomeStates();
}

void LevelChunk::deserialize2DMaps(IDataInput& stream) {
	stream.readBytes(mHeightmap, sizeof(mHeightmap));
	stream.readBytes(mBiomes, sizeof(mBiomes));

	checkBiomeStates();
}

void LevelChunk::deserialize2DMapsLegacy(IDataInput& stream) {
	BiomeChunkDataLegacy legacyBiomes[CHUNK_WIDTH * CHUNK_WIDTH];

	stream.readBytes(mHeightmap, sizeof(mHeightmap));
	stream.readBytes(legacyBiomes, sizeof(legacyBiomes));

	for (int columnIndex : range(CHUNK_WIDTH * CHUNK_WIDTH)) {
		mBiomes[columnIndex].biome = legacyBiomes[columnIndex].biome;
	}

	checkBiomeStates();
}

void LevelChunk::deserializeBlockEntities(IDataInput& stream) {

	mBlockEntities.clear();

	//deserialize block entities
	while (stream.numBytesLeft() > 0) {
		auto et = NbtIo::read(stream);
		auto e = et ? BlockEntity::loadStatic(*et) : nullptr;

		if (e) {
			//ensure that the te is valid
			auto block = Block::mBlocks[getBlock(ChunkBlockPos(e->getPosition()))];
			//DEBUG_ASSERT(block && block->hasBlockEntity(), "Misplaced tile entity!\n");
			if (block && block->hasBlockEntity()) {
				_placeBlockEntity(std::move(e));
			}
		}
		else {
			LOGW("Corrupted tile entity!\n");
		}
	}
}

void LevelChunk::deserializeBorderBlocks(IDataInput& stream) {
	byte count = stream.readByte();

	// deserialize block extra data
	while (count > 0) {
		byte index = stream.readByte();
		mBorderBlockMap[index] = 1;
		--count;
	}
}

void LevelChunk::deserializeBlockExtraData(IDataInput& stream) {
	int count = stream.readInt();
	
	// deserialize block extra data
	while (count > 0) {
		int key = stream.readInt();
		uint16_t value = stream.readShort();
		mBlockExtraData[key] = value;
		--count;
	}
}

void LevelChunk::deserializeBiomeStates(IDataInput& stream) {

	// Biome's don't change their location so we can rely on the biome order
	byte biomeStateCount = stream.readByte();
	mBiomeStates.clear();

	for (int i = 0; i < biomeStateCount; ++i) {
		byte biomeId = stream.readByte();
		byte snowLevel = stream.readByte();
		mBiomeStates[biomeId] = BiomeChunkState(snowLevel);
	}
}

void LevelChunk::deserializeTicks(IDataInput& stream) {
// 	if (auto tag = NbtIo::read(stream)) {
// 		mTickQueue.load(*tag);
// 	}
}

void LevelChunk::deserializeSubChunk(uint8_t idx, IDataInput& stream) {

	if (idx > 0) {
		_createSubChunk(idx - 1, true); //create all missing chunks under this one, should they be missing
	}

	if (mSubChunks.size() <= idx) {
		//emplace one more empty chunk to make sure it can be replaced
		mSubChunks.emplace_back();
		mDirtyTickCounters.emplace_back();
	}
	
	//finally replace the chunk at idx
	mSubChunks[idx] = make_unique<SubChunk>(stream);
	mDirtyTickCounters[enum_cast(LevelChunkDataField::_count) + idx] = &mSubChunks.back()->mDirtyTicksCounter;
}

void LevelChunk::deserializeFinalization(IDataInput& stream) {
	mFinalized = static_cast<Finalization>(stream.readInt());
}

bool LevelChunk::isSubChunkKey(const std::string& key) const {
	return
		key.size() == 2 &&
		key[1] >= 0 &&
		key[1] < (char)getMaxSubChunks();
}

bool LevelChunk::deserialize(KeyValueInput& keymap) {
	static_assert(sizeof(Tag) == 1, "The following code won't work if this isn't true");

	mFinalized = Finalization::Done; // for backwards compatibility, since older saved chunks won't have the finalization tag

	for (auto&& keyStream : keymap) {
		auto& key = keyStream.key;
		auto& stream = keyStream.stream;
		auto tag = static_cast<Tag>(key.name[0]);

		switch (tag)
		{
		case Tag::Data2D: {
			deserialize2DMaps(stream);
			break;
		}
		case Tag::Data2DLegacy: {
			deserialize2DMapsLegacy(stream);
			break;
		}
		case Tag::BlockEntity: {
			deserializeBlockEntities(stream);
			break;
		}
		case Tag::Entity: {
			//on this thread-safe method, we can only load the descriptors for entities, to load them later
			//as a bonus, they will be compressed in memory until the chunk is ticked
			//create a buffer and just load the data in there
			mSerializedEntitiesBuffer.resize(stream.numBytesLeft());

			stream.readBytes((void*)mSerializedEntitiesBuffer.data(), mSerializedEntitiesBuffer.size());
			break;
		}
		case Tag::Version: {
			mLoadedFormat = (LevelChunkFormat)stream.readByte();
			break;
		}
		case Tag::BlockExtraData: {
			deserializeBlockExtraData(stream);
			break;
		}
		case Tag::BiomeState: {
			deserializeBiomeStates(stream);
			break;
		}
		case Tag::PendingTicks: {
			deserializeTicks(stream);
			break;
		}
		case Tag::LegacyTerrain: {
			legacyDeserializeTerrain(stream);
			break;
		}
		case Tag::SubChunkPrefix: {
			if (isSubChunkKey(key.name)) {
				//note that isSubChunkKey already checks if the SubChunk ID is valid
				deserializeSubChunk(key.name[1], stream);
			}
			break;
		}
		case Tag::FinalizedState: {
			deserializeFinalization(stream);
		}
		}
	}

	//TODO is there anything that can set the chunk as corrupted? No terrain tags at all is legal now
	return true;
}

bool isTopSolid(const Material* material, bool includeWater, bool includeLeaves) {
	if (material == nullptr) {
		return false;
	}
	else if (material->isType(MaterialType::Leaves)) {
		// Leaves conditionally count as solid based on includeLeaves
		return includeLeaves;
	}
	else if (includeWater && material->isType(MaterialType::Water)) {
		return true;
	}
	return material->getBlocksMotion();
}

Height LevelChunk::getAboveTopSolidBlock(const ChunkBlockPos& start, bool dimensionShowsSky, bool includeWater, bool includeLeaves) {
	const Material* material = nullptr;
	ChunkBlockPos pos = start;

	pos.y = std::min(pos.y, getAllocatedHeight()); //start from the lowest exitsting height

	if (!dimensionShowsSky) {
		//walk down until it's out of the ceiling (eg. it started inside a topsolid block - happens in the Nether)
		for (; pos.y > 0; --pos.y) {
			material = &Block::mBlocks[getBlock(pos)]->getMaterial();
			if (!isTopSolid(material, includeWater, includeLeaves)) {
				break;
			}
		}
	}

	//now that it's out of the ceiling, run normally
	while (pos.y >= 0) {
		material = &Block::mBlocks[getBlock(pos)]->getMaterial();
		if (isTopSolid(material, includeWater, includeLeaves)) {
			break;
		}
		--pos.y;
	}

	return pos.y >= 0 ? pos.y + 1 : 0;
}

bool LevelChunk::isAABBOverlappingChunk(const BlockPos& bbmin, const BlockPos& bbmax) const {
	//only actually check x and z
	return !((mMax.x <= bbmin.x || mMin.x >= bbmax.x) || (mMax.z <= bbmin.z || mMin.z >= bbmax.z));
}

bool LevelChunk::isAABBFullyInChunk(const BlockPos& bbmin, const BlockPos& bbmax) const {
	return bbmax.x <= mMax.x && bbmin.x >= mMin.x &&
		bbmax.z <= mMax.z && bbmin.z >= mMin.z;
}

bool LevelChunk::needsSaving(int wait, int maxWait) const {
	if (mLoadState >= ChunkState::Loaded && !mReadOnly) {
		for (auto& counter : mDirtyTickCounters) {
			if (counter->getTicksSinceLastChange() >= wait || counter->getTotalDirtyTicks() > maxWait) {
				return true;
			}
		}
	}
	return false;
}

bool LevelChunk::isDirty() const {
	return needsSaving(0, 0);
}

Biome& LevelChunk::getBiome(const ChunkBlockPos& pos) const {
	auto b = Biome::getBiome(mBiomes[pos.index2D()].biome);
	DEBUG_ASSERT(b, "Corrupted biome stored in a chunk, but it should be fixed at load time!");
	return *b;
}

void LevelChunk::setBiome(const Biome& biome, const ChunkBlockPos& pos) {
	mBiomes[pos.index2D()].biome = biome.mId;

	if (biome.canHaveSnowfall()) {
		if (mBiomeStates.find(biome.mId) == mBiomeStates.end()) {
			mBiomeStates[biome.mId] = BiomeChunkState();
		}
	}
	else {
		if (mBiomeStates.size() > 0) {
			mBiomeStates.erase(biome.mId);
		}
	}
}

void LevelChunk::setGrassColor(int grassColor, const ChunkBlockPos& pos) {
	auto& block = mCachedData[pos.index2D()];
	block.grassColor = grassColor | (255 << 24);
}

int LevelChunk::getGrassColor(const ChunkBlockPos& pos) {
	auto& block = mCachedData[pos.index2D()];
	return block.grassColor;
}

bool LevelChunk::wasTickedThisTick(const Tick& tick) const {
	return !(mLastTick < tick);
}

void LevelChunk::updateLightsAndHeights(BlockSource& source) {
// 	DEBUG_ASSERT(mLoadState == ChunkState::PostProcessing, "Wrong state");
// 
// 	//block light
// 	auto lights = moveLightSources();
// 	if (!lights.empty()) {
// 		auto& lightPos = lights[0];
// 		LightUpdate l(source, LightLayer::BLOCK, lightPos, lightPos);
// 
// 		for (auto& pos : lights) {
// 			auto t = source.getBlockID(pos);
// 			auto br = Block::mLightEmission[t];
// 			source.setBrightness(LightLayer::BLOCK, pos, br);
// 
// 			l.expandToContain(pos - 15, pos + 15);
// 		}
// 
// 		l.updateFast();
// 	}
// 
// 	//re calc the heightmap and skylight
// 	ChunkBlockPos pos;
// 	int minh = INT_MAX;
// 	int maxh = INT_MIN;
// 
// 	for (pos.x = 0; pos.x < CHUNK_WIDTH; ++pos.x) {
// 		for (pos.z = 0; pos.z < CHUNK_DEPTH; ++pos.z) {
// 
// 			pos.y = mMax.y;
// 			_recalcHeight(pos);
// 
// 			int h = getHeightmap(pos);
// 			minh = std::min(h, minh);
// 			maxh = std::max(h, maxh);
// 		}
// 	}
// 
// 	LightUpdate skyUpdate(source, mDimension.getSkyLightLayer(), BlockPos(mMin.x - 15, minh - 4, mMin.z - 15), BlockPos(mMax.x + 15, maxh, mMax.z + 15));
// 	skyUpdate.updateFast();
}

const BlockPos LevelChunk::getTopRainBlockPos(const ChunkBlockPos& pos) {
	const int slot = pos.x | (pos.z << 4);
	ChunkBlockPos highest(pos.x, (Height)mRainHeights[slot], pos.z);

	if (mRainHeights[slot] == -999) {
		const Height y = getAllocatedHeight();
		highest = ChunkBlockPos(pos.x, y, pos.z);

		int h = -1;

		while (highest.y > 0 && h == -1) {
			const auto& m = Block::mBlocks[getBlock(highest)]->getMaterial();
			if (!m.getBlocksMotion() && !m.isLiquid()) {
				highest = highest.below();
			}
			else {
				h = highest.y + 1;
			}
		}
		mRainHeights[slot] = std::min(h, (int)y);
	}
	return BlockPos(mMin.x + pos.x, mRainHeights[slot], mMin.z + pos.z);
}

void LevelChunk::setPendingEntities(std::string& entitiesData) {
	std::swap(mSerializedEntitiesBuffer, entitiesData);
}

const std::atomic<ChunkState>& LevelChunk::getState() const {
	return mLoadState;
}

const ChunkTerrainDataState LevelChunk::_getTerrainDataState() const {
	return mTerrainDataState;
}


void LevelChunk::setUnsaved() {
	for (auto& counter : mDirtyTickCounters) {
		(*counter) = DirtyTicksCounter::max();
	}
}

void LevelChunk::setSaved() {
	for (auto& counter : mDirtyTickCounters) {
		(*counter) = {};
	}
}

void LevelChunk::onBlockEntityChanged() {
	_dirtyTicksCounter(LevelChunkDataField::BlockEntity).touch();
}

void LevelChunk::onLoaded(BlockSource &resource) {

	BlockPos pos;
	BlockPos levelPos(mMin);
	resource.setPlaceChunkPos(mPosition);

	ChunkBlockPos cp;
	if (!resource.getLevel().isClientSide() && (mIsLoaded == false)) {
		for (cp.x = 0; cp.x < CHUNK_WIDTH; ++cp.x) {
			pos.x = cp.x + levelPos.x;
			for (cp.z = 0; cp.z < CHUNK_DEPTH; ++cp.z) {
				pos.z = cp.z + levelPos.z;
				for (cp.y = 0; cp.y <= getAllocatedHeight(); ++cp.y) {
					pos.y = cp.y + levelPos.y;
					auto id = getBlock(cp);
					if (id != Block::mAir->mID) {
						Block::mBlocks[id]->onLoaded(resource, pos);
					}
				}
			}
		}
	}

	mIsLoaded = true;
}

bool LevelChunk::needsUpgradeFix() const {
	return mLoadedFormat == LevelChunkFormat::v9_00 || mLoadedFormat == LevelChunkFormat::v9_02;
}

LevelChunkFormat LevelChunk::getLoadedFormat() const {
	return mLoadedFormat;
}

void LevelChunk::tickBlockEntities(BlockSource& tickRegion) {
	// Make a non-owning copy of the map, because the map might be modified while being iterated (pistons)
	std::vector<std::pair<ChunkBlockPos, BlockEntity*>> blockEntities;
	blockEntities.reserve(mBlockEntities.size());
	for (auto& blockEntityPair : mBlockEntities) {
		blockEntities.emplace_back(blockEntityPair.first, blockEntityPair.second.get());
	}
	// As long as we haven't defined the order properly, then nobody should depend on it! (Just like LevelChunk ticking order)
	std::random_shuffle(blockEntities.begin(), blockEntities.end(), tickRegion.getLevel().getRandom());

	// Tick them!
	for (auto& blockEntityPair : blockEntities) {
		// WARNING: blockEntityPair might contain a bad pointer! Needs to validate it first.
		BlockEntity* maybeBadBlockEntity = blockEntityPair.second;
		auto found = mBlockEntities.find(blockEntityPair.first);
		if (found != mBlockEntities.end() && found->second.get() == maybeBadBlockEntity) {
			maybeBadBlockEntity->tick(tickRegion);
		}
	}
}

void LevelChunk::tickEntitiesDirty() {
	if (mEntities.size() > 0) {
		_dirtyTicksCounter(LevelChunkDataField::Entity).touch();
	}
}

bool LevelChunk::applySeasonsPostProcess(BlockSource& region) {

	// Should not call applySeasonsPostProcess() unless checkSeasonsPostProcessDirty() is true.
	mDimension.getSeasons().postProcess(*this, region, mPosition);

	// Update Biome States (so it's not dirty)
	for (auto&& pair : mBiomeStates) {
		Biome* biome = Biome::getBiome(pair.first);
		pair.second.snowLevel = biome->getSnowAccumulationLayers();
	}

	if (mLoadState == ChunkState::Loaded) {
		_dirtyTicksCounter(LevelChunkDataField::BiomeState).touch();
	}


	return true;
}

bool LevelChunk::checkSeasonsPostProcessDirty() {

	// over world? if not.. we don't need to apply weather in the background.
	if (getDimensionId() != DimensionId::Overworld) {
		return false;
	}

	// Compare stored biome state vs current biome state with respect to weather.

	for (auto&& pair : mBiomeStates) {
		Biome* biome = Biome::getBiome(pair.first);
		int delta = pair.second.snowLevel - biome->getSnowAccumulationLayers();
		if (delta != 0) {
			return true;
		}
	}
	return false;
}

uint64_t LevelChunk::key() const {
	// unique key, used by containers
	uint64_t key = 0xFFFFFFFF & (uint64_t)mPosition.x;
	key |= (0xFFFFFFFF & (uint64_t)mPosition.z) << 32;
	return key;
}

Tick const& LevelChunk::getLastTick() const {
	return mLastTick;
}

const BlockPos& LevelChunk::getMin() const {
	return mMin;
}

const BlockPos& LevelChunk::getMax() const {
	return mMax;
}

const ChunkPos& LevelChunk::getPosition() const {
	return mPosition;
}

const bool LevelChunk::isReadOnly() const {
	return mReadOnly;
}

ChunkSource* LevelChunk::getGenerator() const {
	return mGenerator;
}

size_t LevelChunk::countSubChunksAfterPrune() const {
	int i = mSubChunks.size() - 1;
	bool noSkyLight = mDefaultSkyBrightness == Brightness::MIN;

	for (; i >= 0; --i) {
		if(!mSubChunks[i]->scanIsEmpty(noSkyLight)) {
			break;
		}
	}
	return i + 1;
}

void LevelChunk::_setGenerator(ChunkSource* generator) {
	DEBUG_ASSERT(mGenerator == nullptr, "Cannot set the generator on a levelChunk more than once!"); // should have only one generator
	mGenerator = generator;
}

Dimension& LevelChunk::getDimension() const {
	return mDimension;
}

void LevelChunk::setFinalized(Finalization state) {
	DEBUG_ASSERT(state > mFinalized, "Can't go back in time bruh");

	mFinalized = state;
}

DimensionId LevelChunk::getDimensionId() const {
	return mDimension.getId();
}

void LevelChunk::checkBiomeStates() {
	for (auto& b : mBiomes) {
		if (Biome::getBiome(b.biome) == nullptr) {
			LOGW("Invalid biome stored in a chunk (%d)\n", b.biome);
			b.biome = Biome::DEFAULT->mId;
		}
		Biome* biome = Biome::getBiome(b.biome);
		if (biome->canHaveSnowfall()) {
			if (mBiomeStates.find(biome->mId) == mBiomeStates.end()) {
				mBiomeStates[biome->mId] = BiomeChunkState();
			}
		}
		else {
			mBiomeStates.erase(biome->mId);
		}
	}
}

bool LevelChunk::hasAnyBiomeStates() const {
	return !mBiomeStates.empty();
}

LevelChunk::DirtyTicksCounter LevelChunk::DirtyTicksCounter::max() {
	DirtyTicksCounter c;
	c.lastChange = c.totalTime = INT_MAX;
	return c;
}

LevelChunk::DirtyTicksCounter::DirtyTicksCounter()
	: totalTime(INT_MIN)
	, lastChange(0) {

}

void LevelChunk::DirtyTicksCounter::touch() {
	if (totalTime < 0) { //first change
		totalTime = 0;
	}
	lastChange = totalTime;
}

LevelChunk::DirtyTicksCounter& LevelChunk::DirtyTicksCounter::operator++() {
	Math::safeIncrement(totalTime);
	return *this;
}

int LevelChunk::DirtyTicksCounter::getTicksSinceLastChange() const {
	return totalTime - lastChange;
}

int LevelChunk::DirtyTicksCounter::getTotalDirtyTicks() const {
	return totalTime;
}

void LevelChunk::updateCachedData(BlockSource& region) {
	if (mCachedDataState == ChunkCachedDataState::NotGenerated) {

		std::lock_guard<SpinLock> lock(mCachedDataStateSpinLock);

		//try again because it might have been acquired since
		if (mCachedDataState == ChunkCachedDataState::NotGenerated) {
			Random random(getMin().hashCode());
			FoliageColor::buildGrassColor(getMin(), getMax() + 1, region, random);

			mCachedDataState = ChunkCachedDataState::Generated;
		}
	}
}
