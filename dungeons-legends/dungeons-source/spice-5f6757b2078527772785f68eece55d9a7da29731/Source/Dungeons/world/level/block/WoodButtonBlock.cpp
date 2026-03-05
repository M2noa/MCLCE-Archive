#include "Dungeons.h"

#include "WoodButtonBlock.h"
#include "Block.h"
#include "world/Facing.h"
#include "world/item/ItemInstance.h"

WoodButtonBlock::WoodButtonBlock(const std::string& nameId, int id) 
    : ButtonBlock(nameId, id, true) {
}

ItemInstance WoodButtonBlock::asItemInstance(BlockSource& region, const BlockPos& pos, int blockData) const {
   return ItemInstance(Block::mWoodButton, 1, Facing::EAST);
}
