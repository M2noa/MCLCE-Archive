/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

#include "world/level/block/EntityBlock.h"

class HopperBlockEntity;

class HopperBlock : public EntityBlock {
public:
	static const std::string TEXTURE_OUTSIDE;
	static const std::string TEXTURE_INSIDE;

	static int getAttachedFace(int data);
	static bool isTurnedOn(int data);
// 	static HopperBlockEntity* getHopperBlockEntity(BlockSource& region, const BlockPos& pos);

	HopperBlock(const std::string& nameId, int id);

	void onPlace(BlockSource& region, const BlockPos& pos) const override;
	void onLoaded(BlockSource& region, const BlockPos& pos) const override;
	void onRemove(BlockSource& region, const BlockPos& pos) const override;
	void onRedstoneUpdate(BlockSource& region, const BlockPos& pos, int strength, bool isFirstTime) const override;

	bool use(Player& player, const BlockPos& pos) const override;

	bool isInteractiveBlock() const override;

	bool addCollisionShapes(BlockSource& region, const BlockPos& pos, const AABB* intersectTestBox, std::vector<AABB>& inoutBoxes, Entity* entity) const override;
	void addAABBs(BlockSource& region, const BlockPos& pos, const AABB* intersectTestBox, std::vector<AABB>& inoutBoxes) const override;

	virtual bool canProvideSupport(BlockSource& region, const BlockPos& pos, FacingID face, BlockSupportType type) const override;

	int getPlacementDataValue(Entity& by, const BlockPos& pos, FacingID face, const Vec3& clickPos, int itemValue) const override;

	bool hasComparatorSignal() const override;
	int getComparatorSignal(BlockSource& region, const BlockPos& pos, FacingID dir, int data) const override;

	int getResource(Random& random, int data, int bonusLootLevel = 0) const override;
	ItemInstance asItemInstance(BlockSource& region, const BlockPos& pos, int blockData) const override;
	bool canBeSilkTouched() const override;

protected:

private:
	void _installCircuit(BlockSource& region, const BlockPos& pos) const;
};
