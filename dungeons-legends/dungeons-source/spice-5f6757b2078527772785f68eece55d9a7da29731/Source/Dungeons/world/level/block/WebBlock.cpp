/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#include "world/level/block/WebBlock.h"
#include "world/level/material/Material.h"
#include "world/entity/player/Player.h"
#include "world/entity/Entity.h"
#include "world/item/Item.h"
#include "world/phys/AABB.h"

WebBlock::WebBlock(const std::string& nameId, int id)
	: Block(nameId, id, Material::getMaterial(MaterialType::Web)) {
	setSolid(false);
	mRenderLayer = RENDERLAYER_ALPHATEST;
	mProperties = BlockProperty::BreakOnPush;
	mTranslucency[id] = std::max(0.8f, mMaterial.getTranslucency());
}

void WebBlock::entityInside(BlockSource& region, const BlockPos& pos, Entity& entity) const {
// 	if (entity.getEntityTypeId() == EntityType::Player) {
// 		Player& player = static_cast<Player&>(entity);
// 
// 		if (player.mAbilities.mInstabuild && player.mAbilities.mFlying) {
// 			return;
// 		}
// 	}
// 
// 	entity.makeStuckInWeb();
}

const AABB& WebBlock::getAABB(BlockSource& region, const BlockPos& pos, AABB& bufferValue, int optionalData, bool isClipping, int clipData) const {
	if (isClipping) {
		return bufferValue.set(Vec3::ZERO, Vec3::ONE).move(Vec3(pos));
	}

	return AABB::EMPTY;
}

int WebBlock::getResource(Random& random, int data, int bonusLootLevel) const {
	return Item::mString->getId();
}
