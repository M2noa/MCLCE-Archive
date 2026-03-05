/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#include "world/level/block/TntBlock.h"
// #include "world/entity/item/PrimedTnt.h"
#include "world/level/Level.h"
#include "world/level/dimension/Dimension.h"
#include "world/level/material/Material.h"
#include "world/level/BlockSource.h"
#include "world/entity/player/Player.h"
#include "world/Facing.h"
#include "world/item/Item.h"
// #include "world/entity/ParticleType.h"
#include "world/level/block/LevelEvent.h"
// #include "world/redstone/circuit/CircuitSystem.h"
// #include "world/redstone/circuit/components/ConsumerComponent.h"
#include "world/Container.h"
// #include "world/entity/EntityFactory.h"

TntBlock::TntBlock(const std::string& nameId, int id)
	: Block(nameId, id, Material::getMaterial(MaterialType::Explosive)) {

}

int TntBlock::getResourceCount(Random& random, int data, int bonusLootLevel) const{
	return 1;
}

void TntBlock::onLoaded(BlockSource& region, const BlockPos& pos) const {
// 	if (!region.getLevel().isClientSide()) {
// 		auto &circuit = region.getDimension().getCircuitSystem();
// 		circuit.create<ConsumerComponent>(pos, &region, Facing::DOWN);
// 	}
}

void TntBlock::onExploded(BlockSource& region, const BlockPos& pos, Entity* entitySource) const{
// 	auto& level = region.getLevel();
// 	Vec3 vecPos(pos.x + 0.5f, (float)pos.y, pos.z + 0.5f);
// 	auto result = EntityFactory::createSpawnedEntity(EntityDefinitionIdentifier("minecraft", EntityTypeToString(EntityType::PrimedTnt), "from_explosion"), entitySource, vecPos, Vec2::ZERO);
// 	if (result) {
// 		level.addEntity(region, std::move(result));
// 	}
}

void TntBlock::destroy(BlockSource& region, const BlockPos& pos, int data, Entity* entitySource) const{
	auto& level = region.getLevel();
	if (level.isClientSide()) {
		return;
	}

// 	if (shouldExplode(data)) {
// 		auto result = EntityFactory::createSpawnedEntity(EntityType::PrimedTnt, entitySource, Vec3(pos) + Vec3(0.5f, 0.5f, 0.5f), Vec2::ZERO);
// 		if (result) {
// 			level.broadcastDimensionEvent(region, LevelEvent::SoundFuse, result->getPos(), 0);
// 			level.addEntity(region, std::move(result));
// 		}
// 	}
}

bool TntBlock::use(Player& player, const BlockPos& pos) const {
	ItemInstance* carried = player.getSelectedItem();
// 	if (carried && (carried->isInstance(Item::mFlintAndSteel) || carried->isInstance(Item::mFireCharge) || EnchantUtils::hasEnchant(Enchant::WEAPON_FIRE, *carried))) {
	if (carried && (carried->isInstance(Item::mFlintAndSteel) || carried->isInstance(Item::mFireCharge))) {
		auto& region = player.getRegion();

		DataID data = 0;
		getBlockState(BlockState::ExplodeBit).set(data, true);

		carried->hurtAndBreak(1, &player);
		region.setBlockAndData(pos, BlockID::AIR, Block::UPDATE_ALL, &player);
		destroy(region, pos, data, &player);

		return true;
	}

	return Block::use(player, pos);
}

bool TntBlock::canProvideSupport(BlockSource& region, const BlockPos& pos, FacingID face, BlockSupportType type) const {
	return true;
}

bool TntBlock::dispense(BlockSource& region, Container& container, int slot, const Vec3& pos, FacingID face) const {
// 	Level& level = region.getLevel();
// 	BlockPos blockPos(pos);

// 	auto result = EntityFactory::createSpawnedEntity(EntityType::PrimedTnt, nullptr, Vec3(blockPos) + Vec3(0.5f, 0.5f, 0.5f), Vec2::ZERO);
// 	if (result) {
// 		level.addEntity(region, std::move(result));
// 		region.getLevel().broadcastLevelEvent(LevelEvent::SoundClick, pos, 1000);
// 		level.broadcastDimensionEvent(region, LevelEvent::SoundFuse, Vec3(blockPos), 0);
// 
// 		container.removeItem(slot, 1);
// 
// 		return true;
// 	}
	return false;
}

void TntBlock::onRedstoneUpdate(BlockSource& region, const BlockPos& pos, int strength, bool isFirstTime) const {
	if (strength > 0) {
		DataID data = 0;
		getBlockState(BlockState::ExplodeBit).set(data, true);

		destroy(region, pos, data, nullptr);
		region.setBlock(pos.x, pos.y, pos.z, BlockID::AIR, Block::UPDATE_ALL);
	}
}

void TntBlock::onPlace(BlockSource& region, const BlockPos& pos) const {
	Block::onPlace(region, pos);
	onLoaded(region, pos);
}

bool TntBlock::shouldExplode(const DataID& data) const {
	return getBlockState(BlockState::ExplodeBit).getBool(data);
}
