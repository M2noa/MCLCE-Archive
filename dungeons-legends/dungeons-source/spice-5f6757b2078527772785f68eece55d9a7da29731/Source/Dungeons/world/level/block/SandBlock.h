/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

#include "CommonTypes.h"
#include "world/level/block/HeavyBlock.h"

class SandBlock : public HeavyBlock {
public:
	enum class SandType : DataID {
		Normal = 0,
		Red = 1,
	};
	
	int getVariant(int data) const override;
	SandBlock(const std::string& nameId, int id);

	DataID getSpawnResourcesAuxValue(DataID data) const override;

	std::string buildDescriptionName(DataID data) const override;

	Color getDustColor(DataID data) const override;

	virtual Color getMapColor(BlockSource& region, const BlockPos& pos) const override;
};
