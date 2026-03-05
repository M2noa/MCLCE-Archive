/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

#include "world/level/block/LeafBlock.h"
#include "world/item/ItemInstance.h"

class OldLeafBlock : public LeafBlock 
{
public:
	enum class LeafType : DataID {
		Oak,
		Spruce,
		Birch,
		Jungle
	};
	static const int NUM_TYPES = 4;

	OldLeafBlock(const std::string& nameId, int id);

	int getColor(BlockSource& region, const BlockPos& pos, DataID data) const override;
	Color getMapColor(BlockSource& region, const BlockPos& pos) const override;

	void dropExtraLoot(BlockSource& region, const BlockPos& pos, int data) const override;

	int getVariant(int data) const override;
	
	std::string buildDescriptionName(DataID data) const override;
protected:
	DataID getSpawnResourcesAuxValue(DataID data) const override;

};
