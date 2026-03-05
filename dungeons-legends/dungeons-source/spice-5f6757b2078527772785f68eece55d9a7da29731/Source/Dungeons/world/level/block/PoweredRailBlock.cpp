/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#include "world/level/block/PoweredRailBlock.h"

#include "world/level/BlockSource.h"
#include "world/level/Level.h"
#include "world/level/dimension/Dimension.h"
// #include "world/redstone/circuit/CircuitSystem.h"
// #include "world/redstone/circuit/components/BaseRailTransporter.h"

PoweredRailBlock::PoweredRailBlock(const std::string& nameId, int id)
	: BaseRailBlock(nameId, id, true) {
}

int PoweredRailBlock::getVariant(int data) const{
	return Block::mGoldenRail->getBlockState(BlockState::RailDataBit).get<int>(data);
}
 
void PoweredRailBlock::onRedstoneUpdate(BlockSource& region, const BlockPos& pos, int strength, bool isFirstTime) const {
	if (region.getLevel().isClientSide() == false) {

		FullBlock block = region.getBlockAndData(pos);

		// Set or clear the RAIL_DATA_BIT depending on if we have redstone strength
		if (strength > 0) {
			Block::mGoldenRail->getBlockState(BlockState::RailDataBit).set(block.data, 1);
		}
		else {
			Block::mGoldenRail->getBlockState(BlockState::RailDataBit).set(block.data, 0);
		}

// 		auto& circuit = region.getDimension().getCircuitSystem();
// 		circuit.lockGraph(true);
// 		region.setBlockAndData(pos, block, Block::UPDATE_ALL);
// 		circuit.lockGraph(false);
	}
}
