/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

#include "world/level/block/LogBlock.h"

class ItemInstance;

class NewLogBlock : public LogBlock {
public:
	static const int ACACIA_TRUNK = 0;
	static const int BIG_OAK_TRUNK = 1;

	NewLogBlock(const std::string& nameId, int id);

	int getResource(Random& random, int data, int bonusLootLevel = 0) const override;

	virtual std::string buildDescriptionName(DataID data) const override;
};
