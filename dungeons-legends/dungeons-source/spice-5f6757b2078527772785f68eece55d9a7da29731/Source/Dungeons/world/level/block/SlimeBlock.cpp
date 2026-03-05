/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#include "world/level/block/SlimeBlock.h"
#include "world/entity/Entity.h"
#include "world/entity/Mob.h"
#include "world/level/material/Material.h"

SlimeBlock::SlimeBlock(const std::string& nameId, int id, const Material& material) 
	: HalfTransparentBlock(nameId, id, material, false) {
	mProperties = mProperties | BlockProperty::Slime | BlockProperty::TopSolidBlocking | BlockProperty::SolidBlocking;
	mFriction = 0.8f;
	mTranslucency[id] = std::max(0.8f, mMaterial.getTranslucency());
	mRenderLayer = RENDERLAYER_BLEND;
	setSolid(true);
}

void SlimeBlock::onFallOn(BlockSource& region, const BlockPos& pos, Entity* entity, float fallDistance) const {
	if (!entity->isSneaking()) {
		// Reset fall distance to prevent damage
		entity->mFallDistance = 0;
	}
	fallDistance = 0.0f;

	Block::onFallOn(region, pos, entity, fallDistance);
}

void SlimeBlock::onStepOn(Entity& entity, const BlockPos& pos) const {
	if (std::abs(entity.mPosDelta.y) < 0.1f && !entity.isSneaking()) {
		float scale = 0.4f + std::abs(entity.mPosDelta.y) * 0.2f;
		entity.mPosDelta.x *= scale;
		entity.mPosDelta.z *= scale;
	}

	HalfTransparentBlock::onStepOn(entity, pos);
}

void SlimeBlock::updateEntityAfterFallOn(Entity& entity) const {
	if (entity.isSneaking()) {
		HalfTransparentBlock::updateEntityAfterFallOn(entity);
	} else {
		if (entity.mPosDelta.y < 0.0f) {
			entity.mPosDelta.y *= -1;
// 			entity.onBounceStarted(entity.getPos(), mID);
		}
	}
}

int SlimeBlock::getExtraRenderLayers() const {
	return (1 << RENDERLAYER_OPAQUE);
}
