/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/
#pragma once

#include "common_header.h"

#include "client/renderer/renderer/TerrainLayer.h"
#include "client/renderer/chunks/RangeIndices.h"

class RenderChunk;

class RenderChunkSorter {
public:

	RenderChunkSorter();
	RenderChunkSorter(const RenderChunkSorter&) = delete;

	~RenderChunkSorter();

	void sortFaces(RenderChunk& rc);
	void setupIndexRanges(RangeIndices terrainLayerRange[], RangeIndices renderLayerRange[]);

};
