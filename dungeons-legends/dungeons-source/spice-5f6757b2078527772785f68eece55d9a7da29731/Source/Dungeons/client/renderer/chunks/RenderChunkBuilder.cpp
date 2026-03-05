/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/
#include "Dungeons.h"

#include "client/renderer/chunks/RenderChunkBuilder.h"

#include "world/level/chunk/ChunkSource.h"
#include "client/renderer/renderer/Tessellator.h"
// #include "world/level/LightLayer.h"
#include "world/level/BlockSource.h"
#include "client/renderer/chunks/RenderChunk.h"
#include "world/level/block/Block.h"
#include "client/renderer/chunks/VisibilityExtimator.h"
#include "world/level/chunk/ChunkViewSource.h"
#include "world/level/block/LeafBlock.h"
#include "world/level/block/OldLeafBlock.h"
#include "client/renderer/block/BlockTessellator.h"
#include "client/renderer/chunks/BlockQueue.h"
// #include "Renderer/GlobalConstantBufferManager.h"
#include "world/level/chunk/LevelChunk.h"
#include "world/level/dimension/Dimension.h"

typedef std::array<BlockQueue, BlockRenderLayer::_RENDERLAYER_COUNT> QueuePool;


// static ThreadLocal<QueuePool> queuePool([] (){
// 	return make_unique<QueuePool>();
// } );

RenderChunkBuilder::RenderChunkBuilder(ChunkSource& mainSource, Tessellator* t, MemoryTracker* tracker){
	//make a new chunkSource grabbing chunks from the main source
	localSource = make_unique<ChunkViewSource>(mainSource, ChunkSource::LoadMode::None);

	if (!t) {
		ownedTessellator = make_unique<Tessellator>(tracker);
		t = ownedTessellator.get();
	}

	renderer = make_unique<BlockTessellator>(*t);
}

RenderChunkBuilder::~RenderChunkBuilder() {

}

void RenderChunkBuilder::trim() {
	if (ownedTessellator) {
		ownedTessellator->trim();
	}
}

void RenderChunkBuilder::_checkAllDark(BlockSource& region, const BlockPos& p) {
	if (allDark) {
		if (auto lc = region.getChunkAt(p)) {
			if (lc->getRawBrightness(ChunkBlockPos(p.above()), Brightness::MIN) > 2) {
				allDark = false;
			}
		}
	}
}

Index RenderChunkBuilder::_setRange(TerrainLayer r, Index start, Index count) {
	return mTerrainLayerRange[static_cast<int>(r)].set(start, count);
}

Index RenderChunkBuilder::_setRenderLayerRange(BlockRenderLayer r, Index start, Index count) {
	return mRenderLayerRange[(int)r].set(start, count);
}

void RenderChunkBuilder::_checkPropagatedBrightness(BlockSource& source, const BlockPos& p) {
// 	if (!skyLit || allDark) {
// 		auto br = source.getBrightness(source.getDimension().getSkyLightLayer(), p);
// 
// 		if (br) {
// 			skyLit = true;
// 		}
// 
// 		if (allDark) {
// 			br += source.getBrightness(LightLayer::BLOCK, p);
// 			if (br > 2) {
// 				allDark = false;
// 			}
// 		}
// 	}
}

bool RenderChunkBuilder::_seesSkyDirectly(RenderChunk& rc, BlockSource& region) {
	BlockPos min = rc.getPosition();
	BlockPos max = min + RENDERCHUNK_SIDE;

	for (BlockPos p = min; p.x < max.x; p.x++) {
		for (p.z = min.z; p.z < max.z; p.z++) {
			p.y = region.getHeightmap(p);

			//walk down ignoring water
			while (Block::mTranslucency[region.getBlockID(p)] > 0 && p.y > 0) {
				--p.y;
			}

			if (p.y >= min.y && p.y <= max.y) {
				return true;
			}
		}
	}
	return false;
}

