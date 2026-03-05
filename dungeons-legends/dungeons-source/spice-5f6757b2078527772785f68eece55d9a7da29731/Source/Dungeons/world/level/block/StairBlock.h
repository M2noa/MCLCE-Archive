/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

#include "world/level/block/Block.h"

class StairBlock : public Block {
public:
	static const int UPSIDEDOWN_BIT = 4;

	// the direction is the way going up (for normal non-upsidedown stairs)
	static const int DIR_EAST = 0;
	static const int DIR_WEST = 1;
	static const int DIR_SOUTH = 2;
	static const int DIR_NORTH = 3;

	static const int DEAD_SPACES[8][2];

	static bool isStairs(int id);

	StairBlock(const std::string& nameId, int id, const Block& base, int baseData = 0);

	HitResult clip(BlockSource& region, const BlockPos& pos, const Vec3& A, const Vec3& B, bool isClipping = false, int clipData = 0) const override;
	const AABB& getAABB(BlockSource& region, const BlockPos& pos, AABB& bufferValue, int optionalData = 0, bool isClipping = false, int clipData = 0) const override;

	void setBaseShape(BlockSource& region, const BlockPos& pos, AABB& shape, bool shrink = false) const;

	bool isLockAttached(BlockSource& region, const BlockPos& pos, int data) const;

	bool setStepShape(BlockSource& region, const BlockPos& pos, AABB& shape, bool shrink = false) const;
	bool setInnerPieceShape(BlockSource& region, const BlockPos& pos, AABB& shape, bool shrink = false) const;

	virtual void addAABBs(BlockSource& region, const BlockPos& pos, const AABB* intersectTestBox, std::vector<AABB>& inoutBoxes) const override;

	virtual bool addCollisionShapes(BlockSource& region, const BlockPos& pos, const AABB* intersectTestBox, std::vector<AABB>& inoutBoxes, Entity* entity) const override;

	void tick(BlockSource& region, const BlockPos& pos, Random& random) const override;
	void animateTick(BlockSource& region, const BlockPos& pos, Random& random) const override;

	bool attack(Player* player, const BlockPos& pos) const override;

	void destroy(BlockSource& region, const BlockPos& pos, int data, Entity* entitySource) const override;

	int getResourceCount(Random& random, int data, int bonusLootLevel = 0) const override;

	void handleEntityInside(BlockSource& region, const BlockPos& pos, Entity* entity, Vec3& current) const override;

	bool mayPick() const override;
	bool mayPick(BlockSource& region, int data, bool liquid) const override;
	bool mayPlace(BlockSource& region, const BlockPos& pos, FacingID face) const override;
	void onPlace(BlockSource& region, const BlockPos& pos) const override;
	virtual BlockProperty getRedstoneProperty(BlockSource& region, const BlockPos& pos) const override;

	void onRemove(BlockSource& region, const BlockPos& pos) const override;

	void onStepOn(Entity& entity, const BlockPos& pos) const override;

	bool use(Player& player, const BlockPos& pos) const override;

	void onExploded(BlockSource& region, const BlockPos& pos, Entity* entitySource) const override;

	int getPlacementDataValue(Entity& by, const BlockPos& pos, FacingID face, const Vec3& clickPos, int itemValue) const override;

	bool isStairBlock() const override;

	int getVariant(int data) const override;

protected:
	virtual bool canBeSilkTouched() const override;

	void shapeZFightShrink(AABB& shape) const;

	int getDirection(const DataID& data) const;
	bool isUpsideDown(const DataID& data) const;
private:
	const Block& mBase;
#if 0 //[IMPLEMENTATION] Fields ported over and not used 
	int mBaseData;
#endif
};
