#pragma once

#include "BaseRailBlock.h"

class Entity;

class ActivatorRailBlock : public BaseRailBlock {

public:
	ActivatorRailBlock(const std::string& nameId, int id);

	virtual int getVariant(int data) const override;

	virtual void onRedstoneUpdate(BlockSource& source, const BlockPos& pos, int strength, bool isFirstTime) const override;
};
