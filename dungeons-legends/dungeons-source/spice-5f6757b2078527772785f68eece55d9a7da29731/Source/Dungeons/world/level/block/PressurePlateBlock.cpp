/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/
#include "Dungeons.h"

#include "world/level/block/PressurePlateBlock.h"
#include "world/redstone/Redstone.h"
#include "world/entity/Entity.h"
#include "world/item/ItemInstance.h"
#include "world/level/BlockSource.h"

PressurePlateBlock::PressurePlateBlock(const std::string& nameId, int id, const Material &material, Sensitivity sensitivity) 
	: BasePressurePlateBlock(nameId, id, material) {
	this->sensitivity = sensitivity;
}

ItemInstance PressurePlateBlock::asItemInstance(BlockSource& region, const BlockPos& pos, int blockData) const {
	switch(sensitivity) {
	case Sensitivity::MOBS:
	case Sensitivity::PLAYERS:
		return ItemInstance(Block::mStonePressurePlate);
	case Sensitivity::EVERYTHING:
	default:
		return ItemInstance(Block::mWoodPressurePlate);
	}
}

int PressurePlateBlock::getDataForSignal(int signal) const {
	return signal > 0 ? 1 : 0;
}

int PressurePlateBlock::getSignalForData(int data) const {
	return data == 1 ? Redstone::SIGNAL_MAX : 0;
}

int PressurePlateBlock::getSignalStrength(BlockSource& region, const BlockPos &pos) const {
	EntityList entities;

	if (sensitivity == Sensitivity::EVERYTHING) {
		entities = region.getEntities(nullptr, getSensitiveAABB(pos));

	}
	if (sensitivity == Sensitivity::MOBS) {
		entities = region.getEntities(EntityType::Mob, getSensitiveAABB(pos));
	}
	if (sensitivity == Sensitivity::PLAYERS) {
		entities = region.getEntities(EntityType::Player, getSensitiveAABB(pos));
	}

	if (entities.size() > 0 && !entities.empty()) {
		for (auto &e : entities) {
			if (e->getEntityTypeId() != EntityType::Experience) {
				return Redstone::SIGNAL_MAX;
			}
		}
	}

	return Redstone::SIGNAL_NONE;
}
