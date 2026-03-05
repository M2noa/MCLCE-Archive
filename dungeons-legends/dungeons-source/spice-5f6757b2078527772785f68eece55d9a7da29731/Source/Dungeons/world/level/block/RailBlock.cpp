/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#include "world/level/block/RailBlock.h"

RailBlock::RailBlock(const std::string& nameId, int id)
	: BaseRailBlock(nameId, id, false) {
}

int RailBlock::getVariant(int data) const {
	int railDir = getBlockState(BlockState::RailDirection).get<int>(data);
	bool dataBit = getBlockState(BlockState::RailDataBit).getBool(data);
	return (dataBit || railDir >= 6) ? 1 : 0;
}
