/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

#include "world/level/block/BushBlock.h"

enum class TallGrassType : DataID {
	Tall = 1,
	Fern,
	Snow
};

class TallGrass : public BushBlock {
public:
	TallGrass(const std::string& nameId, int id);

	int getColor(int auxData) const override;
	int getColor(BlockSource& region, const BlockPos& pos, DataID data) const override;
	virtual Color getMapColor(BlockSource& region, const BlockPos& pos) const override;

	int getResource(Random& random, int data, int bonusLootLevel = 0) const override;
	void playerDestroy(Player* player, const BlockPos& pos, int data) const override;

	virtual std::string buildDescriptionName(DataID data) const override;

	virtual Vec3 randomlyModifyPosition(const BlockPos& pos) const override;

	const AABB& getAABB(BlockSource& region, const BlockPos& pos, AABB& bufferValue, int optionalData = 0, bool isClipping = false, int clipData = 0) const override;

	bool canSurvive(BlockSource& region, const BlockPos& pos) const override;

	bool onFertilized(BlockSource& region, const BlockPos& pos, Entity* entity) const override;
	bool mayConsumeFertilizer(BlockSource& region) const override;

	void onGraphicsModeChanged(bool fancy_, bool preferPolyTessellation, bool transparentLeaves) override;

protected:
	bool canBeSilkTouched() const override;

};
