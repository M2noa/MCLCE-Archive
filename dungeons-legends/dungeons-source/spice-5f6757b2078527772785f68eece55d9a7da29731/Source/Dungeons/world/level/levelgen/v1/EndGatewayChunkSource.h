/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/
#pragma once

#include "world/level/chunk/ChunkViewSource.h"

class EndGatewayChunkSource : public ChunkViewSource {
public:

	EndGatewayChunkSource(ChunkSource& parent, const BlockPos& entryPortal, const BlockPos& exitOrigin, int radius);
	virtual ~EndGatewayChunkSource();

	const BlockPos& getEntryPos() const;
	const BlockPos& getExitOrigin() const;
	int getRadius() const;

	Unique<CompoundTag> getStructureTag() const override;
	void readStructureTag(const CompoundTag& tag) override;

protected:
	BlockPos mEntryPortal;
	BlockPos mOrigin;
	int mRadius;
};
