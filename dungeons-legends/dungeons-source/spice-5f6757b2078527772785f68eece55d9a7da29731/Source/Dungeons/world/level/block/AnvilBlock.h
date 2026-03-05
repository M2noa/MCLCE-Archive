/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

#include "world/level/block/HeavyBlock.h"

class Material;
class Mob;

enum class AnvilType : DataID {
	Undamaged = 0,
	SlightlyDamaged,
	VeryDamaged,
	Broken
};

enum class AnvilPart {
	Base = 0,
	Joint,
	Column,
	Top
};

class AnvilBlock : public HeavyBlock {
public:
	AnvilBlock(const std::string& nameId, int id, const Material& material);

	bool isCubeShaped(void);
	bool isSolidRender(void) const;

	bool use(Player& player, const BlockPos& pos) const override;

	Color getDustColor(DataID data) const override;
	void animateTick(BlockSource& region, const BlockPos& pos, Random& random) const override;

	bool falling() const override;
	void onLand(BlockSource& region, const BlockPos& pos) const override;

	DataID getSpawnResourcesAuxValue(DataID data) const override;
	ItemInstance asItemInstance(BlockSource& region, const BlockPos& pos, int blockData) const override;
	std::string buildDescriptionName(DataID data) const override;
	int getPlacementDataValue(Entity& by, const BlockPos& pos, FacingID face, const Vec3& clickPos, int itemValue) const override;
	bool shouldRenderFace(BlockSource& region, const BlockPos& pos, FacingID face, const AABB& shape) const override;

	int getContainerSize() const;

	bool isCraftingBlock() const override;
	virtual int getVariant(int data) const override;

	static int getDataForSide(int data);

protected:
	virtual bool canBeSilkTouched() const override;
};
