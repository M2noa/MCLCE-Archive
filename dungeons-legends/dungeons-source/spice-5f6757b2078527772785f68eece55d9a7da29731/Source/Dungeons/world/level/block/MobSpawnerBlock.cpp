/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#include "world/level/block/MobSpawnerBlock.h"
#include "world/level/material/Material.h"
#include "util/Random.h"

MobSpawnerBlock::MobSpawnerBlock(const std::string& nameId, int id)
	: EntityBlock(nameId, id, Material::getMaterial(MaterialType::Stone)) {
	mBlockEntityType = BlockEntityType::MobSpawner;

	setVisualShape(Vec3(SIZE_OFFSET), Vec3::ONE - Vec3(SIZE_OFFSET));

	mRenderLayer = RENDERLAYER_ALPHATEST;
	mLightEmission[id] = Brightness(3);
	mProperties = BlockProperty::CubeShaped | BlockProperty::Immovable;
}

int MobSpawnerBlock::getResourceCount(Random& random, int data, int bonusLootLevel) const {
	return 0;
}

int MobSpawnerBlock::getExperienceDrop(Random& random) const {
	return random.nextInt(15, 43);
}

bool MobSpawnerBlock::canBeSilkTouched() const {
	return false;
}
