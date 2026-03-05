/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

#include "world/level/block/LogBlock.h"

class OldLogBlock : public LogBlock {
public:
	enum class LogType : DataID {
		Oak,
		Spruce,
		Birch,
		Jungle
	};

	OldLogBlock(const std::string& nameId, int id);

	int getResource(Random& random, int data, int bonusLootLevel = 0) const override;

	virtual std::string buildDescriptionName(DataID data) const override;
};
