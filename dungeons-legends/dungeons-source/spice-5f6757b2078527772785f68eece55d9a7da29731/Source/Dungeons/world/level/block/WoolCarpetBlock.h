/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

#include "world/level/block/Block.h"

class BlockSource;

class WoolCarpetBlock : public Block {
public:
	WoolCarpetBlock(const std::string& nameId, int id);

	const AABB& getVisualShape(DataID data, AABB& bufferAABB, bool isClipping = false) const override;
	bool mayPlace(BlockSource& region, const BlockPos& pos) const override;
	virtual void onPlace(BlockSource& region, const BlockPos& pos) const override;
	void neighborChanged(BlockSource& region, const BlockPos& pos, const BlockPos& neighborPos) const override;
	bool canSurvive(BlockSource& region, const BlockPos& pos) const override;
	bool shouldRenderFace(BlockSource& region, const BlockPos& pos, FacingID face, const AABB& shape) const override;

	bool checkIsPathable(Entity& entity, const BlockPos& lastPathPos, const BlockPos& pathPos) const override;

	static int getBlockDataForItemAuxValue(int auxValue);
	static int getItemAuxValueForBlockData(int data);

	Color getMapColor(BlockSource& region, const BlockPos& pos) const override;

protected:
	DataID getSpawnResourcesAuxValue(DataID data) const override;
};
