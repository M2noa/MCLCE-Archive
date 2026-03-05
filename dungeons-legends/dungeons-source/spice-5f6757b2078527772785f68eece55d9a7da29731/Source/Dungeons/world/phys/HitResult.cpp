#include "Dungeons.h"

#include "world/phys/HitResult.h"
#include "world/entity/Entity.h"
#include "HitResult.h"

HitResult::HitResult(const Vec3& rayEnd)
	:   mType(NO_HIT)
	, mIsHitLiquid(false)
	, mEntity(nullptr)
	, mIndirectHit(false)
	, mFacing(0)
	, mPos(rayEnd) {
}

HitResult::HitResult() : HitResult(Vec3::ZERO) {}

HitResult::HitResult(const BlockPos& blockPos, FacingID facing, const Vec3& pos)
	:   mType(TILE)
	, mIsHitLiquid(false)
	, mBlock(blockPos)
	, mFacing(facing)
	, mPos(pos)
	, mIndirectHit(false)
	, mEntity(nullptr){
}

HitResult::HitResult(Entity& entity)
	:   mType(ENTITY)
	, mIsHitLiquid(false)
	, mEntity(&entity)
	, mPos(entity.mPos)
	, mIndirectHit(false)
	, mFacing(0){
}

HitResult::HitResult(const HitResult& hr)
	:   mType(hr.mType)
	, mIsHitLiquid(hr.mIsHitLiquid)
	, mBlock(hr.mBlock)
	, mLiquid(hr.mLiquid)
	, mFacing(hr.mFacing)
	, mLiquidFacing(hr.mLiquidFacing)
	, mPos(hr.mPos)
	, mLiquidPos(hr.mLiquidPos)
	, mIndirectHit(hr.mIndirectHit)
	, mEntity(hr.mEntity) {
}

HitResult::HitResult(Entity& entity, const Vec3& pos)
	:  mType(ENTITY)
	, mIsHitLiquid(false)
	, mEntity(&entity)
	, mPos(pos)
	, mIndirectHit(false)
	, mFacing(0){
}


float HitResult::distanceTo(const Entity& otherEntity) const {
	const float xd = mPos.x - otherEntity.mPos.x;
	const float yd = mPos.y - otherEntity.mPos.y;
	const float zd = mPos.z - otherEntity.mPos.z;
	return xd * xd + yd * yd + zd * zd;
}

void HitResult::setOutOfRange() {
	if (mEntity != nullptr) {
		mType = ENTITY_OUT_OF_RANGE;
	}
	else {
		mType = NO_HIT;
	}
}
