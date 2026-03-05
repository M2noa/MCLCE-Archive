/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/

#include "Dungeons.h"

#include "world/level/BlockSource.h"

#include "world/entity/Entity.h"
#include "world/entity/EntityClassTree.h"
#include "world/entity/player/Player.h"
#include "world/level/biome/Biome.h"
#include "world/level/block/Block.h"
#include "world/level/block/entity/BlockEntity.h"
#include "world/level/block/FireBlock.h"
#include "world/level/block/LiquidBlock.h"
#include "world/level/block/TopSnowBlock.h"
// #include "world/level/BlockTickingQueue.h"
#include "world/level/chunk/ChunkSource.h"
#include "world/level/chunk/LevelChunk.h"
#include "world/level/chunk/PlayerChunkSource.h"
#include "world/level/ChunkPos.h"
#include "world/level/dimension/Dimension.h"
#include "world/level/Level.h"
// #include "world/level/LightLayer.h"
#include "world/level/material/Material.h"
#include "world/level/Weather.h"
#include "world/phys/Vec3.h"
#include "chunk/EmptyChunkSource.h"

#ifndef PUBLISH
#define DEBUG_ASSERT_OWNER_THREAD DEBUG_ASSERT(isOwnerThread(), "ERROR, this is running on the wrong thread");
#else
#define DEBUG_ASSERT_OWNER_THREAD {}
#endif

//TODO assert false if chunks can't be loaded, instead of returning invalid values

BlockSource::BlockSource(Level& level, Dimension& dimension, ChunkSource& source, bool publicSource, bool allowUnpopulatedChunks) :
	mChunkSource(source)
	, mLevel(level)
	, mDimension(dimension)
	, mAllowUnpopulatedChunks(allowUnpopulatedChunks)
// 	, mTickQueue(nullptr)
	, mPublicSource(publicSource)
	, mOwnerThreadID(std::this_thread::get_id())
	, mMaxHeight(dimension.getHeight()) {

	if (publicSource) {
		addListener(level);

		for (size_t i = 0; i < mListeners.size(); ++i) {
			mListeners[i]->onSourceCreated(*this);
		}
	}
}

BlockSource::BlockSource(Level& level, Dimension& dimension)
	: mAllowUnpopulatedChunks(false)
	, mDimension(dimension)
	, mLevel(level)
	, mMaxHeight(LEVEL_HEIGHT_DEPRECATED)
	, mPublicSource(false)
	, mChunkSource(*(new EmptyChunkSource(&level, &dimension)))
{
	// Mock
}

BlockSource::~BlockSource() {
	for(size_t i = 0; i < mListeners.size(); ++i) {
		mListeners[i]->onSourceDestroyed(*this);
	}
}

Level& BlockSource::getLevel() const {
	DEBUG_ASSERT_MAIN_THREAD;	//if someone gets the level on another thread he's up to no good
	DEBUG_ASSERT(mPublicSource, "Cannot get the level on a temporary tileSource");
	return mLevel;
}

Level& BlockSource::getLevel() {
	return mLevel;
}

ChunkSource& BlockSource::getChunkSource() {
	return mChunkSource;
}

//also checks the given blockPos permission since we're about to destroy that block
bool BlockSource::checkBlockDestroyPermissions(Entity& entity, const BlockPos& block, bool generateParticle) {
	return false;

// 	if (entity.isWorldBuilder()) {
// 		return true;
// 	}
// 
// 	bool permission = checkBlockPermissions(entity, block, generateParticle);
// 	if (permission && getBlock(block).hasProperty(BlockProperty::RequiresWorldBuilder)) {
// 		if (generateParticle) {
// 			Vec3 generateAt(block.x + 0.5f, block.y + 0.5f, block.z + 0.5f);
// 			//only render for air blocks, otherwise it looks strange and zfights
// 			mLevel.denyEffect(generateAt);
// 		}
// 		return false;
// 	}
// 
// 	return permission;
}

//doesn't check against the block pos permissions because placing on block faces is permitted
bool BlockSource::checkBlockPermissions(Entity& entity, const BlockPos& block, bool generateParticle) {
	return false;

// 	if (entity.isWorldBuilder()) {
// 		return true;
// 	}
// 	bool ret = _getBlockPermissions(block, !mLevel.getAdventureSettings().immutableWorld);
// 	if (!ret) {
// 		if (generateParticle) {
// 			Vec3 generateAt(block.x + 0.5f, block.y + 0.5f, block.z + 0.5f);
// 			//only render for air blocks, otherwise it looks strange and zfights
// 			mLevel.denyEffect(generateAt);
// 		}
// 	}
// 	return ret;
}

bool BlockSource::_getBlockPermissions(const BlockPos& pos, bool currentState) {
	//Check against border blocks
	if (pos.y >= mMaxHeight || hasBorderBlock(pos)) {
		return false;
	}

	//check all blocks in the column
	BlockPos posCheck(pos);
	while (posCheck.y >= 0) {
		const Block& block = getBlock(posCheck);

		//first one below the block dictates our range type
		if (block.getMaterial().isType(MaterialType::Allow)) {
			return true;
		}
		else if (block.getMaterial().isType(MaterialType::Deny)) {
			return false;
		}
		--posCheck.y;
	}

	return currentState;
}

const Level& BlockSource::getLevelConst() const {
	//this one can actually be called from other threads
	return mLevel;
}

Dimension& BlockSource::getDimension() const {
	DEBUG_ASSERT_MAIN_THREAD;	//if someone gets the dimensino on another thread he's up to no good
	DEBUG_ASSERT(mPublicSource, "Cannot get the dimension on a temporary tileSource");
	return mDimension;
}

Dimension& BlockSource::getDimension() {
	return mDimension;
}

DimensionId BlockSource::getDimensionId() const {
	return mDimension.getId();
}

const Dimension& BlockSource::getDimensionConst() const {
	//this one can actually be called from other threads
	return mDimension;
}

void BlockSource::removeListener(Listener& l) {
	// Only remove a listener if it does not exist
	auto it = std::find(mListeners.begin(), mListeners.end(), &l);
	if (it != mListeners.end()) {
		mListeners.erase(it);
	}
}

LevelChunk* BlockSource::getChunk(int x, int z) {
	return getChunk(ChunkPos(x, z));
}

//BlockTickingQueue* BlockSource::getTickQueue(const BlockPos& forBlock) {
//	if (mTickQueue) {
//		return mTickQueue;
//	}
//	else if (auto c = getChunk((ChunkPos)forBlock)) {
//		return &c->getTickQueue();
//	}
//	else {
//		DEBUG_FAIL("This shouldn't happen, ticking an unloaded tile");
//		return nullptr;
//	}
//}

void BlockSource::addListener(Listener& l) {
	// If the listener does not already exist, add it the listeners
	if (std::find(mListeners.begin(), mListeners.end(), &l) == mListeners.end()) {
		mListeners.push_back(&l);
}
}

// void BlockSource::setTickingQueue(BlockTickingQueue& queue) {
// 	mTickQueue = &queue;
// }

LevelChunk* BlockSource::getChunk(const ChunkPos& pos){
	//DEBUG_ASSERT_OWNER_THREAD;

	//use a single-entry cache for the chunk to avoid even to use the source
	if (mLastChunk && mLastChunk->getPosition() == pos ) {
		return mLastChunk;
	}

	auto lc = mAllowUnpopulatedChunks ? mChunkSource.getGeneratedChunk(pos) : mChunkSource.getAvailableChunk(pos);

	if (lc) {
		mLastChunk = lc;
	}

	return lc;
}

LevelChunk* BlockSource::getWritableChunk(const ChunkPos& pos){
	auto lc = getChunk(pos);
	return (!lc || lc->isReadOnly()) ? nullptr : lc;

}

bool BlockSource::shouldFireEvents(LevelChunk& c) const {
	return !mAllowUnpopulatedChunks && c.getState() == ChunkState::Loaded;
}

