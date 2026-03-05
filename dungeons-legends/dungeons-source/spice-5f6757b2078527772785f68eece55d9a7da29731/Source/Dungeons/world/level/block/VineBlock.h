/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

#include "world/level/block/Block.h"

class VineBlock : public Block {
public:
	static int VINE_SOUTH;
	static int VINE_NORTH;
	static int VINE_EAST;
	static int VINE_WEST;
	static int VINE_ALL;

	VineBlock(const std::string& nameId, int id);
	const AABB& getVisualShape(BlockSource& region, const BlockPos& pos, AABB& bufferAABB, bool isClipping = false) const override;
	const AABB& getAABB(BlockSource& region, const BlockPos& pos, AABB& bufferValue, int optionalData = 0, bool isClipping = false, int clipData = 0) const override;

	bool mayPlace(BlockSource& region, const BlockPos& pos, FacingID face) const override;
	
	int getColor(int auxData) const override;
	int getColor(BlockSource& region, const BlockPos& pos, DataID data) const override;
	virtual Color getMapColor(BlockSource& region, const BlockPos& pos) const override;
	
	void neighborChanged(BlockSource& region, const BlockPos& pos, const BlockPos& neighborPos) const override;
	
	void tick(BlockSource& region, const BlockPos& pos, Random& random ) const override;
	
	int getResource(Random& random, int data, int bonusLootLevel = 0) const override;
	int getResourceCount(Random& random, int data, int bonusLootLevel = 0) const override;
	
	void playerDestroy(Player* player, const BlockPos& pos, int data) const override;
	int getPlacementDataValue(Entity& by, const BlockPos& pos, FacingID face, const Vec3& clickPos, int itemValue) const override;
	bool updateSurvival(BlockSource& region, const BlockPos& pos) const;

private:
	static int _getFacingData(int facing);
	bool _isAcceptableNeighbor(const Block& block) const;
};