bool RenderChunkBuilder::_sortBlocks(BlockSource& region, RenderChunk& rc, VisibilityExtimator* VE, bool transparentLeaves) {

	BlockPos min = rc.getPosition();
	BlockPos max = min + RENDERCHUNK_SIDE;

	//put all the blocks on their queue
	for (BlockPos p = min; p.x < max.x; p.x++) {
		for (p.z = min.z; p.z < max.z; p.z++) {

			//check the brightness above the top of the column
			_checkPropagatedBrightness(region, p.above());

			for (p.y = min.y; p.y < max.y; p.y++) {
				auto& block = region.getBlock(p);

				if (!block.isType(Block::mAir)) {

					if (VE) {
						VE->setBlock(p, &block);
					}

					BlockRenderLayer renderLayer = block.getRenderLayer(region, p);
					if (block.hasProperty(BlockProperty::Leaf)) {
						bool isSeasonsTinted = (renderLayer == BlockRenderLayer::RENDERLAYER_SEASONS_OPAQUE || renderLayer == BlockRenderLayer::RENDERLAYER_SEASONS_OPTIONAL_ALPHATEST);

						int leafType = Block::mLeaves->getBlockState(BlockState::MappedType).get<int>(region.getData(p));
						if (leafType == enum_cast(OldLeafBlock::LeafType::Jungle)) {
							renderLayer = transparentLeaves && isSeasonsTinted ? BlockRenderLayer::RENDERLAYER_SEASONS_OPAQUE : BlockRenderLayer::RENDERLAYER_OPAQUE;
						}
						else if (LeafBlock::isDeepLeafBlock(region, p)) {
							renderLayer = transparentLeaves && isSeasonsTinted ? BlockRenderLayer::RENDERLAYER_SEASONS_OPAQUE : BlockRenderLayer::RENDERLAYER_OPAQUE;
						}
					}
					else if (block.canHaveExtraData()) {

						uint16_t extraData = region.getExtraData(p);
						if (extraData != 0) {
							auto& pBlock = Block::mBlocks[extraData & 0xFF];
							if (pBlock != nullptr) {
								mQueues[pBlock->getRenderLayer()].add(p);
							}
						}
					}

					int extraLayers = block.getExtraRenderLayers();
					if (extraLayers != 0) {
						for (int i = 0; i < enum_cast(BlockRenderLayer::_RENDERLAYER_COUNT); i++) {
							if ((extraLayers & (1 << i)) != 0) {
								mQueues[i].add(p);
							}
						}
					}

					mQueues[renderLayer].add(p);
				}

				_checkPropagatedBrightness(region, p);
			}

			_checkAllDark(region, p);
		}
	}

	return true;
}

bool RenderChunkBuilder::_tessellateQueues(RenderChunk& rc, BlockSource& region) {
	auto& t = renderer->getTessellator();

	for (int l = 0; l < BlockRenderLayer::_RENDERLAYER_COUNT; ++l) {

		if (!mQueues[l].empty()) {
			if (!t.isTessellating()) {	//lazy-begin tessellating
// 				t.begin(12000);	//made up average
				t.setOffset(-rc.getPosition());
			}

			auto existingVertices = t.getVertexCount();

			renderer->setRenderLayer(l);
			renderer->setForceOpaque(l == BlockRenderLayer::RENDERLAYER_DOUBLE_SIDED);
			renderer->resetCache(rc.getPosition(), region);

			for (BlockPos& pos : mQueues[l]) {
				FullBlock fullBlock = region.getBlockAndData(pos);
				const Block* block = Block::mBlocks[fullBlock.id];
				renderer->tessellateInWorld(*block, pos, fullBlock.data);
			}
			renderer->setForceOpaque(false);

			mQueues[l].setIndexCount((t.getVertexCount() - existingVertices) / 4 * 6);
		}
	}

	return true;
}

