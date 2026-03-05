#include "Dungeons.h"

#include "ActivatorRailBlock.h"
#include "world/level/Level.h"
#include "world/level/BlockSource.h"
// #include "world/level/BlockTickingQueue.h"
#include "world/level/dimension/Dimension.h"
#include "world/redstone/Redstone.h"
// #include "world/redstone/circuit/CircuitSystem.h"
// #include "world/redstone/circuit/components/BaseRailTransporter.h"

ActivatorRailBlock::ActivatorRailBlock(const std::string& nameId, int id) 
	: BaseRailBlock(nameId, id, true) {
	setTicking(false);
}

int ActivatorRailBlock::getVariant(int data) const{
	return Block::mActivatorRail->getBlockState(BlockState::RailDataBit).get<int>(data);
}

void ActivatorRailBlock::onRedstoneUpdate(BlockSource& source, const BlockPos& pos, int strength, bool isFirstTime) const {
	if (source.getLevel().isClientSide() == false) {
		FullBlock b = source.getBlockAndData(pos);
		
		auto &railDataState = Block::mActivatorRail->getBlockState(BlockState::RailDataBit);
		
		int railBit = railDataState.get<int>(b.data);
		railDataState.set(b.data, !railBit);

		if (strength > 0) {
			railDataState.set(b.data, 1);
		}

		source.setBlockAndData(pos, b, UPDATE_CLIENTS);
	}
}
