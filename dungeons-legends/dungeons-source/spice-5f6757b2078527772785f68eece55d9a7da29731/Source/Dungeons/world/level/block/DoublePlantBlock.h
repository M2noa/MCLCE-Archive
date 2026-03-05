/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

#include "world/level/block/BushBlock.h"

enum class DoublePlantType : DataID {
	Sunflower = 0,
	Syringa = 1,
	Grass = 2,
	Fern = 3,
	Rose = 4,
	Paeonia = 5,
	Count = 6
};

class DoublePlantBlock : public BushBlock {
public:
	DoublePlantBlock(const std::string& nameId, int id);

	int getType(BlockSource& region, const BlockPos& pos) const;
	int getOrientation(BlockSource& region, const BlockPos& pos);
	bool mayPlace(BlockSource& region, const BlockPos& pos) const override;
	bool mayPlace(BlockSource& region, const BlockPos& pos, FacingID face) const override;
	void checkAlive(BlockSource& region, const BlockPos& pos) const override;
	bool canSurvive(BlockSource& region, const BlockPos& pos) const override;

	void neighborChanged(BlockSource& region, const BlockPos& pos, const BlockPos& neighborPos) const override;
	int getResource(Random& random, int data, int bonusLootLevel = 0) const override;
	int getResourceCount(Random& random, int data, int bonusLootLevel = 0) const override;
	ItemInstance asItemInstance(BlockSource& region, const BlockPos& pos, int blockData) const override;

	DataID getSpawnResourcesAuxValue(DataID data) const override;
	bool isTop(BlockSource& region, const BlockPos& pos) const;
	bool isTop(DataID data) const;
	Vec3 randomlyModifyPosition(const BlockPos& pos, int32_t& seed) const override;
	Vec3 randomlyModifyPosition(const BlockPos& pos) const override;

	const AABB& getAABB(BlockSource& region, const BlockPos& pos, AABB& bufferValue, int optionalData = 0, bool isClipping = false, int clipData = 0) const override;
	const AABB& getVisualShape(BlockSource& region, const BlockPos& pos, AABB& bufferAABB, bool isClipping = false) const override;
	const AABB& getVisualShape(DataID data, AABB& bufferAABB, bool isClipping = false) const override;

	int getColor(BlockSource& region, const BlockPos& pos, DataID data) const override;
	int getColorForParticle(BlockSource& region, const BlockPos& pos, int auxData) const override;
	void placeAt(BlockSource& region, const BlockPos& pos, DataID plantType, int updateType, Entity* placer) const;
	void playerDestroy(Player* player, const BlockPos& pos, int data) const override;
	bool playerWillDestroy(Player& player, const BlockPos& pos, int data) const override;
	bool popGrassResources(BlockSource& region, const BlockPos& pos, int data, Player* player) const;
	bool popFlowerResources(BlockSource& region, const BlockPos& pos, int data, Entity* entity) const;

	bool getSecondPart(BlockSource& region, const BlockPos& pos, BlockPos& out) const override;

	void onPlace(BlockSource& region, const BlockPos& pos) const override;
	void onLoaded(BlockSource& region, const BlockPos& pos) const override;
	virtual bool onFertilized(BlockSource& region, const BlockPos& pos, Entity* entity) const override;

	std::string buildDescriptionName(DataID data) const override;
	void onGraphicsModeChanged(bool fancy_, bool preferPolyTessellation, bool transparentLeaves) override;

	bool canBeBuiltOver(BlockSource& region, const BlockPos& pos) const override;

protected:
	bool canBeSilkTouched() const override;

private:
	AABB mBottomVisualShape;
	bool _isCoveredByTopSnow(BlockSource& region, const BlockPos& pos) const;
};
