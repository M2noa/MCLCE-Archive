/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

#include "world/level/block/BushBlock.h"

class Sapling : public BushBlock {
public:
	static const int NEW_LEAF_OFFSET = 4;
	static const int TYPE_DEFAULT;
	static const int TYPE_EVERGREEN;
	static const int TYPE_BIRCH;
	static const int TYPE_JUNGLE;
	static const int TYPE_ACACIA;
	static const int TYPE_ROOFED_OAK;

	Sapling(const std::string& nameId, int id);

	void tick(BlockSource& region, const BlockPos& pos, Random& random) const override;
	int getVariant(int data) const override;

	void growTree(BlockSource& region, const BlockPos& pos, Random& random, Entity* placer) const;

	bool isSapling(BlockSource& region, const BlockPos& pos, int type) const;

	bool onFertilized(BlockSource& region, const BlockPos& pos, Entity* entity) const override;

	virtual std::string buildDescriptionName(DataID data) const override;

protected:
	void advanceTree(BlockSource& region, const BlockPos& pos, Random& random, Entity* placer) const;

	DataID getSpawnResourcesAuxValue(DataID data) const override;

	std::unique_ptr<Feature> generateJungleTree(int& ox, int& oz, bool& multiblock, const BlockPos& pos, BlockSource& region, Entity* placer) const;
	std::unique_ptr<Feature> generateRoofTree(int& ox, int& oz, bool& multiblock, const BlockPos& pos, BlockSource& region, Entity* placer) const;
	std::unique_ptr<Feature> generateRedwoodTree(int& ox, int& oz, bool& multiblock, const BlockPos& pos, BlockSource& region, Entity* placer) const;

private:
	static const int TYPE_MASK = 7;
	static const int AGE_BIT = 8;
};
