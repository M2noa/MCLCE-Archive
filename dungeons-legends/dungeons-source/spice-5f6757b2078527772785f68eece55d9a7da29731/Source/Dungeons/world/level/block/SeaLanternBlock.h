/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

#include "world/level/block/Block.h"

class SeaLanternBlock : public Block {
public:
	SeaLanternBlock(const std::string& nameId, int id);

	int getResource(Random& random, int data, int bonusLootLevel = 0) const override;
	DataID getSpawnResourcesAuxValue(DataID data) const override;
	virtual std::string buildDescriptionName(DataID data) const override;
	virtual bool canBeSilkTouched() const override;
	int getResourceCount(Random& random, int data, int bonusLootLevel = 0) const override;
};
