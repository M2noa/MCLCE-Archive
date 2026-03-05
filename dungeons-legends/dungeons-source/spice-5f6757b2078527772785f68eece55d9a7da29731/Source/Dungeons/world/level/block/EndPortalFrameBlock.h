/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

#include "world/level/block/Block.h"

class EndPortalFrameBlock : public Block {
public:
	static const int EYE_BIT = 4;
	static const int PORTAL_SIZE = 3;

	static bool hasEye(int data);

	EndPortalFrameBlock(const std::string& nameId, int id);

	void addAABBs(BlockSource& region, const BlockPos& pos, const AABB* intersectTestBox, std::vector<AABB>& inoutBoxes) const override;
	bool addCollisionShapes(BlockSource& region, const BlockPos& pos, const AABB* intersectTestBox, std::vector<AABB>& inoutBoxes, Entity* entity) const override;
	int getPlacementDataValue(Entity& by, const BlockPos& pos, FacingID face, const Vec3& clickPos, int itemValue) const override;

	int getResource(Random& random, int data, int bonusLootLevel = 0) const override;
	int getResourceCount(Random& random, int data, int bonusLootLevel = 0) const override;

	virtual void onPlace(BlockSource& region, const BlockPos& pos) const override;

	virtual bool use(Player& player, const BlockPos& pos) const override;
	virtual bool isInteractiveBlock() const override;
 
	int getComparatorSignal(BlockSource& region, const BlockPos& pos, FacingID dir, int data) const override;

	static void tryCreatePortal(BlockSource& region, const BlockPos& pos);

	virtual ItemInstance asItemInstance(BlockSource& region, const BlockPos& pos, int blockData) const override;
	virtual ItemInstance getSilkTouchItemInstance(DataID data) const override;
private:
	static void createPortal(BlockSource& region, const BlockPos& origin);
};
