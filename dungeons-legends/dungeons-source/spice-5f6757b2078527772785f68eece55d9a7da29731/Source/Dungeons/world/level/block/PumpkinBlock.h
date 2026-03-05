/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

#include "world/level/block/Block.h"

class PumpkinBlock : public Block {
public:
	static const int DIR_SOUTH = 0;
	static const int DIR_WEST = 1;
	static const int DIR_NORTH = 2;
	static const int DIR_EAST = 3;

	PumpkinBlock(const std::string& nameId, int id, bool lit);

	void onPlace(BlockSource& region, const BlockPos& pos) const override;
	bool mayPlace(BlockSource& region, const BlockPos& pos) const override;

	int getPlacementDataValue(Entity& by, const BlockPos& pos, FacingID face, const Vec3& clickPos, int itemValue) const override;

	int getVariant(int data) const override;
	FacingID getMappedFace(FacingID face, int data) const override;

	ItemInstance asItemInstance(BlockSource& region, const BlockPos& pos, int blockData) const override;

	bool dispense(BlockSource& region, Container& container, int slot, const Vec3& pos, FacingID face) const override;

private:
	bool _golemPumpkinTester(BlockSource& region, const BlockPos& pos, BlockID id) const;

	bool _canSpawnGolem(BlockSource& region, const BlockPos& pos) const;
	bool _canDispense(BlockSource& region, const Vec3& pos, FacingID face) const;

	bool mLit;
};
