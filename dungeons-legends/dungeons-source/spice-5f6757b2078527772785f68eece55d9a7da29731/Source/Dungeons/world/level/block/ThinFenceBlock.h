/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

#include "world/level/block/Block.h"

class ThinFenceBlock : public Block {
public:
	ThinFenceBlock(const std::string& nameId, int id, const Material& material, bool dropsResources);

	virtual int getResource(Random& random, int data, int bonusLootLevel = 0) const override;

	bool shouldRenderFace(BlockSource& region, const BlockPos& pos, FacingID face, const AABB& shape) const override;

	void addAABBs(BlockSource& region, const BlockPos& pos, const AABB* intersectTestBox, std::vector<AABB>& inoutBoxes) const override;
	bool addCollisionShapes( BlockSource& region, const BlockPos& pos, const AABB* intersectTestBox, std::vector<AABB>& inoutBoxes, Entity* entity ) const override;

	const AABB& getVisualShape(BlockSource& region, const BlockPos& pos, AABB& bufferAABB, bool isClipping = false) const override;

	bool attachsTo(int block) const;

	virtual ItemInstance asItemInstance(BlockSource& region, const BlockPos& pos, int blockData) const override;
	std::string buildDescriptionName(DataID data) const override;
	virtual ItemInstance getSilkTouchItemInstance(DataID data) const override;

private:
	const bool mDropsResources;
};
