/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

#include "world/level/block/Block.h"

class PrismarineBlock : public Block {
public:
	enum class PrismarineType : DataID {
		Prismarine = 0,
		PrismarineDark = 1,
		PrismarineBricks = 2
	};
	static const int NUM_TYPES = 3;

	PrismarineBlock(const std::string& nameId, int id);
	int getResource(Random& random, int data, int bonusLootLevel = 0) const override;
	DataID getSpawnResourcesAuxValue(DataID data) const override;
	virtual std::string buildDescriptionName(DataID data) const override;
};