bool BlockSource::hasBlock(const BlockPos& pos) {
	LevelChunk* lc = getChunkAt(pos);
	// Returns false for chunks outside in limited worlds
	return lc != nullptr && lc->getPosition() != ChunkPos::INVALID && !lc->isReadOnly();
}

bool BlockSource::hasChunksAt(const Bounds& bounds) {

	auto itr = bounds.begin(), end = bounds.end();

	//check if all the chunks are existing & loaded
	for (; itr != end; ++itr) {
		if (!getChunk( ChunkPos(itr.x, itr.z))) {
			return false;
		}
	}
	return true;
}

bool BlockSource::hasChunksAt(const AABB& bb) {
	//note: we squash all the chunks bounds, it makes no sense to request a span over y
	return hasChunksAt(Bounds(BlockPos(bb.min.x, 0.f, bb.min.z), BlockPos(bb.max.x, 0.f, bb.max.z), mChunkSource.getChunkSide()));
}

bool BlockSource::hasChunksAt(const BlockPos& pos, int r){
	return hasChunksAt(pos.x, pos.y, pos.z, r);
}

bool BlockSource::hasChunksAt(int x, int y, int z, int r) {
	return hasChunksAt(Bounds(BlockPos(x - r, y, z - r), BlockPos(x + r, y, z + r), mChunkSource.getChunkSide()));
}

bool BlockSource::hasChunksAt(const BlockPos& min, const BlockPos& max) {
	return hasChunksAt(Bounds(min, max, mChunkSource.getChunkSide()));
}

BlockID BlockSource::getBlockID(const BlockPos& pos){
	if (pos.y >= 0 && pos.y < mMaxHeight) {
		LevelChunk* c = getChunk( pos );
		return c ? c->getBlock((ChunkBlockPos)pos ) : BlockID::AIR;
	}
	else{
		return BlockID::AIR;
	}

}

const Block& BlockSource::getBlock(const BlockPos& pos){
	return *Block::mBlocks[getBlockID(pos)];
}

DataID BlockSource::getData(const BlockPos& p){
	if (p.y >= 0 && p.y < mMaxHeight) {
		LevelChunk* c = getChunk(p);
		return c ? c->getData((ChunkBlockPos)p) : 0;
	}
	return 0;
}

FullBlock BlockSource::getBlockAndData(const BlockPos& p) {

	if (p.y >= 0 && p.y < mMaxHeight) {
		LevelChunk* c = getChunk(p);
		if (c) {
			return c->getBlockAndData((ChunkBlockPos)p);
		}
	}
	return FullBlock();
}

bool BlockSource::setExtraData(const BlockPos& p, uint16_t extraData) {

	if (p.y >= 0 && p.y < mMaxHeight) {
		LevelChunk* c = getChunk(p);
		if (c) {
			if(c->setBlockExtraData((ChunkBlockPos)p, extraData)) {
				// Update change to clients
				if (shouldFireEvents(*c)) {
// 					mLevel.broadcastLevelEvent(LevelEvent::UpdateExtraData, Vec3(p.x + 0.5f, p.y + 0.5f, p.z + 0.5f), extraData);
				}
			}
			return true;
		}
	}
	return false;
}

uint16_t BlockSource::getExtraData(const BlockPos& p){
	if (p.y >= 0 && p.y < mMaxHeight) {
		LevelChunk* c = getChunk(p);
		if (c) {
			return c->getBlockExtraData((ChunkBlockPos)p);
		}
	}
	return 0;
}

void BlockSource::setBorderBlock(const BlockPos& pos, bool val) {
	LevelChunk* c = getChunkAt(pos);
	if (c) {
		c->setBorder(ChunkBlockPos(pos), val);
	}
}

bool BlockSource::hasBorderBlock(const BlockPos pos) {
	LevelChunk* chunk = getChunkAt(pos);

	return chunk ? chunk->getBorder(ChunkBlockPos(pos)) : false;
}

Height BlockSource::getAllocatedHeightAt(const BlockPos& xzPos) {
	LevelChunk* chunk = getChunkAt(xzPos);

	return chunk ? chunk->getAllocatedHeight() : 0;
}

BrightnessPair BlockSource::getLightColor(const BlockPos& pos, Brightness minBlockLight) {
// 	auto brightness = BrightnessPair{
// 		mDimension.getSkyLight(),
// 		LightLayer::BLOCK.getSurrounding()
// 	};
// 
// 	if (pos.y >= 0 && pos.y < mMaxHeight) {
// 		if (auto chunk = getChunk(pos)) {
// 			//TODO //OPTIMIZE expose a getBrightnessPair in LevelChunk for efficiency
// 			//this might be the most called brightness method because it's used in tessellating
// 			ChunkBlockPos cp(pos);
// 			brightness.sky = chunk->getBrightness(mDimension.getSkyLightLayer(), cp);
// 			brightness.block = chunk->getBrightness(LightLayer::BLOCK, cp);
// 		}
// 	}
// 
// 	brightness.block = std::max(brightness.block, minBlockLight);
// 
// 	return brightness;

	return BrightnessPair();
}

// Brightness BlockSource::getBrightness(const LightLayer& layer, const BlockPos& pos){
// 	if (pos.y < 0 || pos.y >= mMaxHeight ) {
// 		return layer.getSurrounding();
// 	}
// 
// 	LevelChunk* c = getChunk( pos );
// 	return c ? c->getBrightness(layer, (ChunkBlockPos)pos) : Brightness::MIN;
// }

float BlockSource::getBrightness(const BlockPos& pos){
	return mDimension.getBrightnessRamp()[getRawBrightness(pos)];
}

Brightness BlockSource::getRawBrightness(const BlockPos& pos, bool propagate){
	auto id = getBlockID(pos);
	if (Block::mLightBlock[id] == Brightness::MAX) {
		return Brightness::MIN;	//no need to follow through with everything!

	}
	//@bounds-check
	if (propagate) {
		if (id == Block::mStoneSlab->mID || id == Block::mFarmland->mID || id == Block::mWoodenSlab->mID) {
			Brightness br = getRawBrightness(pos.above(), false);
			br = std::max(br, getRawBrightness(pos.west(), false));
			br = std::max(br, getRawBrightness(pos.east(), false));
			br = std::max(br, getRawBrightness(pos.south(), false));
			br = std::max(br, getRawBrightness(pos.north(), false));
			return br;
		}
	}

	if (pos.y < 0) {
		return Brightness::MIN;
	}
	else if (pos.y >= mMaxHeight) {
		auto br = Brightness::MAX;
		br -= _getSkyDarken();	//TODO is this evil?
		return br > Brightness::MAX ? Brightness::MIN : br;
	}

	LevelChunk* c = getChunk(pos);
	return c ? c->getRawBrightness((ChunkBlockPos)pos, _getSkyDarken()) : Brightness::MAX;
}

Height BlockSource::getAboveTopSolidBlock(const BlockPos& pos, bool includeWater /* = false */, bool includeLeaves /* = false */) {
	LevelChunk* levelChunk = getChunkAt(pos);

	return levelChunk ? levelChunk->getAboveTopSolidBlock((ChunkBlockPos)pos, mDimension.showSky(), includeWater, includeLeaves) : getDimension().getHeight();
}

bool BlockSource::canSeeSky(const BlockPos& pos){
	if (pos.y >= mMaxHeight) {
		return true;
	}
	else if (pos.y < 0) {
		return false;
	}
	else {
		auto lc = getChunk(pos);
		return lc ? lc->isSkyLit((ChunkBlockPos)pos) : true;
	}
}

const Material& BlockSource::getMaterial(const BlockPos& pos) {
	return getBlock(pos).getMaterial();
}

Height BlockSource::getHeightmap(const BlockPos& pos) {
	LevelChunk* c = getChunkAt(pos);

	return c ? c->getHeightmap(ChunkBlockPos(pos)) : 0;
}

BlockPos BlockSource::getHeightmapPos(const BlockPos& xzPos) {
	return BlockPos(xzPos.x, getHeightmap(xzPos), xzPos.z);
}

