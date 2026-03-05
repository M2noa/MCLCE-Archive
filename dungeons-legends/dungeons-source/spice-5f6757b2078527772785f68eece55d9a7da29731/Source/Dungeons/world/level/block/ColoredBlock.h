/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

#include "world/level/block/Block.h"

class ItemInstance;

class ColoredBlock : public Block {
public:
	ColoredBlock(const std::string& nameId, int id, const Material& material);

	DataID getSpawnResourcesAuxValue(DataID data) const override;

	static int getBlockDataForItemAuxValue(int data);
	static int getItemAuxValueForBlockData(int data);

	virtual std::string buildDescriptionName(DataID data) const override;

	virtual Color getMapColor(BlockSource& region, const BlockPos& pos) const override;
};