void RenderChunkBuilder::_buildRanges() {

	//calculate the "near" index ranges
	memset(mTerrainLayerRange, 0, sizeof(mTerrainLayerRange));
	memset(mRenderLayerRange, 0, sizeof(mRenderLayerRange));

	//WARNING: the order in which _setRenderLayerRange is called is *VERY* important and depends on the ordering of the renderlayers!!
	int offset = 0;
	offset = _setRenderLayerRange(BlockRenderLayer::RENDERLAYER_DOUBLE_SIDED, offset, mQueues[BlockRenderLayer::RENDERLAYER_DOUBLE_SIDED].getIndexCount());
	offset = _setRenderLayerRange(BlockRenderLayer::RENDERLAYER_BLEND, offset, mQueues[BlockRenderLayer::RENDERLAYER_BLEND].getIndexCount());
	offset = _setRenderLayerRange(BlockRenderLayer::RENDERLAYER_WATER, offset, mQueues[BlockRenderLayer::RENDERLAYER_WATER].getIndexCount());
	offset = _setRenderLayerRange(BlockRenderLayer::RENDERLAYER_OPAQUE, offset, mQueues[BlockRenderLayer::RENDERLAYER_OPAQUE].getIndexCount());
	offset = _setRenderLayerRange(BlockRenderLayer::RENDERLAYER_ENDPORTAL, offset, mQueues[BlockRenderLayer::RENDERLAYER_ENDPORTAL].getIndexCount());
	offset = _setRenderLayerRange(BlockRenderLayer::RENDERLAYER_OPTIONAL_ALPHATEST, offset, mQueues[BlockRenderLayer::RENDERLAYER_OPTIONAL_ALPHATEST].getIndexCount());
	offset = _setRenderLayerRange(BlockRenderLayer::RENDERLAYER_ALPHATEST, offset, mQueues[BlockRenderLayer::RENDERLAYER_ALPHATEST].getIndexCount());
	offset = _setRenderLayerRange(BlockRenderLayer::RENDERLAYER_SEASONS_OPAQUE, offset, mQueues[BlockRenderLayer::RENDERLAYER_SEASONS_OPAQUE].getIndexCount());
	offset = _setRenderLayerRange(BlockRenderLayer::RENDERLAYER_SEASONS_OPTIONAL_ALPHATEST, offset, mQueues[BlockRenderLayer::RENDERLAYER_SEASONS_OPTIONAL_ALPHATEST].getIndexCount());
	offset = _setRenderLayerRange(BlockRenderLayer::RENDERLAYER_ALPHATEST_SINGLE_SIDE, offset, mQueues[BlockRenderLayer::RENDERLAYER_ALPHATEST_SINGLE_SIDE].getIndexCount());

	mRenderChunkSorter.setupIndexRanges(mTerrainLayerRange, mRenderLayerRange);
}


VisibilityExtimator* RenderChunkBuilder::_getVisibilityHelper() {
	// TODO: rherlitz
// 	return visibilityChanged ? &VisibilityExtimator::pool.getLocal() : nullptr;
	return nullptr;
}

void RenderChunkBuilder::build(RenderChunk& rc, bool transparentLeaves, bool smoothLighting) {

	//create a new BlockSource
	BlockSource region(localSource->getLevel(), localSource->getDimension(), *localSource, false);

	DEBUG_ASSERT(rc.isBuildState(RenderChunk::BuildState::Building), "Wrong state, wrong thread");

	if (auto lc = region.getChunk(rc.getCenter())) {
		lc->updateCachedData(region);
	}

	//get our local visibility extimator if needed
	auto VE = _getVisibilityHelper();
	if (VE) {
		VE->start(rc);
	}

	{
		renderer->setRegion(region);
		renderer->setSmoothLightingEnabled(smoothLighting);
		skyLit = _seesSkyDirectly(rc, region);
		allDark = true;
// 		mQueues = queuePool.getLocal().data();

		for (int i = 0; i < BlockRenderLayer::_RENDERLAYER_COUNT; ++i) {
			mQueues[i].reset();
		}
	}

	if (!_sortBlocks(region, rc, VE, transparentLeaves)) {
		return;
	}

	averageSkyLight = skyLit ? 15.f : 0;

	if (!_tessellateQueues(rc, region)) {
		return;
	}

	_buildRanges();

	if (VE) {
		mVisibility = VE->finish();
	}
}