Brightness BlockSource::_getSkyDarken() {
	return mPublicSource ? mDimension.getSkyDarken() : Brightness::MIN;
}

bool BlockSource::isSolidBlockingBlock(int x, int y, int z){
	auto& block = getBlock({x, y, z});
	return block.getMaterial().isSolidBlocking() && (block.hasProperty(BlockProperty::CubeShaped) || block.hasProperty(BlockProperty::SolidBlocking));
}

bool BlockSource::isSolidBlockingBlock(const BlockPos& p){
	return isSolidBlockingBlock(p.x, p.y, p.z);
}

bool BlockSource::isConsideredSolidBlock(int x, int y, int z) {
	return getBlock({ x, y, z }).isSolid();
}

bool BlockSource::isConsideredSolidBlock(const BlockPos& pos) {
	return isConsideredSolidBlock(pos.x, pos.y, pos.z);
}

bool BlockSource::mayPlace(BlockID blockId, const BlockPos& pos, FacingID face, Entity* placer, bool ignoreEntitites /*= false*/, Entity* ignoreEntity /*= nullptr*/){
	const auto& targetBlock = getBlock(pos);
	const Block* block = Block::mBlocks[blockId];

	AABB tmpAABB;
	DataID placementData = placer != nullptr ? block->getPlacementDataValue(*placer, pos, face, Vec3::ZERO, 0) : 0;
	auto& aabb = ignoreEntitites ? AABB::EMPTY : block->getAABB(*this, pos, tmpAABB, placementData);

	if (pos.y < 0 || pos.y >= mMaxHeight) {
		return false;
	}

	if (!aabb.isEmpty() && !isUnobstructedByEntities(aabb, ignoreEntity)) {
		return false;
	}

	bool canBeBuiltOver = targetBlock.canBeBuiltOver(*this, pos);

	bool topSnowPlaceable = targetBlock.hasProperty(BlockProperty::Snow | BlockProperty::TopSnow) || 
		(block->mID == Block::mTopSnow->mID && TopSnowBlock::checkIsRecoverableBlock(targetBlock.mID));

	if (blockId > BlockID::AIR && 
		(canBeBuiltOver || topSnowPlaceable)) {
		if (block->mayPlace(*this, pos, face)) {
			return true;
		}
	}

	return false;
}

const MobList& BlockSource::getMobsAt(EntityType category, const BlockPos& pos) {
	return mChunkSource.getMobsAt(*this, category, pos);
}

bool BlockSource::setBlock(int x, int y, int z, BlockID block, int updateFlags) {
	return setBlockAndData(BlockPos(x, y, z), FullBlock(block, 0), updateFlags);
}

bool BlockSource::setBlock(const BlockPos& pos, BlockID block, int updateFlags) {
	return setBlockAndData(pos, FullBlock(block, 0), updateFlags);
}

void BlockSource::neighborChanged(const BlockPos& neighPos, const BlockPos& myPos){
// 	if (!getLevelConst().isClientSide()) {
// 		auto& block = getBlock(neighPos);
// 		if(!block.isType(Block::mAir)) {
// 			if (auto queue = getTickQueue(neighPos)) {
// 				if (!queue->isInstaticking() || block.canInstatick()) {
// 					block.neighborChanged(*this, neighPos, myPos);
// 				}
// 			}
// 		}
// 	}
}

void BlockSource::updateNeighborsAt(const BlockPos& pos){
	neighborChanged(pos.west(), pos);
	neighborChanged(pos.east(), pos);
	neighborChanged(pos.south(), pos);
	neighborChanged(pos.north(), pos);
	neighborChanged(pos.above(), pos);
	neighborChanged(pos.below(), pos);
}

void BlockSource::updateNeighborsAt(const BlockPos& destPos, const BlockPos& srcPos) {
	neighborChanged(destPos.west(), srcPos);
	neighborChanged(destPos.east(), srcPos);
	neighborChanged(destPos.south(), srcPos);
	neighborChanged(destPos.north(), srcPos);
	neighborChanged(destPos.above(), srcPos);
	neighborChanged(destPos.below(), srcPos);
}

void BlockSource::updateNeighborsAtExceptFromFacing(const BlockPos& pos, const BlockPos& neighborPos, int skipFacing) {
	if (skipFacing != Facing::WEST) {
		neighborChanged(pos.west(), neighborPos);
	}
	if (skipFacing != Facing::EAST) {
		neighborChanged(pos.east(), neighborPos);
	}
	if (skipFacing != Facing::DOWN) {
		neighborChanged(pos.below(), neighborPos);
	}
	if (skipFacing != Facing::UP) {
		neighborChanged(pos.above(), neighborPos);
	}
	if (skipFacing != Facing::NORTH) {
		neighborChanged(pos.north(), neighborPos);
	}
	if (skipFacing != Facing::SOUTH) {
		neighborChanged(pos.south(), neighborPos);
	}
}


void BlockSource::_blockChanged(const BlockPos& pos, FullBlock block, FullBlock previousBlock, int updateFlags, Entity* changer) {
	if (updateFlags & Block::UPDATE_NEIGHBORS) {
		updateNeighborsAt(pos);
	}

	//let's not call events over sources used for loading
	if ((updateFlags& Block::UPDATE_CLIENTS) != 0 && !(getLevelConst().isClientSide() && (updateFlags& Block::UPDATE_INVISIBLE) != 0)) {
		fireBlockChanged(pos, block, previousBlock, updateFlags, changer);
	}
}

bool BlockSource::setBlockAndData(const BlockPos& pos, FullBlock block, int updateFlags, Entity* placer, Unique<BlockEntity> blockEntity) {
	DEBUG_ASSERT(block.data >= 0 && block.data <= 0xf, "Invalid data");

	if (pos.y < 0 || pos.y >= mMaxHeight) {
		return false;
	}

	LevelChunk* levelChunk = getWritableChunk(pos);
	if (!levelChunk) {
		return false;
	}

	bool fireEvent = shouldFireEvents(*levelChunk);
	auto previousBlock = levelChunk->setBlockAndData((ChunkBlockPos)pos, block, fireEvent ? this : nullptr, std::move(blockEntity));

	BlockID oldBlockId = block.id;
	// When setBlockAndData is called some blocks actually change multiple times. (es: rails) (or SnowGolem blocks turning
	// into AIR)
	// Because of this we need to call getBlockAndData() to capture the latest block data.
	block = levelChunk->getBlockAndData((ChunkBlockPos)pos);

	const bool wasGolemSpawned = (oldBlockId == Block::mPumpkin->mID || oldBlockId == Block::mLitPumpkin->mID) && (block == BlockID::AIR);

	if ((previousBlock != block && fireEvent) || (updateFlags& Block::UPDATE_ITEM_DATA) != 0) {
		_blockChanged(pos, block, previousBlock, updateFlags, placer);
	}

	return (previousBlock.id != block.id) || (previousBlock.data != block.data) || wasGolemSpawned;

}

bool BlockSource::setBlockAndData(const BlockPos& pos, FullBlock block, int updateFlags, Entity* placer) {
	return setBlockAndData(pos, block, updateFlags, placer, nullptr);
}

bool BlockSource::setBlockAndData(const BlockPos& pos, BlockID block, DataID data, int updateFlags, Entity* placer /* = nullptr */) {
	return setBlockAndData(pos, { block, data }, updateFlags, placer, nullptr);
}

bool BlockSource::setBlockAndData(int x, int y, int z, BlockID id, DataID data, int updateFlags) {
	return setBlockAndData(x, y, z, FullBlock(id, data), updateFlags);
}

// void BlockSource::setBrightness(const LightLayer& layer, const BlockPos& pos, Brightness brightness) {
// 	//@bounds-check
// 	if (pos.y < 0 || pos.y >= mMaxHeight) {
// 		return;
// 	}
// 
// 	LevelChunk* c = getWritableChunk(pos);
// 	if (!c) {
// 		return;
// 	}
// 
// 	if (c->setBrightness(layer, (ChunkBlockPos)pos, brightness) && shouldFireEvents(*c)) {
// 		fireBrightnessChanged(pos);
// 	}
// }

