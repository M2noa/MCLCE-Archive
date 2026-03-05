/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

#include "world/level/block/Block.h"

class RotatedPillarBlock : public Block {
public:
	static const int FACING_Y = 0;
	static const int FACING_X = 1;
	static const int FACING_Z = 2;

	static int getType(int data);

	RotatedPillarBlock(const std::string& nameId, int id, const Material& material);

	ItemInstance asItemInstance(BlockSource& region, const BlockPos& pos, int blockData) const override;

	int getVariant(int data) const override;
	FacingID getMappedFace(FacingID face, int data) const override;

	int getPlacementDataValue(Entity& by, const BlockPos& pos, FacingID face, const Vec3& clickPos, int itemValue) const override;

private:
	static const int mRotatedX[6];
	static const int mRotatedZ[6];
};
