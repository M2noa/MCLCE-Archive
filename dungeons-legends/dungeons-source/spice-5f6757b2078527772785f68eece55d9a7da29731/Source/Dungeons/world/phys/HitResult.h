/********************************************************
*   (c) Mojang. All rights reserved                       *
*   (c) Microsoft. All rights reserved.                   *
*********************************************************/
#pragma once

#include "world/phys/Vec3.h"
#include "world/level/BlockPos.h"

class Entity;

enum HitResultType {
	TILE,
	ENTITY,
	ENTITY_OUT_OF_RANGE, // needed to position the cursor on an entity, but not allow interaction
	NO_HIT
};

class HitResult {
public:

	HitResult();
	explicit HitResult(const Vec3& rayEnd);
	HitResult(const BlockPos& blockPos, FacingID facing, const Vec3& pos);
	explicit HitResult(Entity& entity);
	HitResult(Entity& entity, const Vec3& pos);
	HitResult(const HitResult& hr);

	void setIsHitLiquid(bool isHit, HitResult liquidHit) {
		mIsHitLiquid = isHit; 
		if (isHit) {
			mLiquidFacing = liquidHit.mFacing;
			mLiquid = liquidHit.mBlock;
			mLiquidPos = liquidHit.mPos;
		}
	}

	void setIndirectHit() { mIndirectHit = true; }

	void setFacing(FacingID facing) { mFacing = facing; }

	float distanceTo(const Entity& otherEntity) const;

	void resetHitType() { mType = NO_HIT; }

	void resetHitEntity() { mEntity = nullptr; }

	// causes the hit type to be "NO_HIT" if no entity, otherwise "ENTITY_OUT_OF_RANGE"
	void setOutOfRange();

	bool isHit() const {
		return mType != NO_HIT && mType != ENTITY_OUT_OF_RANGE;
	}

	bool isEntity() const {
		return mType == ENTITY;
	}

	bool isEntityOutOfRange() const {
		return mType == ENTITY_OUT_OF_RANGE;
	}

	bool isTile() const {
		return mType == TILE;
	}

	bool isHitLiquid() const {
		return mIsHitLiquid;
	}

	const Vec3& getPos() const {
		return mPos;
	}

	const Vec3& getLiquidPos() const {
		return mLiquidPos;
	}

	Entity* getEntity() const {
		return mEntity;
	}

	const BlockPos& getBlock() const {
		return mBlock;
	}

	const BlockPos& getLiquid() const {
		return mLiquid;
	}

	bool isIndirectHit() const {
		return mIndirectHit;
	}

	HitResultType getHitResultType() {
		return mType;
	}

	FacingID getFacing() const {
		return mFacing;
	}

	FacingID getLiquidFacing() const {
		return mLiquidFacing;
	}

private:
	HitResultType mType;
	FacingID mFacing;
	BlockPos mBlock;
	Vec3 mPos;
	Entity* mEntity;

	bool mIsHitLiquid;
	FacingID mLiquidFacing;
	BlockPos mLiquid;
	Vec3 mLiquidPos;

	bool mIndirectHit;	// when targeting the block we're standing on, without actually looking at it
};
