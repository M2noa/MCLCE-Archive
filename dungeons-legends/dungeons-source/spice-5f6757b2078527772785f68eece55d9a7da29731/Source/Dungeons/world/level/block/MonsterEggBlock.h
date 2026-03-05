/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

#include "world/level/block/Block.h"

class Silverfish;

class MonsterEggBlock : public Block {
public:
	enum class HostBlockType {
		Stone = 0,
		Cobblestone = 1,
		StoneBrick = 2,
		MossyStoneBrick = 3,
		CrackedStoneBrick = 4,
		ChiseledStoneBrick = 5,
	};

	static bool isCompatibleHostBlock(int blockId, int blockData);
	static int getDataForHostBlock(int block, int data);

	MonsterEggBlock(const std::string& nameId, int id);

	int getVariant(int data) const override;
	int getResourceCount(Random& random, int data, int bonusLootLevel = 0) const override;
	int getResource(Random& random, int data, int bonusLootLevel = 0) const override;
	void spawnResources(BlockSource& region, const BlockPos& pos, int data, float odds = 1, int bonusLootLevel = 0) const override;

	virtual std::string buildDescriptionName(DataID data) const override;

protected:
	ItemInstance getSilkTouchItemInstance(DataID data) const override;

private:
	int _getBlockIdFromData(int data) const;
};
