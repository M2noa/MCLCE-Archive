/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/
#pragma once

#include "common_header.h"

#include "world/level/chunk/ChunkViewSource.h"

class PlayerChunkSource : public ChunkViewSource {
public:

	PlayerChunkSource(Player& origin, ChunkSource& parent, int radius);

	virtual ~PlayerChunkSource();

	int getRadius() const;

	///sets a new radius, loads what is missing & discards what's not needed
	void setRadius(int newRadius);

	void centerAt(const Vec3& pos, float minDist = 16.f);

	///gets the new position of the entity and moves the view if needed
	void center( float minDist = 16.f );

	void suspend();

	Unique<CompoundTag> getStructureTag() const override;
	void readStructureTag(const CompoundTag& tag) override;

protected:

	Player& mOrigin;
	int mRadius;
	bool mSuspended;
	Vec3 mLastChunkUpdatePos = Vec3(-FLT_MAX, -FLT_MAX, -FLT_MAX );
};
