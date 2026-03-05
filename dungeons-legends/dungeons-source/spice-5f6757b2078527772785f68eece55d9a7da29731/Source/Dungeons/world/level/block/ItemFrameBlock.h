/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

#include "world/level/block/EntityBlock.h"

class ItemFrameBlockEntity;
class MapItemSavedData;

class ItemFrameBlock : public EntityBlock {
public:
	static const int NUM_ROTATIONS = 8;

	// note - these values differ from Direction and that worries me so I'm leaving them here for now
	// changing these to direction values could break old worlds without conversion
	enum ItemFrameDirection : unsigned char {
		DIR_EAST = 0,
		DIR_WEST,
		DIR_SOUTH,
		DIR_NORTH
	};

	ItemFrameBlock(const std::string& nameId, int id);

	static int getDirection(int data);
	int getPlacementDataValue(Entity& by, const BlockPos& pos, FacingID face, const Vec3& clickPos, int itemValue) const override;

	bool mayPlace(BlockSource& region, const BlockPos& pos, FacingID face) const override;
	const AABB& getVisualShape(BlockSource& region, const BlockPos& pos, AABB& bufferAABB, bool isClipping = false) const override;
	const AABB& getAABB(BlockSource& region, const BlockPos& pos, AABB& bufferValue, int optionalData = 0, bool isClipping = false, int clipData = 0) const override;
	
	bool isInteractiveBlock() const override;

	void spawnResources(BlockSource& region, const BlockPos& pos, int data, float odds = 1, int bonusLootLevel = 0) const override;
	int getResource(Random& random, int data, int bonusLootLevel = 0) const override;
	ItemInstance asItemInstance(BlockSource& region, const BlockPos& pos, int blockData) const override;

	bool canBeSilkTouched() const override;

	bool playerWillDestroy(Player& player, const BlockPos& pos, int data) const override;
	bool attack(Player* player, const BlockPos& pos) const override;

	void onPlace(BlockSource& region, const BlockPos& pos) const override;
	void onRemove(BlockSource& region, const BlockPos& pos) const override;

	void neighborChanged(BlockSource& region, const BlockPos& pos, const BlockPos& neighborPos) const override;

	bool hasComparatorSignal() const override;
	int getComparatorSignal(BlockSource& region, const BlockPos& pos, FacingID dir, int data) const override;

	bool use(Player& player, const BlockPos& pos) const override;

private:
	void getShape(int dir, AABB& bufferValue) const;
	int _getFacingID(int data) const;
	
	// Helpers for Map Wall achievement
	int _addMapCollection(std::vector<MapItemSavedData*>& mapGrids, const BlockPos& centerPos, BlockSource& region) const;
	void _checkAchievements(Player& player, const BlockPos& currentPos) const;

	ItemFrameBlockEntity* _getItemFrame(BlockSource& region, const BlockPos& pos) const;
};