bool BlockSource::setBlockNoUpdate(int x, int y, int z, BlockID block) {
	return setBlockAndData(BlockPos(x, y, z), FullBlock(block, 0), Block::UPDATE_NONE);
}

bool BlockSource::setBlockAndDataNoUpdate(int x, int y, int z, FullBlock block) {
	return setBlockAndData(BlockPos(x, y, z), block, Block::UPDATE_NONE);
}

bool BlockSource::removeBlock(int x, int y, int z) {
	return setBlockAndData(BlockPos(x, y, z), FullBlock(), Block::UPDATE_ALL);
}

bool BlockSource::removeBlock(const BlockPos& pos) {
	return setBlockAndData(pos, FullBlock(), Block::UPDATE_ALL);
}

 

BlockEntity* BlockSource::getBlockEntity(const BlockPos& pos ) {
// 	LevelChunk* lc = getChunkAt(pos);
// 	return lc ? lc->getBlockEntity(ChunkBlockPos(pos)) : nullptr;
	return nullptr;
}

Unique<BlockEntity> BlockSource::removeBlockEntity(const BlockPos& blockPos) {
// 	LevelChunk* chunk = getChunkAt(blockPos);
// 	if(chunk) {
// 		return chunk->removeBlockEntity(blockPos);
// 	}

	return nullptr;
}

void BlockSource::blockEvent(int x, int y, int z, int b0, int b1) {
	DEBUG_ASSERT_MAIN_THREAD;

	BlockID t = getBlockID(x, y, z);
	if (t > 0) {
		Block::mBlocks[t]->triggerEvent(*this, BlockPos(x, y, z), b0, b1);
	}

	fireBlockEvent(x, y, z, b0, b1);
}

void BlockSource::blockEvent(const BlockPos& pos, int b0, int b1){
	blockEvent(pos.x, pos.y, pos.z, b0, b1);
}

const EntityList& BlockSource::getEntities(Entity* except, const AABB& bb) {
	mTempEntityList.clear();

	//TODO assign the entities to chunks in the client as well, stop having a global list
	if(getLevel().isClientSide()) {
		const auto& entityMap = getDimension().getEntityIdMap();

		for(auto& entity : entityMap) {
			if(entity.second != except && entity.second->mBB.intersects(bb)) {
				mTempEntityList.push_back(entity.second);
			}
		}
	}
	else {
		int xc0 = Math::floor((bb.min.x - 2) / 16);
		int xc1 = Math::floor((bb.max.x + 2) / 16);
		int zc0 = Math::floor((bb.min.z - 2) / 16);
		int zc1 = Math::floor((bb.max.z + 2) / 16);

		for(auto xc : range_incl(xc0, xc1)) {
			for(auto zc : range_incl(zc0, zc1)) {
				LevelChunk* c = getChunk(xc, zc);
				if(c) {
					c->getEntities(except, bb, mTempEntityList);
				}
			}
		}
	}

// 	getLevel().forEachPlayer([this, &except, &bb](Player& player) {
// 		if(&player != except && player.getDimensionId() == getDimensionId() && player.intersects(bb.min, bb.max)) {
// 			mTempEntityList.push_back(&player);
// 		}
// 		return true;
// 	});
	return mTempEntityList;
}

const EntityList& BlockSource::getEntities(EntityType entityTypeId, const AABB& bb, Entity* except){
	mTempEntityList.clear();
	BlockPos min = bb.min, max = bb.max;
	Bounds bounds(min, max, CHUNK_WIDTH, true);

	//TODO assign the entities to chunks in the client as well, stop having a global list
	if(getLevel().isClientSide()) {
		const auto& entityMap = getDimension().getEntityIdMap();

		for(auto& entity : entityMap) {
			if(entity.second != except && EntityClassTree::isInstanceOf(*entity.second, entityTypeId) && entity.second->mBB.intersectsInner(bb)) {
				mTempEntityList.push_back(entity.second);
			}
		}
	}
	else {
		for(auto& pos : bounds) {
			auto lc = getChunk(ChunkPos(pos.x, pos.z));
			if(lc) {
				for(auto& e : lc->getEntities()) {
					if(e.get() != except && EntityClassTree::isInstanceOf(*e, entityTypeId) && e->mBB.intersectsInner(bb)) {
						mTempEntityList.push_back(e.get());
					}
				}
			}
		}
	}

// 	if (EntityClassTree::isTypeInstanceOf(EntityType::Player, entityTypeId)) {
// 		getLevel().forEachPlayer([this, &except, &bb](Player& player) {
// 			if (&player != except && player.getDimensionId() == getDimensionId() && player.intersects(bb.min, bb.max)) {
// 				mTempEntityList.push_back(&player);
// 			}
// 
// 			return true;
// 		});
// 	}

	return mTempEntityList;
}

Entity* BlockSource::getNearestEntityOfType(Entity* except, const AABB& bb, EntityType entityTypeId){
	DEBUG_ASSERT(except, "Must pass in an entity to compare distance against");
	Entity* selected = nullptr;
	float closestDistSqr = std::numeric_limits<float>::max();
	const EntityList& selection = getEntities(entityTypeId, bb, except);

	for (Entity* entity : selection) {
		float distSqr = entity->distanceToSqr(*except);
		if (distSqr > closestDistSqr) {
			continue;
		}

		closestDistSqr = distSqr;
		selected = entity;
	}

	return selected;
}

Entity* BlockSource::getNearestEntityOfType(Entity* except, const Vec3& pos, float range, EntityType entityTypeId){
	AABB bb(pos, pos);
	const EntityList& selection = getEntities(entityTypeId, bb.grow(Vec3(range)), except);

	float dist = -1;
	Entity* selected = nullptr;

	for (unsigned int i = 0; i < selection.size(); i++) {
		Entity* e = selection[i];

		float d = e->distanceToSqr(pos);
		if (dist < 0 || d < dist) {
			dist = d;
			selected = e;
		}
	}
	return selected;
}

Entity* BlockSource::getNearestEntityNotOfType(Entity* except, const Vec3& pos, float range, EntityType entityTypeId) {
	AABB bb(pos, pos);
	const EntityList& selection = getEntities(EntityType::Mob, bb.grow(Vec3(range)), except);

	float dist = -1;
	Entity* selected = nullptr;

	for (unsigned int i = 0; i < selection.size(); i++) {
		Entity* e = selection[i];

		if (!EntityClassTree::isInstanceOf(*e, entityTypeId)) {
			float d = e->distanceToSqr(pos);
			if (selected == nullptr || d < dist) {
				dist = d;
				selected = e;
			}
		}
	}
	return selected;
}

bool BlockSource::isUnobstructedByEntities(const AABB& aabb, Entity* ignoreEntity) {
	auto& entities = getEntities(ignoreEntity, aabb);

	for (unsigned int i = 0; i < entities.size(); i++) {
		Entity* e = entities[i];
		if (!e->isRemoved() && e->mBlocksBuilding) {
			return false;
		}
	}
	return true;
}

// Need to make it low enough players wont fall and hit it, displaying the wrong death message
const float VOID_BOUNDARY = -40.f;

std::vector<AABB>& BlockSource::fetchAABBs(const AABB& intersectTestBox, bool withUnloadedChunks ) {	//@attn: check the
																									// AABB* new/delete
																									// stuff

	mTempCubeList.clear();
	AABB box = intersectTestBox;

	if (withUnloadedChunks) {
		addUnloadedChunksAABBs(box);
	}

	//prevent things from falling infinitely
	if (box.max.y < 0) {
		addVoidFloor(box);
	}

	for(auto x : range_incl(Math::floor(box.min.x), Math::floor(box.max.x + 1))) {
		for(auto z : range_incl(Math::floor(box.min.z), Math::floor(box.max.z + 1))) {
			if(hasBlock(x, mMaxHeight / 2, z)) {
				for(auto y : range_incl(Math::floor(box.min.y - 1), Math::floor(box.max.y))) {
					auto& block = getBlock({ x, y, z });
					block.addAABBs(*this, BlockPos(x, y, z), &box, mTempCubeList);
				}
			}
		}
	}

	fetchBorderBlockAABBs(intersectTestBox);

	return mTempCubeList;
}

