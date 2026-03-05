/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

#include "world/level/block/Block.h"

class ClothBlock : public Block {
public:
	ClothBlock(const std::string& nameId, int id);
	ClothBlock(const std::string& nameId, int id, int data);

	static int getBlockDataForItemAuxValue(int auxValue);
	virtual Color getMapColor(BlockSource& region, const BlockPos& pos) const override;
protected:
	DataID getSpawnResourcesAuxValue(DataID data) const override;

private:
	static const int COLOR_TEX_POS = 7 * 16 + 1;
};
