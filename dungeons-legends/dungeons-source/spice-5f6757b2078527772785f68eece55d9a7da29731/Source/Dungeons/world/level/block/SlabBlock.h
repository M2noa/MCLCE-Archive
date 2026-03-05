/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

#include "world/level/block/Block.h"

class SlabBlock : public Block {
public:
	static const int TOP_SLOT_BIT = 16; // This is this: 00010000, which means no slab can have an aux > 15
	static const int TYPE_MASK = TOP_SLOT_BIT - 1;

	enum class SlabType {
		WoodSlab,
		StoneSlab
	};

	static bool isBottomSlab(int data);

	SlabBlock(const std::string& nameId, int id, bool fullSize, const Material& mat );

	bool isBottomSlab(BlockSource& region, const BlockPos& pos) const;

	const AABB& getVisualShape(DataID data, AABB& bufferAABB, bool isClipping = false) const override;

	int getPlacementDataValue(Entity& by, const BlockPos& pos, FacingID face, const Vec3& clickPos, int itemValue) const override;

	int getResourceCount(Random& random, int data, int bonusLootLevel = 0) const override;

	bool shouldRenderFace(BlockSource& region, const BlockPos& pos, FacingID face, const AABB& shape) const override;

	bool isObstructingChests(BlockSource& region, const BlockPos& pos) const override;

	virtual BlockProperty getRedstoneProperty(BlockSource& region, const BlockPos& pos) const override;
 
	virtual bool checkIsPathable(Entity& entity, const BlockPos& lastPathPos, const BlockPos& pathPos) const override;

protected:
	bool canBeSilkTouched() const override;

	SlabType mSlabType;

private:
	bool fullSize;
};
