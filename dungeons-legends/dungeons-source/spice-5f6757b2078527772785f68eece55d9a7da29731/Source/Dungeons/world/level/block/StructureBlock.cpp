/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/

#include "Dungeons.h"

#include "world/level/block/StructureBlock.h"
#include "world/level/material/Material.h"
#include "world/level/block/entity/BlockEntity.h"
// #include "world/level/block/entity/StructureBlockEntity.h"
#include "world/level/BlockSource.h"
#include "world/level/Level.h"
#include "world/level/LevelConstants.h"
#include "world/entity/player/Player.h"

// #include "world/redstone/circuit/CircuitSystem.h"
// #include "world/redstone/circuit/components/ConsumerComponent.h"

StructureBlock::StructureBlock(const std::string& nameId, int id) :
	EntityBlock(nameId, id, Material::getMaterial(MaterialType::Metal)) {

	mBlockEntityType = BlockEntityType::StructureBlock;

	mProperties = BlockProperty::Immovable;
}

bool StructureBlock::use(Player& player, const BlockPos& pos) const {
	if (player.getLevel().isClientSide()) {
		return true;
	}

// 	BlockEntity* blockEntity = player.getRegion().getBlockEntity(pos);
// 	if (blockEntity && blockEntity->isType(BlockEntityType::StructureBlock)) {
// 		player.openStructureEditor(blockEntity->getPosition());
// 	}

	return true;
}

int StructureBlock::getResourceCount(Random& random, int data, int bonusLootLevel) const {
	return 0;
}

void StructureBlock::onRedstoneUpdate(BlockSource& region, const BlockPos& pos, int strength, bool isFirstTime) const {
// 	if (!isFirstTime) {
// 		// Call up the structure block and tell it that is time to become empowered (or not)
// 		BlockEntity *blockEntity = region.getBlockEntity(pos);
// 		if (blockEntity && blockEntity->isType(BlockEntityType::StructureBlock)) {
// 			StructureBlockEntity &structureBlock = *static_cast<StructureBlockEntity*>(blockEntity);
// 			structureBlock.setPowered(region, pos, strength > 0);
// 		}
// 	}
}

void StructureBlock::onLoaded(BlockSource& region, const BlockPos& pos) const {
	// Create the circuit for the Redstone interaction
// 	if (!region.getLevel().isClientSide()) {
// 		auto &circuit = region.getDimension().getCircuitSystem();
// 		auto consumer = circuit.create<ConsumerComponent>(pos, &region, Facing::DOWN);
// 		if (consumer) {
// 			consumer->setAllowPowerUp(true);
// 		}
// 	}
}

ItemInstance StructureBlock::asItemInstance(BlockSource& region, const BlockPos& pos, int blockData) const {
// 	return ItemInstance(this, 1, StructureBlockEntity::DATA);
	return ItemInstance(this, 1, 0);
}
