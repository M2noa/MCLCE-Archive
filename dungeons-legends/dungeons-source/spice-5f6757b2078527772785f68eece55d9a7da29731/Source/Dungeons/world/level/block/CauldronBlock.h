/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

#include "world/level/block/EntityBlock.h"
#include "world/level/material/Material.h"
#include "world/level/block/LevelEvent.h"

class Level;

class CauldronBlock : public EntityBlock {
public:
	static int MIN_FILL_LEVEL;
	static int MAX_FILL_LEVEL;

	CauldronBlock(const std::string& nameId, int id);

	bool addCollisionShapes(BlockSource& region, const BlockPos& pos, const AABB* intersectTestBox, std::vector<AABB>& inoutBoxes, Entity* entity) const override;
	void addAABBs(BlockSource& region, const BlockPos& pos, const AABB* intersectTestBox, std::vector<AABB>& inoutBoxes) const override;
	void updateShape(BlockSource& region, const BlockPos& pos);

	virtual bool canProvideSupport(BlockSource& region, const BlockPos& pos, FacingID face, BlockSupportType type) const override;

	void handleEntityInside(BlockSource& region, const BlockPos& pos, Entity* entity, Vec3& current) const override;

	bool use(Player& player, const BlockPos& pos) const override;
	void setWaterLevel(BlockSource& region, const BlockPos& pos, DataID data, int waterLevel, Entity* setter) const;
	static int getWaterLevel(DataID data);

	void handleRain(BlockSource& region, const BlockPos& pos, float downfallAmount) const override;

	int getResource(Random& random, int data, int bonusLootLevel = 0) const override;
	ItemInstance asItemInstance(BlockSource& region, const BlockPos& pos, int blockData) const override;
	ItemInstance getSilkTouchItemInstance(DataID data) const override;

	bool hasComparatorSignal() const override;
	int getComparatorSignal(BlockSource& region, const BlockPos& pos, FacingID dir, int data) const override;

	void animateTick(BlockSource& region, const BlockPos& pos, Random& random) const override;
	bool isInteractiveBlock() const override { return true; }

	int getExtraRenderLayers() const override;

	static void spawnPotionParticles(Level& level, const Vec3& pos, Random& random, int potionColor, int count);
	static void spawnSplashParticles(Level& level, const Vec3& pos, Random& random, int waterColor, int count);
	static void spawnBubbleParticles(Level& level, const Vec3& pos, Random& random, int waterColor, int count);

private:
	void _useInventory(Player& player, ItemInstance* current, ItemInstance& replaceWith, int useCount = 1) const;
	void _setSelectedItemOrAdd(Player& player, ItemInstance& newSelected) const;
	void _decrementStack(Player& player, ItemInstance* current) const;
	void _explodeCauldronContents(BlockSource& region, const BlockPos& pos, DataID data) const;
	void _spawnCauldronEvent(BlockSource& region, const BlockPos& pos, LevelEvent levelEvent) const;

	static int BASE_WATER_PIXEL;
	static int PIXEL_PER_LEVEL;
};

