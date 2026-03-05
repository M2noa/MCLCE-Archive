#pragma once

#include "ButtonBlock.h"

class Icon;

class WoodButtonBlock : public ButtonBlock {
public:
	WoodButtonBlock(const std::string& nameId, int id);

	virtual ItemInstance asItemInstance(BlockSource& region, const BlockPos& pos, int blockData) const override;
};
