/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

#include "world/level/block/Block.h"

class WallBlock : public Block {
public:
	static const float WALL_WIDTH;
	static const float WALL_HEIGHT;
	static const float POST_WIDTH;
	static const float POST_HEIGHT;

	static const int TYPE_NORMAL = 0;
	static const int TYPE_MOSSY = 1;

	static const std::string COBBLE_NAMES[];

	WallBlock(const std::string& nameId, int id, const Block& baseBlock);
	WallBlock(const std::string& nameId, int id, const Material& material);

	std::string buildDescriptionName(DataID data) const override;
	
	const AABB& getAABB(BlockSource& region, const BlockPos& pos, AABB& bufferValue, int optionalData = 0, bool isClipping = false, int clipData = 0) const override;
	const AABB& getVisualShape(BlockSource& region, const BlockPos& pos, AABB& bufferAABB, bool isClipping = false) const override;

	virtual bool canProvideSupport(BlockSource& region, const BlockPos& pos, FacingID face, BlockSupportType type) const override;
	
	virtual bool connectsTo(BlockSource& region, const BlockPos& thisPos, const BlockPos& otherPos) const;
	
	DataID getSpawnResourcesAuxValue(DataID data) const override;
	bool shouldRenderFace(BlockSource& region, const BlockPos& pos, FacingID face, const AABB& shape) const override;

	int getVariant(int data) const override;

protected:
	virtual bool canBeSilkTouched() const override;
};