std::vector<AABB>& BlockSource::fetchBorderBlockAABBs(const AABB& intersectTestBox) {
#ifdef MCPE_EDU
	//allow world builder players through
	float y = intersectTestBox.getCenter().y;
	AABB box = intersectTestBox;
	int startingVal = static_cast<int>(std::floor(y - 1));
	int endVal = static_cast<int>(std::ceil(y + 1));
	//generates aabbs based on border blocks around you for your positions delta this frame
	for (auto x : range_incl(Math::floor(box.min.x), Math::floor(box.max.x + 1))) {
		for (auto z : range_incl(Math::floor(box.min.z), Math::floor(box.max.z + 1))) {
			auto hasBorder = hasBorderBlock(BlockPos(x, 0, z));

			if (hasBorder) {
				//Allow Super Users through
				const Block& block = *Block::mBorder;

				//fill from our last y to our next possible y
				for (int i = startingVal; i < endVal; ++i) {
					block.addAABBs(*this, BlockPos(x, i, z), &box, mTempCubeList);
				}
			}
		}
	}
#endif
	return mTempCubeList;
}

std::vector<AABB>& BlockSource::fetchBorderBlockCollisions(BlockSource& region, const AABB& intersectTestBox, Entity* entity) {
	//allow world builder players through
	if (entity && EntityClassTree::isInstanceOf(*entity, EntityType::Player) && static_cast<Player*>(entity)->mAbilities.isWorldBuilder()) {
		return mTempCubeList;
	}
	float y = intersectTestBox.getCenter().y;
	AABB box = intersectTestBox;
	int startingVal = static_cast<int>(std::floor(y - 1));
	int endVal = static_cast<int>(std::ceil(y + 1));
	//generates aabbs based on border blocks around you for your positions delta this frame
	for (auto x : range_incl(Math::floor(box.min.x), Math::floor(box.max.x + 1))) {
		for (auto z : range_incl(Math::floor(box.min.z), Math::floor(box.max.z + 1))) {
			auto hasBorder = hasBorderBlock(BlockPos(x, 0, z));
			
			if (hasBorder) {
				//Allow Super Users through
				const Block& block = *Block::mBorder;

				//fill from our last y to our next possible y
				for (int i = startingVal; i < endVal; ++i) {
					block.addCollisionShapes(region, BlockPos(x, i, z), &box, mTempCubeList, entity);	
				}
			}
		}
	}

	return mTempCubeList;
}

std::vector<AABB>& BlockSource::fetchCollisionShapes(BlockSource& region, const AABB& intersectTestBox, float* actualSurfaceOffset, bool withUnloadedChunks, Entity* entity){
	mTempCubeList.clear();
	AABB box = intersectTestBox;

	if (withUnloadedChunks) {
		addUnloadedChunksAABBs(box);
	}

	//prevent things from falling infinitely
	if (box.max.y < 0) {
		addVoidFloor(box);
	}

	int lowestThickBlockY = INT_MAX;
	if (actualSurfaceOffset) {
		*actualSurfaceOffset = 0;
	}

	for (auto x : range_incl(Math::floor(box.min.x), Math::floor(box.max.x + 1))) {
		for (auto z : range_incl(Math::floor(box.min.z), Math::floor(box.max.z + 1))) {
			for (auto y : range_incl(Math::floor(box.min.y - 1), Math::floor(box.max.y + 1))) {

				auto& block = getBlock({ x, y, z });
				if (block.addCollisionShapes(region, BlockPos(x, y, z), &box, mTempCubeList, entity)) {
					if (actualSurfaceOffset) {
						float thickness = block.getThickness();
						if (thickness && y < lowestThickBlockY) {
							*actualSurfaceOffset = thickness;
							lowestThickBlockY = y;
						}
					}
				}
			}
		}
	}

	//append border block collisions
	fetchBorderBlockCollisions(region, intersectTestBox, entity);

	return mTempCubeList;
}

bool BlockSource::containsAnyLiquid(const AABB& box) {
	AABB floored = box.flooredCeiledCopy();

	for(auto x : rangef(floored.min.x, floored.max.x)) {
		for(auto z : rangef(floored.min.z, floored.max.z)) {
			for(auto y : rangef(floored.min.y, floored.max.y)) {
				auto& block = getBlock({ x, y, z });
				if (block.getMaterial().isLiquid()) {
					return true;
				}
			}
		}
	}
	return false;
}

bool BlockSource::containsAnySolidBlocking(const AABB& box) {
	AABB floored = box.flooredCeiledCopy();

	for (auto x : rangef(floored.min.x, floored.max.x)) {
		for (auto z : rangef(floored.min.z, floored.max.z)) {
			for (auto y : rangef(floored.min.y, floored.max.y)) {
				auto& block = getBlock({ x, y, z });
				if (block.getMaterial().isSolidBlocking()) {
					return true;
				}
			}
		}
	}
	return false;
}

bool BlockSource::containsAny(const BlockPos& min, const BlockPos& max) {
	BlockPos p = min;

	for (; p.x < max.x; ++p.x) {
		for (p.z = min.z; p.z < max.z; ++p.z) {
			for (p.y = min.y; p.y < max.y; ++p.y) {
				if (getBlockID(p) != BlockID::AIR) {
					return true;
				}
			}
		}
	}
	return false;
}

bool BlockSource::containsFireBlock(const AABB& box) {
	AABB floored = box.flooredCeiledCopy();

	if(hasChunksAt(floored)) {
		for (auto x : rangef(floored.min.x, floored.max.x)) {
			for (auto z : rangef(floored.min.z, floored.max.z)) {
				for (auto y : rangef(floored.min.y, floored.max.y)) {
					if( getMaterial(x, y, z).isSuperHot() ) {
						return true;
					}
				}
			}
		}
	}
	return false;
}

bool BlockSource::containsMaterial(const AABB& box, MaterialType material) {
	int x0 = Math::floor(box.min.x);
	int x1 = Math::floor(box.max.x + 1);
	int y0 = Math::floor(box.min.y);
	int y1 = Math::floor(box.max.y + 1);
	int z0 = Math::floor(box.min.z);
	int z1 = Math::floor(box.max.z + 1);

	for (int x = x0; x < x1; x++) {
		for (int y = y0; y < y1; y++) {
			for (int z = z0; z < z1; z++) {
				const Block& block = getBlock(BlockPos(x, y, z));
				if (block.getMaterial().isType(material)) {
					return true;
				}
			}
		}
	}
	return false;
}

bool BlockSource::containsLiquid(const AABB& box, MaterialType material) {
	int x0 = Math::floor(box.min.x);
	int x1 = Math::floor(box.max.x + 1);
	int y0 = Math::floor(box.min.y);
	int y1 = Math::floor(box.max.y + 1);
	int z0 = Math::floor(box.min.z);
	int z1 = Math::floor(box.max.z + 1);

	for (int x = x0; x < x1; x++) {
		for (int y = y0; y < y1; y++) {
			for (int z = z0; z < z1; z++) {
				const Block& block = getBlock(BlockPos(x, y, z));
				if (block.getMaterial().isType(material)) {
					int data = getData(x, y, z);
					float yh1 = (float)(y + 1);
					if (data < 8) {
						yh1 = (float)y + 1.0f - (float)data / 8.0f;
					}
					if (yh1 >= box.min.y) {
						return true;
					}
				}
			}
		}
	}
	return false;
}

