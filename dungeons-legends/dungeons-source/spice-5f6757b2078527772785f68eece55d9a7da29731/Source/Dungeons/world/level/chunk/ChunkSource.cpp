/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/
#include "Dungeons.h"

#include "world/level/chunk/ChunkSource.h"

#include "world/level/chunk/LevelChunk.h"
//#include "platform/threading/BackgroundWorker.h"
//#include "platform/threading/WorkerPool.h"
#include "world/level/block/BlockVolume.h"
#include "world/level/dimension/Dimension.h"
#include "world/level/chunk/ChunkViewSource.h"
#include "world/level/BlockSource.h"
#include "world/level/Level.h"
#include "world/level/biome/Biome.h"
#include "world/entity/player/Player.h"
#include "util/Boxed.h"
// #include "util/PerfTimer.h"

ChunkSourceView ChunkSource::getView(const BlockPos& min_, const BlockPos& max_, LoadMode lm, bool circle, const ChunkSourceView::AddCallback& add ) {

	///ensure that the view is flat
	BlockPos max(max_), min(min_);
	max.y = min.y = 0;

	return ChunkSourceView(min, max, CHUNK_WIDTH,
		[lm, this] (const Pos& cp) {
		return getOrLoadChunk( ChunkPos(cp.x, cp.z), lm);
	},
		[lm, this] (LevelChunk*& removed) {
		releaseChunk(*removed);
	},
		circle, add );
}

ChunkSourceView ChunkSource::getView(const BlockPos& pos, int radius, LoadMode lm, bool circle, const ChunkSourceView::AddCallback& add) {
	return getView(pos - radius, pos + radius, lm, circle, add);
}

int ChunkSource::_getChunkPriority(const LevelChunk& lc) {
	using namespace std::chrono;

	if (!mLevel) {
		return 0;
	}

	float mindist = FLT_MAX;
	Vec3 fpos = lc.getMin();
	fpos.y = 0.0f; // ensure we don't get a non-zero element in the Y value that gets used in the distance function

	mLevel->forEachPlayer([&mindist, &fpos](Player& player) {
		Vec3 playerPos = player.getPos();
		playerPos.y = 0.0f; // ensure that player height doesn't affect the distance function
		mindist = std::min(mindist, fpos.distanceToSqr(playerPos));
		return true;
	});

	return (int)sqrtf(mindist) + (int)duration_cast<seconds>(steady_clock::now().time_since_epoch()).count() * 16;
}

void ChunkSource::_fireChunkLoaded(LevelChunk& lc) {
// 	DEBUG_ASSERT(lc.getState() == ChunkState::Loaded, "Wrong state");

	if (mLevel) {
		mLevel->onChunkLoaded(lc);
	}
}

LevelChunk* ChunkSource::getOrLoadChunk(const ChunkPos& cp, LoadMode lm) {
	auto lc = requestChunk(cp, lm);

// 	DEBUG_ASSERT(lc || lm == LoadMode::None, "this call must always create a chunk if not LoadMode::None");

	//load it
	if ((!mLevel || (!mLevel->isClientSide() && !mLevel->getTearingDown())) && lc && lc->getState() == ChunkState::Unloaded && lm == LoadMode::Deferred) {

		//grab the chunk again while we load
		requestChunk(cp, LoadMode::None);

		//queue the loading
		auto succeeded = make_shared<bool>(false);
		bool immediatelyLoadReplacementData = lm == LoadMode::Deferred;

		// Hack
		loadChunk(*lc, immediatelyLoadReplacementData);
		bool released = releaseChunk(*lc);
		if (!released && *succeeded) {
			_startPostProcessingArea(*lc);
		}

		// TODO: rherlitz test
// 		if (lc->tryChangeState(ChunkState::Unloaded, ChunkState::Generating)) {
		 	loadChunk(*lc, immediatelyLoadReplacementData);
		 	*succeeded = true;
// 		}
		released = releaseChunk(*lc);
		if (!released && *succeeded) {
			_startPostProcessingArea(*lc);
		}

		// TODO
// 		WorkerPool::getFor(WorkerRole::Streaming).queue(
// 			[this, lc, succeeded, immediatelyLoadReplacementData] () {
// 				ScopedProfile("loadChunk");
// 				if (lc->tryChangeState(ChunkState::Unloaded, ChunkState::Generating)) {	//check if this chunk isn't already
// 																						// being loaded by the main thread
// 
// 					// if we're deferred, we're special case, thus immediately load any replacement data, 
// 					// since post-process is not going to have access to the db system
// 					// this is a fix that allows old levels with border limits to still allow modifications to chunks next to the border
// 					loadChunk(*lc, immediatelyLoadReplacementData);
// 					*succeeded = true;
// 				}
// 				return true;
// 			},
// 			[this, lc, succeeded] () {
// 				ScopedProfile("loadChunk callback");
// 				bool released = releaseChunk(*lc);
// 				if (!released && *succeeded) {
// 					_startPostProcessingArea(*lc);
// 				}
// 			},
// 			32 + _getChunkPriority(*lc)
// 		);
	}

	return lc;
}

