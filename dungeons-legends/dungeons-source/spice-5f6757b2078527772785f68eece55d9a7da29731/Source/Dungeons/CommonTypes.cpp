#include "Dungeons.h"

#include "CommonTypes.h"

#include "world/level/block/Block.h"

bool BlockID::hasProperty(BlockProperty properties) const {
	return (Block::mBlocks[value]->getProperties() & properties) != BlockProperty::Unspecified;
}

const Block& FullBlock::getBlock() const {
	return *Block::mBlocks[id];
}