HitResult BlockSource::clip(const Vec3& A, const Vec3& B, bool liquid /*= false*/, bool solidOnly /*= false*/, int maxDistance /*= 200*/, bool useBorderBlocks /*= false*/) {

	if (A.isNan()) {
		return HitResult(B);
	}
	if (B.isNan()) {
		return HitResult(B);
	}

	Vec3 a(A);
	Vec3 b(B);

	int xBlock1 = Math::floor(b.x);
	int yBlock1 = Math::floor(b.y);
	int zBlock1 = Math::floor(b.z);

	int xBlock0 = Math::floor(a.x);
	int yBlock0 = Math::floor(a.y);
	int zBlock0 = Math::floor(a.z);

	HitResult liquidHit;
	bool hitLiquid = false;

	// The java version does an initial check on the position you start from,
	// could potentially solve the issue where you can remove blocks through ladders
	const Material* startedInLiquidMaterial = nullptr;
	{
		FullBlock block = getBlockAndData(xBlock0, yBlock0, zBlock0);
		const Block* t = Block::mBlocks[block.id];
		AABB bufferAABB;
		if (t != nullptr)
		{
			// Did we start off inside liquid?
			startedInLiquidMaterial = t->getMaterial().isLiquid() ? &t->getMaterial() : nullptr;
			if (startedInLiquidMaterial != nullptr) {
				// don't hit blocks that have the same liquid material when starting off inside liquid.
				liquid = false;
				hitLiquid = true;
				liquidHit = t->clip(*this, BlockPos(xBlock0, yBlock0, zBlock0), a, b);
			} else {
				if (solidOnly && t->getAABB(*this, BlockPos(xBlock0, yBlock0, zBlock0), bufferAABB).isEmpty()) {
					// No collision
				}
				else if (t->mayPick(*this, block.data, liquid)) {
					HitResult r = t->clip(*this, BlockPos(xBlock0, yBlock0, zBlock0), a, b);
					r.setIsHitLiquid(hitLiquid, liquidHit);
					if (r.isHit()) {
						return r;
					}
				}
			}

		}
		
	}

	HitResult ret = HitResult(B);

	for (auto i : range(0, maxDistance)) {
		UNUSED_VARIABLE(i);
		if (a.isNan()) {
			ret = HitResult(b);
			break;
		}
		if (xBlock0 == xBlock1 && yBlock0 == yBlock1 && zBlock0 == zBlock1) {
			ret = HitResult(b);
			break;
		}

		float xClip = 0.f;
		float yClip = 0.f;
		float zClip = 0.f;

		bool hasXClip = false;
		bool hasYClip = false;
		bool hasZClip = false;

		// Set clip to be the far boundary of the block we're in relative to the ray direction
		// If the ray is pointing right, put xclip on right boundary
		if (xBlock1 > xBlock0) {
			hasXClip = true;
			xClip = xBlock0 + 1.000f;
		}
		// If the ray is pointing left, put xclip on the left boundary, etc. for y and z
		if (xBlock1 < xBlock0) {
			hasXClip = true;
			xClip = xBlock0 + 0.000f;
		}

		if (yBlock1 > yBlock0) {
			hasYClip = true;
			yClip = yBlock0 + 1.000f;
		}
		if (yBlock1 < yBlock0) {
			hasYClip = true;
			yClip = yBlock0 + 0.000f;
		}

		if (zBlock1 > zBlock0) {
			hasZClip = true;
			zClip = zBlock0 + 1.000f;
		}
		if (zBlock1 < zBlock0) {
			hasZClip = true;
			zClip = zBlock0 + 0.000f;
		}

		float xDist = 999;
		float yDist = 999;
		float zDist = 999;

		float xd = b.x - a.x;
		float yd = b.y - a.y;
		float zd = b.z - a.z;

		// Determine time of intersection of the ray out of this block for each axis
		if (hasXClip) {
			xDist = (xClip - a.x) / xd;
		}
		if (hasYClip) {
			yDist = (yClip - a.y) / yd;
		}
		if (hasZClip) {
			zDist = (zClip - a.z) / zd;
		}

		// Clip to the closest boundary axis of this block
		if (xDist < yDist && xDist < zDist) {
			// March block pos forward into the block we're clipping the ray on. This is more reliable then flooring clip result for when multiple axes of a lie on a boundary
			// It is safe to make a change by one because we are ensuring stepping forward one block by clipping to a block boundary which is unit size and choosing the earliest intersection
			// Same idea in different directions for each branch here
			if (xBlock1 > xBlock0) {
				++xBlock0;
			}
			else{
				--xBlock0;
			}

			// Clip a to the block boundary. While it may seem redundant to have a clip in each branch instead of one clip with xd,yd,zd, explicitly setting xclip instead of xd*xDist helps mitigate precision issues
			a.x = xClip;
			a.y += yd * xDist;
			a.z += zd * xDist;
		}
		else if (yDist < zDist) {
			if (yBlock1 > yBlock0) {
				++yBlock0;
			}
			else{
				--yBlock0;
			}

			a.x += xd * yDist;
			a.y = yClip;
			a.z += zd * yDist;
		}
		else {
			if (zBlock1 > zBlock0) {
				++zBlock0;
			}
			else{
				--zBlock0;
			}

			a.x += xd * zDist;
			a.y += yd * zDist;
			a.z = zClip;
		}

		FullBlock t = getBlockAndData(xBlock0, yBlock0, zBlock0);
		const Block* block = Block::mBlocks[t.id];

		AABB aaBB;
		//border blocks are infinite so the y BlockPos in our check is unwanted
		//Currently the system doesn't support an idea of an infinite block/collision and We've seen this as the least intrusive way to do so.
		bool hasborderBlock = !useBorderBlocks && hasBorderBlock(BlockPos(xBlock0, 0, zBlock0));
		BlockPos blockPos(xBlock0, yBlock0, zBlock0);

		//if border block. worldbuilders can get through
		if (hasborderBlock) {
			block = Block::mBorder;
			block->getAABB(*this, blockPos, aaBB);
			aaBB.min.y = 0;
			aaBB.max.y = mMaxHeight;
		}

		if(block) {

			// Toggle on hitting liquid blocks if we left the type of liquid we started in.
			if (startedInLiquidMaterial && !liquid && (&block->getMaterial() != startedInLiquidMaterial))
				liquid = true;

			if(solidOnly && (!hasborderBlock && block->getAABB(*this, blockPos, aaBB).isEmpty())) {
				// No collision
			}
			else if(block->mayPick(*this, t.data, liquid)) {
				if (liquid && block->getMaterial().isLiquid()) {
					liquid = false;
					hitLiquid = true;
					liquidHit = hasborderBlock ? block->clip(*this, blockPos, a, b, false, 0, aaBB) : block->clip(*this, blockPos, a, b);
					continue;
				}

				// pass origin A because AABB for block might be out side of target block
				HitResult hit = hasborderBlock ? block->clip(*this, blockPos, A, b, false, t.data, aaBB) : block->clip(*this, blockPos, A, b, false, t.data);
				hit.setIsHitLiquid(hitLiquid, liquidHit);
				if(hit.isHit()) {
					return hit;
				}
			}
		}
	}

	ret.setIsHitLiquid(hitLiquid, liquidHit);
	return ret;
}

float BlockSource::getSeenPercent(const Vec3& center, const AABB& bb) {
	float xs = 1.0f / ((bb.max.x - bb.min.x) * 2 + 1);
	float ys = 1.0f / ((bb.max.y - bb.min.y) * 2 + 1);
	float zs = 1.0f / ((bb.max.z - bb.min.z) * 2 + 1);
	int hits = 0;
	int count = 0;

	float xOffset = (1.0f - (float)Math::floor(1.0f / xs) * xs) / 2.0f;
	float zOffset = (1.0f - (float)Math::floor(1.0f / zs) * zs) / 2.0f;
	float yOffset = 0.1f; // y test should be above ground by a small amount.
	
	if (xs < 0 || ys < 0 || zs < 0) {
		return 0;
	}

	for (float xx = 0.0f; xx <= 1.0f; xx += xs) {
		for (float yy = 0.0f; yy <= 1.0f; yy += ys) {
			for (float zz = 0.0f; zz <= 1.0f; zz += zs) {
				float x = bb.min.x + (bb.max.x - bb.min.x) * xx;
				float y = bb.min.y + (bb.max.y - bb.min.y) * yy;
				float z = bb.min.z + (bb.max.z - bb.min.z) * zz;
				
				Vec3 testPos(x + xOffset, y + yOffset, z + zOffset);

				HitResult hit = clip(testPos, center);
				if (!hit.isHit()) {
					hits++;
				}
				count++;
			}
		}
	}

	return hits / (float)count;
}

