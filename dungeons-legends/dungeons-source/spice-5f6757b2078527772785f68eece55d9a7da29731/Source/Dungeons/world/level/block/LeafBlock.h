/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

#include "world/level/block/TransparentBlock.h"

class Color;

class LeafBlock : public TransparentBlock {
public:
	static const int REQUIRED_WOOD_RANGE = 4;

	//@attn @note:	PERSISTENT_LEAF_BIT and UPDATE_LEAF_BIT are reversed
	//				here, compared to desktop version
	//static const int PERSISTENT_LEAF_BIT = 8;	// player-placed
	//static const int UPDATE_LEAF_BIT = 4;
	static const int NORMAL_LEAF = 0;

	static void runDecay(BlockSource& region, const BlockPos& pos, int range);

	static bool isDeepLeafBlock(BlockSource& region, const BlockPos& pos);

	LeafBlock(const std::string& nameId, int id);
	~LeafBlock();

	void onRemove(BlockSource& region, const BlockPos& pos) const override;

	bool isSeasonTinted(BlockSource& region, const BlockPos& p) const override;
	virtual BlockRenderLayer getRenderLayer(BlockSource& region, const BlockPos& pos) const override;
	int getColor(BlockSource& region, const BlockPos& pos, DataID data) const override;
	virtual Color getMapColor(BlockSource& region, const BlockPos& pos) const override;
	Color getSeasonsColor(BlockSource& region, const BlockPos& pos, int startColumn, int endColumn) const;

	void tick(BlockSource& region, const BlockPos& pos, Random& random) const override;

	void playerDestroy(Player* player, const BlockPos& pos, int data) const override;

	int getResourceCount(Random& random, int data, int bonusLootLevel = 0) const override;
	int getResource(Random& random, int data, int bonusLootLevel = 0) const override;
	void spawnResources(BlockSource& region, const BlockPos& pos, int data, float odds = 1, int bonusLootLevel = 0) const override;

	virtual void dropExtraLoot(BlockSource& region, const BlockPos& pos, int data) const = 0;

	void onGraphicsModeChanged(bool fancy, bool preferPolyTessellation, bool transparentLeaves) override;

	const MobSpawnerData* getMobToSpawn(BlockSource& region, const BlockPos& pos) const override;

	int getLeafType(int data) const;

protected:
	DataID getSpawnResourcesAuxValue(DataID data) const override;

	void die(BlockSource& region, const BlockPos& pos) const;

	bool mHasTransparentLeaves;
};
