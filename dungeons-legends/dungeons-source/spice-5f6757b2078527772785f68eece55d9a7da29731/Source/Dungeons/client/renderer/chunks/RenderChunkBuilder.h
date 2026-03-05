/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/
#pragma once

#include "common_header.h"

#include "CommonTypes.h"
#include "client/renderer/chunks/VisibilityNode.h"
#include "client/renderer/renderer/TerrainLayer.h"
#include "client/renderer/chunks/RangeIndices.h"
#include "world/level/LevelConstants.h"
#include "world/level/BlockPos.h"
#include "world/level/block/Block.h"
#include "client/renderer/chunks/RenderChunkSorter.h"

#include <array>
// #include "platform/threading/ThreadLocal.h"

class ChunkSource;
class Tessellator;
class MemoryTracker;
class ChunkViewSource;
class BlockTessellator;
class RenderChunk;
class BlockSource;
class BlockPos;
class RebuildData;
class BlockQueue;
class VisibilityExtimator;

class RenderChunkBuilder {
public:

	Unique<ChunkViewSource> localSource;
	Unique<BlockTessellator> renderer;

	bool visibilityChanged, allDark, skyLit;
	float averageSkyLight;
	VisibilityNode mVisibility;

	RenderChunkBuilder(ChunkSource& mainSource, Tessellator* maybeTessellator = nullptr, MemoryTracker* tracker = nullptr);
	RenderChunkBuilder() = delete;
	RenderChunkBuilder(const RenderChunkBuilder&) = delete;

	~RenderChunkBuilder();

	void build(RenderChunk& rc, bool transparentLeaves, bool smoothLighting);

	VisibilityExtimator* _getVisibilityHelper();


	RangeIndices mTerrainLayerRange[enum_cast(TerrainLayer::Count)];
	RangeIndices mRenderLayerRange[enum_cast(BlockRenderLayer::_RENDERLAYER_COUNT)];

	void trim();

protected:

	BlockQueue* mQueues = nullptr;

	Unique<Tessellator> ownedTessellator;
	RenderChunkSorter mRenderChunkSorter;

	Index _setRange(TerrainLayer r, Index start, Index count);
	Index _setRenderLayerRange(BlockRenderLayer r, Index start, Index count);

	void _checkPropagatedBrightness(BlockSource& source, const BlockPos& p);
	bool _seesSkyDirectly(RenderChunk& rc, BlockSource& region);
	bool _sortBlocks(BlockSource& region, RenderChunk& rc, VisibilityExtimator* VE, bool transparentLeaves);
	bool _tessellateQueues(RenderChunk& rc, BlockSource& region);
	void _buildRanges();
	void _checkAllDark(BlockSource& region, const BlockPos& p);

};