// void BlockSource::runLightUpdates(const LightLayer& layer, const BlockPos& min, const BlockPos& max) {
// 	getLevel().runLightUpdates(*this, layer, min, max);	//we just want level to continue this next frame basically
// }

void BlockSource::_fireColumnDirty(int x, int y0, int y1, int z, int flags) {
	fireBlocksDirty(BlockPos(x, y0, z), BlockPos(x, y1, z));
}

// void BlockSource::updateLightIfOtherThan(const LightLayer& layer, const BlockPos& pos, Brightness expected) {
// 
// 	if (layer.isSky() && layer.getSurrounding() == Brightness::MIN) {
// 		return;
// 	}
// 
// 	LevelChunk* c = getWritableChunk(pos);
// 	if (!c) {
// 		return;
// 	}
// 
// 	if (layer.isSky()) {
// 		if (canSeeSky(pos)) {
// 			expected = layer.getSurrounding();
// 		}
// 	}
// 	else if (layer == LightLayer::BLOCK) {
// 		BlockID t = getBlockID(pos);
// 		if (Block::mLightEmission[t] > expected) {
// 			expected = Block::mLightEmission[t];
// 		}
// 	}
// 
// 	if ( shouldFireEvents(*c) && getBrightness(layer, pos) != expected) {
// 		runLightUpdates(layer, pos, pos);
// 	}
// }

//TODO move this out and make a property this is horrible oh god
#include "world/level/block/StairBlock.h"
#include "world/level/block/SlabBlock.h"
#include "world/level/block/TopSnowBlock.h"

bool BlockSource::canProvideSupport(const BlockPos& pos, FacingID face, BlockSupportType type) {
	return getBlock(pos).canProvideSupport(*this, pos, face, type);
}

bool BlockSource::isInWall(const Vec3& pos) {
	BlockPos blockPos(pos);
	return isSolidBlockingBlock(blockPos) && isConsideredSolidBlock(blockPos);
}

void BlockSource::onChunkDiscarded(LevelChunk& lc) {
	if (&lc == mLastChunk) {
		mLastChunk = nullptr;
	}
}

void BlockSource::clearCachedLastChunk() {
	mLastChunk = nullptr;
}

void BlockSource::fireAreaChanged( const BlockPos& min, const BlockPos& max ){
	DEBUG_ASSERT_MAIN_THREAD;

	for(size_t i = 0; i < mListeners.size(); ++i) {
		mListeners[i]->onAreaChanged(*this, min, max);
	}
}

void BlockSource::fireBlocksDirty(const BlockPos& pos0, const BlockPos& pos1) {
	DEBUG_ASSERT_MAIN_THREAD;

	for(size_t i = 0; i < mListeners.size(); ++i) {
		mListeners[i]->onBlocksDirty(*this, pos0, pos1);
	}
}

void BlockSource::fireBlockChanged(const BlockPos& pos, FullBlock block, FullBlock oldBlock, int flags, Entity* changer){
	DEBUG_ASSERT_MAIN_THREAD;

	for(size_t i = 0; i < mListeners.size(); ++i) {
		mListeners[i]->onBlockChanged(*this, pos, block, oldBlock, flags, changer);
	}
}

void BlockSource::fireBrightnessChanged( const BlockPos& pos ){
	DEBUG_ASSERT_MAIN_THREAD;

	for(size_t i = 0; i < mListeners.size(); ++i) {
		mListeners[i]->onBrightnessChanged(*this, pos);
	}
}

void BlockSource::fireBlockEntityChanged(BlockEntity& te) {
	DEBUG_ASSERT_MAIN_THREAD;
// 	LevelChunk* c = getChunkAt(te.getPosition());
// 	if (c && shouldFireEvents(*c)) {
// 		//mark the chunk as dirty
// 		c->onBlockEntityChanged();
// 
// 		for(size_t i = 0; i < mListeners.size(); ++i) {
// 			mListeners[i]->onBlockEntityChanged(*this, te);
// 		}
// 	}
}

void BlockSource::fireBlockEntityRemoved(Unique<BlockEntity> te){
	DEBUG_ASSERT_MAIN_THREAD;

// 	for(size_t i = 0; i < mListeners.size() && te; ++i) {
// 		te = mListeners[i]->onBlockEntityRemoved(*this, std::move(te));
// 	}
}

void BlockSource::fireBlockEvent(int x, int y, int z, int b0, int b1){
	DEBUG_ASSERT_MAIN_THREAD;

// 	for(size_t i = 0; i < mListeners.size(); ++i) {
// 		mListeners[i]->onBlockEvent(*this, x, y, z, b0, b1);
// 	}
}

void BlockSource::fireEntityChanged(Entity& entity) {
// 	LevelChunk* c = getChunkAt(entity.getPos());
// 	if (c && shouldFireEvents(*c)) {
// 		for (size_t i = 0; i < mListeners.size(); ++i) {
// 			mListeners[i]->onEntityChanged(*this, entity);
// 		}
// 	}
}

bool BlockSource::isEmptyBlock(const BlockPos& pos) {
	return getBlockID(pos) == 0;
}

bool BlockSource::isEmptyBlock(int x, int y, int z){
	return isEmptyBlock(BlockPos(x, y, z));
}

BlockID BlockSource::getTopBlock(int x, int& y, int z) {
	y = 63;

	while (!isEmptyBlock(x, y + 1, z)) {
		y++;
	}
	return getBlockID(x, y, z);
}

LevelChunk* BlockSource::getChunkAt(const BlockPos& pos) {
	return getChunk(ChunkPos(pos));
}

LevelChunk* BlockSource::getChunkAt(int x, int y, int z){
	return getChunk(BlockPos(x, y, z));
}

Biome* BlockSource::tryGetBiome(const BlockPos& pos) {
	BlockPos fixedPos(pos.x, 0, pos.z);

	LevelChunk* c = getChunkAt(fixedPos);
	if (c == nullptr || fixedPos.y < 0 || fixedPos.y >= mMaxHeight) {
		return nullptr;
	}
	return &c->getBiome(ChunkBlockPos(fixedPos));
}

Biome& BlockSource::getBiome(const BlockPos& pos){
	Biome* found = tryGetBiome(pos);
	return found ? *found : *Biome::ocean; // default biome
}

void BlockSource::setGrassColor(int grassColor, const BlockPos& pos, int flags ) {
	LevelChunk* c = getWritableChunk(pos);
	if(c != nullptr) {
		c->setGrassColor(grassColor, ChunkBlockPos(pos));

		if (shouldFireEvents(*c)) {
			auto block = getBlockAndData(pos);
			_blockChanged(pos, block, block.id, flags, nullptr);
		}
	}
}

int BlockSource::getGrassColor(const BlockPos& pos) {
	LevelChunk* c = getChunkAt(pos);

	return c ? c->getGrassColor(ChunkBlockPos(pos)) : 0;
}

bool BlockSource::findNextTopSolidBlockUnder(BlockPos& pos){
	//look for the start of the next air column
	auto lc = getChunkAt(pos);

	if(!lc) {
		return false;
	}

	pos.y = std::min(pos.y, (int)lc->getAllocatedHeight()); //start from the lowest exitsting height

	while (true) {
		auto t = getBlockID(pos);

		--pos.y;

		if (!t) {
			break;
		}

		if (pos.y < 0) {
			return false;
		}
	}

	//look for the first solid block
	while (true) {
		auto& mat = getMaterial(pos);

		if (mat.isSolidBlocking()) {
			return true;//found

		}
		if (--pos.y < 0) {
			return false;
		}
	}
}