void ChunkSource::_startPostProcessingArea(LevelChunk& center) {
	if (center.getState() == ChunkState::Loaded) {
		_fireChunkLoaded(center);
	}

	static const ChunkPos offsets[] = {
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

	for (auto& offset : offsets) {
		LevelChunk* n = getGeneratedChunk(center.getPosition() + offset);
		if (n && (n->getState() == ChunkState::Generated)) {
			_startPostProcessing(*n);
		}
	}
}

void ChunkSource::_startPostProcessing(LevelChunk& lc) {
	DEBUG_ASSERT(lc.getState() >= ChunkState::Generated, "Provided chunk is not in the right state");

	if (mLevel && mLevel->getTearingDown()) {
		return;
	}

	//acquire the chunks on the main thread
	const BlockPos chunkdim(CHUNK_WIDTH, CHUNK_WIDTH, CHUNK_WIDTH);

	//create a view around lc
	auto chunks = make_shared<ChunkViewSource>(*this, LoadMode::None);
	chunks->move(lc.getMin() - chunkdim, lc.getMax() + chunkdim);

	//check that all the chunks are ready to be loaded
	for (auto&& c : chunks->getArea()) {
		if (!c || c->getState() < ChunkState::Generated) {
			return;
		}
	}

	auto succeeded = make_shared<bool>(false);
// 	WorkerPool::getFor(WorkerRole::Streaming).queue(
// 		[this, &lc, chunks, succeeded] () {
// 
// 			ScopedProfile("postProcess");
// 			if (lc.tryChangeState(ChunkState::Generated, ChunkState::PostProcessing)) {
// 				if (lc.getGenerator()->postProcess(*chunks) == false) {
// 					lc.changeState(ChunkState::PostProcessing, ChunkState::Generated);	//go back and try again
// 					return false;
// 				}
// 
// 				lc.changeState(ChunkState::PostProcessing, ChunkState::Loaded);
// 				*succeeded = true;
// 			}
// 
// 			return true;
// 		},
// 		[this, &lc, chunks, succeeded] () {
// 			ScopedProfile("main::postProcess");
// 			if (*succeeded) {
// 				_fireChunkLoaded(lc);
// 			}
// 
// 			chunks->clear();//clear up the acquired chunks (explicitly to force the compiler to acquire the shared ptr)
// 		},
// 		16 + _getChunkPriority(lc)
// 	);
}

std::string ChunkSource::getFeatureName(StructureFeatureType f) {
	static std::map<StructureFeatureType, std::string> typeToName = {
		{ StructureFeatureType::EndCity,	"endcity"},
		{ StructureFeatureType::Fortress,	"fortress"},
		{ StructureFeatureType::Mineshaft,	"mineshaft" },
		{ StructureFeatureType::Monument,	"monument" },
		{ StructureFeatureType::Stronghold,	"stronghold" },
		{ StructureFeatureType::Temple,		"temple" },
		{ StructureFeatureType::Village,	"village" }
	};

	auto s = typeToName.find(f);
	DEBUG_ASSERT(s != typeToName.end(), "That's not a valid feature!");

	return s->second;
}

StructureFeatureType ChunkSource::getFeatureId(const std::string& s) {
	static std::map<std::string, StructureFeatureType> nameToType = {
		{ "endcity",	StructureFeatureType::EndCity },
		{ "fortress",	StructureFeatureType::Fortress },
		{ "mineshaft",	StructureFeatureType::Mineshaft },
		{ "monument",	StructureFeatureType::Monument },
		{ "stronghold",	StructureFeatureType::Stronghold },
		{ "temple",		StructureFeatureType::Temple },
		{ "village",	StructureFeatureType::Village }
	};

	auto f = nameToType.find(s);
	DEBUG_ASSERT(f != nameToType.end(), "That's not a valid feature!");

	return f->second;
}

ChunkSource::ChunkSource(Level* level, Dimension* dimension, int side)
	: mLevel(level)
	, mDimension(dimension)
	, mChunkSide(side) {

}

ChunkSource::ChunkSource(ChunkSource& parent) 
	: mLevel(parent.mLevel)
	, mDimension(parent.mDimension)
	, mParent(&parent)
	, mChunkSide(parent.getChunkSide()) {

}

ChunkSource::ChunkSource(Unique<ChunkSource> parent) 
	: ChunkSource(*parent) {
	DEBUG_ASSERT(parent, "Don't pass an empty pointer dude");

	mOwnedParent = std::move(parent);
}

ChunkSource::~ChunkSource() {
}

Level& ChunkSource::getLevel() const {
	return *mLevel;
}

LevelChunk* ChunkSource::getExistingChunk(const ChunkPos& cp) {
	UNUSED_PARAMETER(cp);
	return nullptr;
}

LevelChunk* ChunkSource::requestChunk( const ChunkPos& cp, LoadMode lm ) {
	return mParent ? mParent->requestChunk(cp, lm) : nullptr;
}

bool ChunkSource::releaseChunk(LevelChunk& lc) {
	if (mParent) {
		return mParent->releaseChunk(lc);
	}
	else{
		return false;
	}
}

bool ChunkSource::postProcess(ChunkViewSource& neighborhood) {
	UNUSED_PARAMETER(neighborhood);
	DEBUG_FAIL("This chunk source can't postprocess, but can load?");
	return true;
}

void ChunkSource::loadChunk(LevelChunk& lc, bool forceImmediateReplacementDataLoad) {
	if (mParent) {
		mParent->loadChunk(lc, forceImmediateReplacementDataLoad);
	}
}

void ChunkSource::hintDiscardBatchBegin() {
	if (mParent) {
		mParent->hintDiscardBatchBegin();
	}
}

void ChunkSource::hintDiscardBatchEnd() {
	if (mParent) {
		mParent->hintDiscardBatchEnd();
	}
}

void ChunkSource::acquireDiscarded(ChunkPtr&& ptr) {
	if (mParent) {
		mParent->acquireDiscarded(std::move(ptr));
	}
}

void ChunkSource::waitDiscardFinished() {
	if(mParent) {
		mParent->waitDiscardFinished();
	}
}

Dimension& ChunkSource::getDimension() const {
	return *mDimension;
}

ChunkSource* ChunkSource::getParent() const {
	return mParent;
}

int ChunkSource::getChunkSide() const {
	return mChunkSide;
}

LevelChunk* ChunkSource::getAvailableChunk(const ChunkPos& cp) {
	auto lc = getExistingChunk(cp);
	return (lc && lc->getState() >= ChunkState::Loaded) ? lc : nullptr;
}

LevelChunk* ChunkSource::getGeneratedChunk(const ChunkPos& cp) {
	auto lc = getExistingChunk(cp);
	return (lc && lc->getState() >= ChunkState::Generated) ? lc : nullptr;
}

LevelChunk* ChunkSource::getAvailableChunkAt(const BlockPos& pos) {
	return getAvailableChunk(pos);
}

const ChunkRefCount::Map& ChunkSource::getStoredChunks() const {
	DEBUG_ASSERT(mParent != nullptr, "Method not implementable here");
	return mParent->getStoredChunks();
}

ChunkRefCount::Map& ChunkSource::getStoredChunks() {
	DEBUG_ASSERT(mParent != nullptr, "Method not implementable here");
	return mParent->getStoredChunks();
}

void ChunkSource::discard(ChunkPtr& ptr) {
	if (mParent) {
		mParent->acquireDiscarded(std::move(ptr));
	}
}

void ChunkSource::discard(LevelChunk& lc) {
	ChunkPtr p(&lc);
	discard(p);
}

bool ChunkSource::findNearestFeature(StructureFeatureType feature, Player& player, BlockPos& pos) {
	UNUSED_PARAMETER(feature,player,pos);
	return false;
}

void ChunkSource::prepareHeights(BlockVolume& blocks, int xOffs, int zOffs) {
	if (mParent) {
		mParent->prepareHeights(blocks, xOffs, zOffs);
	}
}

bool ChunkSource::saveLiveChunk(LevelChunk& lc) {
	return mParent ? mParent->saveLiveChunk(lc) : false;
}

void ChunkSource::compact() {
	if (mParent) {
		mParent->compact();
	}
}

void ChunkSource::postProcessMobsAt(BlockSource* blockSource, int chunkWestBlock, int chunkNorthBlock, Random& random) {
	if (mParent) {
		mParent->postProcessMobsAt(blockSource, chunkWestBlock, chunkNorthBlock, random);
	}
}

const MobList& ChunkSource::getMobsAt(BlockSource& blockSource, EntityType category, const BlockPos& pos) {
	if (mParent) {
		return mParent->getMobsAt(blockSource, category, pos);
	}
	else{
		return Biome::EMPTY_MOBLIST;
	}
}
