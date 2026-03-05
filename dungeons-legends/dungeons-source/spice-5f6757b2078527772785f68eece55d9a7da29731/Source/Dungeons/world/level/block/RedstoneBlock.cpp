#include "Dungeons.h"

#include "RedstoneBlock.h"
#include "world/level/material/Material.h"
#include "world/level/BlockSource.h"
#include "world/level/dimension/Dimension.h"
#include "world/redstone/Redstone.h"
// #include "world/redstone/circuit/CircuitSystem.h"
// #include "world/redstone/circuit/components/ProducerComponent.h"
#include "world/level/Level.h"

RedstoneBlock::RedstoneBlock(const std::string& nameId, int id)
	: Block(nameId, id, Material::getMaterial(MaterialType::Metal)) {
}

bool RedstoneBlock::shouldConnectToRedstone(BlockSource& region, const BlockPos& pos, int direction) const
{
	return true;
}

void RedstoneBlock::onLoaded(BlockSource& region, const BlockPos& pos) const {
// 	if (!region.getLevel().isClientSide()) {
// 		auto producer = region.getDimension().getCircuitSystem().create<ProducerComponent>(pos, &region, Facing::NOT_DEFINED);
// 		if (producer) {
// 			producer->setStrength(Redstone::SIGNAL_MAX);
// 		}
// 	}
}

void RedstoneBlock::onPlace(BlockSource& region, const BlockPos& pos) const {
	onLoaded(region, pos);
}
