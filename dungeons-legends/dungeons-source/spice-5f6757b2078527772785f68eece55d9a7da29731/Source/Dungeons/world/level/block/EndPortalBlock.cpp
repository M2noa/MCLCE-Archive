/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#include "world/level/block/EndPortalBlock.h"
#include "world/level/material/Material.h"
#include "util/Random.h"
#include "world/level/BlockSource.h"
#include "world/level/Level.h"
// #include "world/entity/ParticleType.h"
#include "world/entity/Entity.h"
#include "world/entity/player/Player.h"
#include "world/level/dimension/Dimension.h"

// #include "network/packet/ShowCreditsPacket.h"

EndPortalBlock::EndPortalBlock(const std::string& nameId, int id)
	: EntityBlock(nameId, id, Material::getMaterial(MaterialType::Portal)) {
	setSolid(false);
	setPushesOutItems(true);

	mProperties = BlockProperty::Portal | BlockProperty::Immovable;
	setLightEmission(1.0f);
	setMapColor(Color::BLACK);
	mRenderLayer = RENDERLAYER_ENDPORTAL;
	mBlockEntityType = BlockEntityType::EndPortal;
}

bool EndPortalBlock::addCollisionShapes(BlockSource& region, const BlockPos& pos, const AABB* intersectTestBox, std::vector<AABB>& inoutBoxes, Entity* entity) const {
	Block::addCollisionShapes(region, pos, intersectTestBox, inoutBoxes, entity);
	return false;
}

void EndPortalBlock::animateTick(BlockSource& region, const BlockPos& pos, Random& random) const {
	for (int i = 0; i < 10; ++i) {
		const float smokePosX = pos.x + random.nextFloat();
		const float smokePosY = pos.y + 0.8f;
		const float smokePosZ = pos.z + random.nextFloat();
		Vec3 smokeAccel(0, 0, 0);

// 		region.getLevel().addParticle(ParticleType::Smoke, Vec3(smokePosX, smokePosY, smokePosZ), smokeAccel);
	}
}

void EndPortalBlock::entityInside(BlockSource& region, const BlockPos& pos, Entity& entity) const {
// 	AABB aabb;
// 	getAABB(region, pos, aabb);
// 	aabb = aabb.grow(Vec3(0.01f, 0.01f, 0.01f));
// 	if (!region.getLevel().isClientSide() && !entity.isRiding() && !entity.isRide() && entity.canChangeDimensions() && entity.mBB.intersects(aabb)) {
// 		DimensionId targetDimension = entity.getRegion().getDimension().getId() == DimensionId::TheEnd ? DimensionId::Overworld : DimensionId::TheEnd;
// 
// 		if (entity.getEntityTypeId() == EntityType::Player && targetDimension == DimensionId::Overworld) {
// 			Player& player = (Player&)entity;
// 			player.changeDimensionWithCredits(targetDimension);
// 		}
// 		else {
// 			entity.changeDimension(targetDimension);
// 		}
// 	}
}

int EndPortalBlock::getResource(Random& random, int data, int bonusLootLevel) const {
	return 0;
}

int EndPortalBlock::getResourceCount(Random& random, int data, int bonusLootLevel) const {
	return 0;
}

bool EndPortalBlock::isWaterBlocking() const {
	return true;
}

const AABB& EndPortalBlock::getAABB(BlockSource& region, const BlockPos& pos, AABB& bufferValue, int optionalData, bool isClipping, int clipData) const {
	bufferValue.set((float)pos.x, (float)pos.y, (float)pos.z, (float)pos.x + 1.0f, (float)pos.y + (0.75f), (float)pos.z + 1.0f);
	return bufferValue;
}

bool EndPortalBlock::getCollisionShape(AABB& outAABB, BlockSource& region, const BlockPos& pos, Entity* entity) const {
	getAABB(region, pos, outAABB);
	return true;
}

void EndPortalBlock::onFallOn(BlockSource& region, const BlockPos& pos, Entity* entity, float fallDistance) const {
// 	AABB aabb;
// 	getAABB(region, pos, aabb);
// 	aabb = aabb.grow(Vec3(0.01f, 0.01f, 0.01f));
// 	if (!region.getLevel().isClientSide() && !entity->isRiding() && !entity->isRide() && entity->canChangeDimensions() && entity->mBB.intersects(aabb)) {
// 		DimensionId targetDimension = entity->getRegion().getDimension().getId() == DimensionId::TheEnd ? DimensionId::Overworld : DimensionId::TheEnd;
// 
// 		if (entity->getEntityTypeId() == EntityType::Player && targetDimension == DimensionId::Overworld) {
// 			auto player = static_cast<Player*>(entity);
// 			player->changeDimensionWithCredits(targetDimension);
// 		}
// 		else {
// 			entity->changeDimension(targetDimension);
// 		}
// 	}
}
