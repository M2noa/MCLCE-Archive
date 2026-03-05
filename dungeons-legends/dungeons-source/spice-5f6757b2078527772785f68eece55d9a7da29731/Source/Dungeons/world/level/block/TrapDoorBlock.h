/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

#include "world/level/block/Block.h"

class TrapDoorBlock : public Block {
public:
	TrapDoorBlock(const std::string& nameId, int id, const Material& material);

	virtual const AABB& getVisualShape(DataID data, AABB& bufferAABB, bool isClipping = false) const override;

	virtual bool use(Player& player, const BlockPos& pos) const override;
	void setOpen(BlockSource& region, const BlockPos& pos, bool shouldOpen) const;

	int getPlacementDataValue(Entity& by, const BlockPos& pos, FacingID face, const Vec3& clickPos, int itemValue) const override;

	virtual void onLoaded(BlockSource& region, const BlockPos& pos) const override;
	virtual void onRedstoneUpdate(BlockSource& region, const BlockPos& pos, int strength, bool isFirstTime) const override;
	virtual void onPlace(BlockSource& region, const BlockPos& pos) const override;

	ItemInstance asItemInstance(BlockSource& region, const BlockPos& pos, int blockData) const override;

	virtual bool isInteractiveBlock() const override;

	virtual int getResourceCount(Random& random, int data, int bonusLootLevel = 0) const override;
	virtual int getResource(Random& random, int data, int bonusLootLevel = 0) const override;

protected:
	virtual bool canBeSilkTouched() const override;

private:
	bool _isOpen(DataID data) const;
	bool _isUpsideDown(DataID data) const;
	int  _getDirection(DataID data) const;
	void _toggleOpen(BlockSource& region, Entity* user, const BlockPos& pos) const;

	// note - these values differ from Direction and that worries me so I'm leaving them here for now
	// changing these to direction values could break old worlds without conversion
	enum TrapDoorDir : unsigned char {
		DIR_EAST = 0,
		DIR_WEST,
		DIR_SOUTH,
		DIR_NORTH
	};
};
