/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

#include "world/level/block/Block.h"

class TorchBlock : public Block {
public:
	const static int TORCH_UNKNOWN = 0;
	const static int TORCH_WEST = 1;
	const static int TORCH_EAST = 2;
	const static int TORCH_NORTH = 3;	// attached to north block's south face
	const static int TORCH_SOUTH = 4;
	const static int TORCH_TOP = 5;

	TorchBlock(const std::string& nameId, int id);

	const AABB& getAABB(BlockSource& region, const BlockPos& pos, AABB& bufferValue, int optionalData = 0, bool isClipping = false, int clipData = 0) const override;

	int getIconYOffset() const override;

	bool mayPlace(BlockSource& region, const BlockPos& pos) const override;

	int getPlacementDataValue(Entity& by, const BlockPos& pos, FacingID face, const Vec3& clickPos, int itemValue) const override;

	void onPlace(BlockSource& region, const BlockPos& pos) const override;

	void neighborChanged(BlockSource& region, const BlockPos& pos, const BlockPos& neighborPos) const override;

	const AABB& getVisualShape(BlockSource& region, const BlockPos& pos, AABB& bufferAABB, bool isClipping = false) const override;

	void animateTick(BlockSource& region, const BlockPos& pos, Random& random) const override;

protected:	
	bool canBeSilkTouched() const override;

private:
	bool canBePlacedOn(BlockSource& region, const BlockPos& pos, FacingID facing) const;

	bool _checkDoPop(BlockSource& region, const BlockPos& pos) const;

	bool _checkCanSurvive(BlockSource& region, const BlockPos& pos) const;
	Vec3 _flameParticlePos(const BlockPos& pos, DataID dir) const;

	int getDirection(const DataID& data) const;

	static const DataID DATA_FROM_FACING[6];
	static const DataID FACING_FROM_DATA[6];
};
