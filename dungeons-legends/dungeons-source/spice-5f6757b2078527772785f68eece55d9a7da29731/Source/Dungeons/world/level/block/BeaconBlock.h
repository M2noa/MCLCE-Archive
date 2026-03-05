/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/

#pragma once

#include "world/level/block/EntityBlock.h"

class BeaconBlock : public EntityBlock {
public:
	BeaconBlock(const std::string& nameId, int id);
	virtual ~BeaconBlock();

	virtual bool use(Player& player, const BlockPos& pos) const override;

private:
};