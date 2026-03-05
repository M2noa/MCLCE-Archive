/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#include "world/level/block/EnchantingTableBlock.h"
//#include "world/level/block/entity/EnchantingTableEntity.h"
#include "world/level/material/Material.h"
#include "world/level/BlockSource.h"
#include "world/level/Level.h"
//#include "world/entity/player/Player.h"
#include "world/Facing.h"
//#include "world/entity/ParticleType.h"

EnchantingTableBlock::EnchantingTableBlock(const std::string& nameId, int id)
	: EntityBlock(nameId, id, Material::getMaterial(MaterialType::Stone)) {
	setVisualShape( Vec3::ZERO, Vec3(1, 12 / 16.0f, 1));
	setLightBlock(Brightness::MAX);
	setLightEmission(0.8f);
	setDestroyTime(5);

	mBlockEntityType = BlockEntityType::EnchantingTable;
	
	mProperties = BlockProperty::Immovable;

	setSolid(false);
	setPushesOutItems(true);
}

bool EnchantingTableBlock::use(Player& player, const BlockPos& pos) const {
	// TODO: rherlitz

	//TODO remove when creative inventories are added
	//if (!player.getLevel().isClientSide()) {
	//	BlockEntity* entity = player.getRegion().getBlockEntity(pos);
	//	//	First make sure the entity at this position is actually an enchanting table block entity before casting it.
	//	if (entity != nullptr && entity->isType(BlockEntityType::EnchantingTable)) {
	//		EnchantingTableEntity * table = static_cast<EnchantingTableEntity*>(entity);
	//		player.openEnchanter(table->getContainerSize(), table->getPosition());
	//	}
	//}

	return true;
}

void EnchantingTableBlock::animateTick(BlockSource& region, const BlockPos& pos, Random& random) const {
	EntityBlock::animateTick(region, pos, random);

	// TODO: rherlitz

	//const Vec3 particleEnd = Vec3(pos) + Vec3(0.5f, 2.0f, 0.5f);

	//Level& level = region.getLevel();

	//DEBUG_ASSERT_MAIN_THREAD;
	//auto positions = EnchantUtils::getBookCasePositions(region, pos);

	//for (auto& bookcase : positions) {
	//	if (random.nextInt(4) == 0) {
	//		continue;
	//	}

	//	float randx = random.nextFloat(-0.5f, 0.5f);
	//	float randy = random.nextFloat(-2.0f, -1.0f);
	//	float randz = random.nextFloat(-0.5f, 0.5f);
	//	auto velocity = Vec3(bookcase - pos) + Vec3(randx, randy, randz);

	//	level.addParticle(ParticleType::EnchantingTable, particleEnd, velocity);
	//}
}

bool EnchantingTableBlock::isCraftingBlock() const {
	return true;
}

bool EnchantingTableBlock::canBeSilkTouched() const {
	return false;
}
