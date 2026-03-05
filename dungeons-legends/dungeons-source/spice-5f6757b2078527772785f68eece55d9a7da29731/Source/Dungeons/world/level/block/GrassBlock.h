/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

#include "world/level/block/Block.h"
#include "Core/Math/Color.h"

class GrassBlock : public Block {
public:
	static const Brightness MIN_BRIGHTNESS;

	GrassBlock(const std::string& nameId, int id);

	int getColor(int auxdata) const override;
	int getColor(BlockSource& region, const BlockPos& pos, DataID data) const override;

	void tick(BlockSource& region, const BlockPos& pos, Random& random) const override;

	bool onFertilized(BlockSource& region, const BlockPos& pos, Entity* entity) const override;

	int getResource(Random& random, int data, int bonusLootLevel = 0) const override;

	const MobSpawnerData* getMobToSpawn(BlockSource& region, const BlockPos& pos) const override;
	DataID calcVariant(BlockSource& region, const BlockPos& pos, DataID data) const override;

	virtual Color getMapColor(BlockSource& region, const BlockPos& pos) const override;

	void initSideColorsFromAtlas(const TextureAtlas& atlas, const TextureAtlasItem& atlasItem);

private:
	bool _randomWalk(BlockSource& region, BlockPos& pos, int j) const;
	int _getGrassSide(int color) const;

	std::vector<Color> sideColors;
};
