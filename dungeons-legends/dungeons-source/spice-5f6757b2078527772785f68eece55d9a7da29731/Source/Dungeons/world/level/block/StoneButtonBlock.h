#pragma once

#include "ButtonBlock.h"

class Icon;

class StoneButtonBlock : public ButtonBlock {
public:
    StoneButtonBlock(const std::string& nameId, int id);
	virtual ItemInstance asItemInstance(BlockSource& region, const BlockPos& pos, int blockData) const override;
};
