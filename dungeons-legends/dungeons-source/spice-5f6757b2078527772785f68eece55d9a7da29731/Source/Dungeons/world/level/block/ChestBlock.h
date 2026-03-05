/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

#include "world/level/block/EntityBlock.h"
#include "util/Random.h"

#include "world/level/material/MaterialType.h"

class Level;
class BlockSource;
class Mob;

class ChestBlock : public EntityBlock {
public:

	enum ChestType {
		TYPE_BASIC = 0,
		TYPE_TRAP = 1,
		TYPE_ENDER = 2
	};

	static const int EVENT_SET_IS_OPEN = 1;

	ChestBlock(const std::string& nameId, int id, ChestBlock::ChestType type = TYPE_BASIC, MaterialType materialType = MaterialType::Wood);

	virtual Block& init() override;

	int getPlacementDataValue(Entity& by, const BlockPos& pos, FacingID face, const Vec3& clickPos, int itemValue) const override;

	virtual void onLoaded(BlockSource& region, const BlockPos& pos) const override;
	virtual void onPlace(BlockSource& region, const BlockPos& pos) const override;
	virtual void onRemove(BlockSource& region, const BlockPos& pos) const override;

	void onMove(BlockSource& region, const BlockPos& from, const BlockPos& to) const override;
	bool detachesOnPistonMove(BlockSource& region, const BlockPos& pos) const override;

	void updateSignalStrength(BlockSource& region, const BlockPos& pos, int strength) const;

	bool hasComparatorSignal() const override;
	int getComparatorSignal(BlockSource& region, const BlockPos& pos, FacingID dir, int data) const override;

	FacingID getMappedFace(FacingID face, int data) const override;

	bool use(Player& player, const BlockPos& pos) const override;

	virtual bool getSecondPart(BlockSource& region, const BlockPos& pos, BlockPos& out) const override;

	virtual bool isContainerBlock() const override;

	bool shouldConnectToRedstone(BlockSource& region, const BlockPos& pos, int direction) const override;


protected:
	virtual bool canBeSilkTouched() const override;

private:

	ChestType mType;
};
