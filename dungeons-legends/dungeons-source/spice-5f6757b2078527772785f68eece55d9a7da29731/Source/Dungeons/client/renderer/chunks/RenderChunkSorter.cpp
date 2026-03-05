/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/
#include "Dungeons.h"

#include "client/renderer/chunks/RenderChunkSorter.h"

#include "world/level/chunk/ChunkSource.h"
#include "client/renderer/renderer/Tessellator.h"
#include "client/renderer/chunks/RenderChunk.h"
#include "world/level/chunk/ChunkViewSource.h"
#include "client/renderer/block/BlockTessellator.h"

RenderChunkSorter::RenderChunkSorter(){
}

RenderChunkSorter::~RenderChunkSorter() {

}

void RenderChunkSorter::setupIndexRanges(RangeIndices terrainLayerRange[], RangeIndices renderLayerRange[]) {
	terrainLayerRange[enum_cast(TerrainLayer::DoubleSide)] = renderLayerRange[BlockRenderLayer::RENDERLAYER_DOUBLE_SIDED];
	terrainLayerRange[enum_cast(TerrainLayer::Blend)] = renderLayerRange[BlockRenderLayer::RENDERLAYER_BLEND];
	terrainLayerRange[enum_cast(TerrainLayer::Water)] = renderLayerRange[BlockRenderLayer::RENDERLAYER_WATER];
	terrainLayerRange[enum_cast(TerrainLayer::Opaque)] = renderLayerRange[BlockRenderLayer::RENDERLAYER_OPAQUE];
	terrainLayerRange[enum_cast(TerrainLayer::Alpha)].mStart = renderLayerRange[BlockRenderLayer::RENDERLAYER_OPTIONAL_ALPHATEST].mStart;
	terrainLayerRange[enum_cast(TerrainLayer::Alpha)].mCount = renderLayerRange[BlockRenderLayer::RENDERLAYER_OPTIONAL_ALPHATEST].mCount +
		renderLayerRange[BlockRenderLayer::RENDERLAYER_ALPHATEST].mCount;
	terrainLayerRange[enum_cast(TerrainLayer::EndPortal)] = renderLayerRange[BlockRenderLayer::RENDERLAYER_ENDPORTAL];
	terrainLayerRange[enum_cast(TerrainLayer::AlphaSingleSide)] = renderLayerRange[BlockRenderLayer::RENDERLAYER_ALPHATEST_SINGLE_SIDE];
	terrainLayerRange[enum_cast(TerrainLayer::Far)].mStart = renderLayerRange[BlockRenderLayer::RENDERLAYER_BLEND].mStart;
	terrainLayerRange[enum_cast(TerrainLayer::Far)].mCount = renderLayerRange[BlockRenderLayer::RENDERLAYER_BLEND].mCount +
		renderLayerRange[BlockRenderLayer::RENDERLAYER_WATER].mCount +
		renderLayerRange[BlockRenderLayer::RENDERLAYER_OPAQUE].mCount +
		renderLayerRange[BlockRenderLayer::RENDERLAYER_OPTIONAL_ALPHATEST].mCount;
	terrainLayerRange[enum_cast(TerrainLayer::OpaqueSeasons)] = renderLayerRange[BlockRenderLayer::RENDERLAYER_SEASONS_OPAQUE];
	terrainLayerRange[enum_cast(TerrainLayer::AlphaSeasons)] = renderLayerRange[BlockRenderLayer::RENDERLAYER_SEASONS_OPTIONAL_ALPHATEST];
	terrainLayerRange[enum_cast(TerrainLayer::FarSeasons)] = renderLayerRange[BlockRenderLayer::RENDERLAYER_SEASONS_OPAQUE];
	terrainLayerRange[enum_cast(TerrainLayer::FarSeasonsAlpha)] = renderLayerRange[BlockRenderLayer::RENDERLAYER_SEASONS_OPTIONAL_ALPHATEST];
}

void RenderChunkSorter::sortFaces(RenderChunk& rc) {
	if (! rc.isSortState(RenderChunk::SortState::Sorting)) {
		return;
	}
	rc.sortFaces();
}