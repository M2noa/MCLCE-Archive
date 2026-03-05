/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

#include "world/level/block/BaseRailBlock.h"

class RailBlock : public BaseRailBlock {
public:
	RailBlock(const std::string& nameId, int id);
	virtual int getVariant(int data) const override;
};
