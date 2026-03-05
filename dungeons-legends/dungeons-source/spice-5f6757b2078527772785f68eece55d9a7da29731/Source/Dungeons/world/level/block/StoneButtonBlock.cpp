#include "Dungeons.h"

#include "StoneButtonBlock.h"
#include "world/Facing.h"
#include "world/item/ItemInstance.h"

StoneButtonBlock::StoneButtonBlock(const std::string& nameId, int id) 
	: ButtonBlock(nameId, id, false) {
}

ItemInstance StoneButtonBlock::asItemInstance(BlockSource& region, const BlockPos& pos, int blockData) const {
	return ItemInstance(Block::mStoneButton, 1, Facing::EAST);
}