bool BlockSource::findNextSpawnBlockUnder(BlockPos& pos) {
	//look for the start of the next air column
	if (!hasBlock(pos)) {
		return false;
	}

	while (true) {
		auto t = getBlockID(pos);

		--pos.y;

		auto& mat = Block::mBlocks[t]->getMaterial();
	
		if (mat.isType(MaterialType::Air) || mat.isType(MaterialType::Water)) {
			break;
		}

		if (pos.y < 0) {
			return false;
		}
	}

	//look for the first solid block
	while (true) {
		auto& mat = getMaterial(pos);

		if (mat.isSolidBlocking()) {
			return true;//found

		}
		if (--pos.y < 0) {
			return false;
		}
	}
}

const BlockPos BlockSource::getTopRainBlockPos(const BlockPos& pos) {
	LevelChunk* c = getChunkAt(pos);
	return c ? c->getTopRainBlockPos(ChunkBlockPos(pos)) : BlockPos(-1, -1, -1);
}

bool BlockSource::checkIsTopRainBlockPos(const BlockPos& pos) {
	LevelChunk* c = getChunkAt(pos);
	return c && (pos == c->getTopRainBlockPos(ChunkBlockPos(pos)));
}

bool BlockSource::shouldFreezeIgnoreNeighbors(const BlockPos& pos) {
	return shouldFreeze(pos, false);
}

bool BlockSource::shouldFreeze(const BlockPos& pos, bool checkNeighbors) {
	const float temp = getBiome(pos).getTemperature();
	if(temp > Biome::RAIN_TEMP_THRESHOLD) {
		return false;
	}

// 	if(pos.y >= 0 && pos.y < mMaxHeight && getBrightness(LightLayer::BLOCK, pos) < 10) {
// 		const BlockID block = getBlockID(pos);
// 		if((block == Block::mStillWater->mID || block == Block::mFlowingWater->mID) && getData(pos) == 0) {
// 			if(!checkNeighbors) {
// 				return true;
// 			}
// 
// 			const bool surroundedByWater = _isWaterAt(pos.west()) && _isWaterAt(pos.east()) && _isWaterAt(pos.north()) && _isWaterAt(pos.south());
// 			if(!surroundedByWater) {
// 				return true;
// 			}
// 		}
// 	}
	return false;
}

bool BlockSource::shouldThaw(const BlockPos& pos, bool checkNeighbors) {
	const float temp = getBiome(pos).getTemperature();

	if (temp <= Biome::RAIN_TEMP_THRESHOLD) {
		return false;
	}

	if (pos.y >= 0 && pos.y < mMaxHeight) {
		const BlockID block = getBlockID(pos);
		if (block == Block::mIce->mID) {
			if (!checkNeighbors) {
				return true;
			}

			const bool surroundedByIce = _isIceAt(pos.west()) && _isIceAt(pos.east()) && _isIceAt(pos.north()) && _isIceAt(pos.south());
			if (!surroundedByIce) {
				return true;
			}
		}
	}
	return false;
}

bool BlockSource::_isWaterAt(const BlockPos& pos) {
	return getMaterial(pos).isType(MaterialType::Water);
}

const bool BlockSource::_isIceAt(const BlockPos& pos) {
	return getMaterial(pos).isType(MaterialType::Ice) || getMaterial(pos).isType(MaterialType::Dirt);
}

bool BlockSource::isSnowTemperature(const BlockPos& pos) {
	float temp = getBiome(pos).getTemperature();
	if (temp > Biome::RAIN_TEMP_THRESHOLD) {
		return false;
	}
	return true;
}

bool BlockSource::canSeeSkyFromBelowWater(BlockPos pos) {
	Dimension& d = getDimension();
	if (pos.y >= d.getSeaLevel()) {
		return canSeeSky(pos);
	}
	BlockPos scanPoint = BlockPos(Vec3((float)pos.x, (float)d.getSeaLevel(), (float)pos.z));
	if (canSeeSky(scanPoint)) {
		return false;
	}
	scanPoint = scanPoint.below();
	Block b = getBlock(scanPoint);
	while (scanPoint.y > pos.y) {
		if (getBrightness(pos) > 0 && !b.getMaterial().isLiquid()) {
			return false;
		}
		scanPoint = scanPoint.below();
	}
	return true;
}

bool BlockSource::isHumidAt(const BlockPos& pos){
	return getBiome(pos).isHumid();
}

bool BlockSource::hasBlock(int x, int y, int z) {
	return hasBlock(BlockPos(x, y, z));
}

BlockID BlockSource::getBlockID(int x, int y, int z){
	return getBlockID(BlockPos(x, y, z));
}

DataID BlockSource::getData(int x, int y, int z){
	return getData(BlockPos(x, y, z));
}

FullBlock BlockSource::getBlockAndData(int x, int y, int z){
	return getBlockAndData(BlockPos(x, y, z));
}

// Brightness BlockSource::getBrightness(const LightLayer& layer, int x, int y, int z){
// 	return getBrightness(layer, BlockPos(x, y, z));
// }

float BlockSource::getBrightness(int x, int y, int z){
	return getBrightness(BlockPos(x, y, z));
}

Brightness BlockSource::getRawBrightness(int x, int y, int z, bool propagate){
	return getRawBrightness(BlockPos(x, y, z), propagate);
}

Height BlockSource::getAboveTopSolidBlock(int x, int z, bool includeWater, bool includeLeaves) {
	return getAboveTopSolidBlock(BlockPos(x, mMaxHeight - 1, z), includeWater, includeLeaves);
}

bool BlockSource::canSeeSky(int x, int y, int z){
	return canSeeSky(BlockPos(x, y, z));
}

Height BlockSource::getHeightmap(int x, int z){
	return getHeightmap(BlockPos(x, 0, z));
}

bool BlockSource::setBlockAndData(int x, int y, int z, FullBlock block, int updateFlags){
	return setBlockAndData(BlockPos(x, y, z), block, updateFlags);
}

// void BlockSource::setBrightness(const LightLayer& layer, int x, int y, int z, Brightness brightness) {
// 	return setBrightness(layer, BlockPos(x, y, z), brightness);
// }

BlockEntity* BlockSource::getBlockEntity(int x, int y, int z){
	return getBlockEntity(BlockPos(x, y, z));
}

const Block& BlockSource::getBlock(int x, int y, int z) {
	return getBlock(BlockPos(x, y, z));
}

const Material& BlockSource::getMaterial(int x, int y, int z) {
	return getMaterial(BlockPos(x, y, z));
}

bool BlockSource::isOwnerThread() const {
	return std::this_thread::get_id() == mOwnerThreadID;
}

bool BlockSource::isPositionUnderLiquid(const BlockPos& p, MaterialType m) {
	auto& mat = getMaterial(p);
	if (mat.isLiquid() && mat.isType(m)) {
		float hh = LiquidBlock::getHeightFromData((int)getData(p)) - 1 / 9.0f;
		float h = p.y + 1 - hh;
		return p.y < h;
	}
	return false;
}

const bool BlockSource::getPublicSource() const {
	return mPublicSource;
}

void BlockSource::addUnloadedChunksAABBs(const AABB& box) {
	// Add bounding boxes for chunks that have not been completely loaded
	// including not populated by neighbours yet.
	// The boxes are 3 x 3 chunks big
	Bounds bounds(BlockPos(box.min) - CHUNK_WIDTH, BlockPos(box.max) + (CHUNK_WIDTH * 2), CHUNK_WIDTH, true);

	for (auto& c : bounds) {
		ChunkPos cPos(c.x, c.z);
		if (!getChunk(cPos)) {
			AABB aabb(BlockPos(cPos) - CHUNK_WIDTH, BlockPos(cPos) + (CHUNK_WIDTH * 2));
			aabb.max.y = mMaxHeight;
			aabb.min.y = 0;
			mTempCubeList.push_back(aabb);
		}
	}
}

void BlockSource::addVoidFloor(const AABB& box) {
	//prevent things from falling infinitely
	auto floor(box);
	floor.max.y = VOID_BOUNDARY;
	floor.min.y = -FLT_MAX;
	mTempCubeList.push_back(floor);
}