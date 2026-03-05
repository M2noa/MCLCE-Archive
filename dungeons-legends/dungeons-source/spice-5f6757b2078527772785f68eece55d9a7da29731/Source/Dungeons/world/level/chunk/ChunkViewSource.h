/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/
#pragma once

#include "common_header.h"

#include "world/level/chunk/ChunkSource.h"
#include "util/GridArea.h"
#include "ChunkSourceView.h"

class ChunkViewSource : public ChunkSource {
public:

	ChunkViewSource(ChunkSource& parent, LoadMode parentLoadMode = LoadMode::Deferred);

	virtual LevelChunk* getExistingChunk(const ChunkPos& pos) override;

	void move(const BlockPos& blockMin, const BlockPos& blockMax );
	void move(const BlockPos& center, int radius);

	void clear();

	ChunkSourceView& getArea();

	Unique<CompoundTag> getStructureTag() const override;
	void readStructureTag(const CompoundTag& tag) override;

protected:

	LoadMode mParentLoadMode;
	ChunkSourceView mArea;
};
