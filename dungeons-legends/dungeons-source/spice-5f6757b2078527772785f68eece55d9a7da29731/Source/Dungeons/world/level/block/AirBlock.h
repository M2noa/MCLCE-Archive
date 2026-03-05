/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

#include "world/level/block/Block.h"
#include "world/level/material/Material.h"

class AirBlock : public Block {
public:
	AirBlock(const std::string& nameId, int id, const Material& material);

	// Visual Shape
	const AABB& getVisualShape(BlockSource& region, const BlockPos& pos, AABB& bufferAABB, bool isClipping = false) const override;
	const AABB& getVisualShape(DataID data, AABB& bufferAABB, bool isClipping = false) const override;

	bool isObstructingChests(BlockSource& region, const BlockPos& pos) const override;
	virtual bool isWaterBlocking() const override { 
		return false; 
	}

	bool shouldRenderFace(BlockSource& region, const BlockPos& pos, FacingID face, const AABB& shape) const override;

	void addAABBs(BlockSource& region, const BlockPos& pos, const AABB* intersectTestBox, std::vector<AABB>& inoutBoxes) const override;
	// The bufferValue is not always the returned value, always use the reference returned.
	const AABB& getAABB(BlockSource& region, const BlockPos& pos, AABB& bufferValue, int optionalData = 0, bool isClipping = false, int clipData = 0) const override;
	bool addCollisionShapes(BlockSource& region, const BlockPos& pos, const AABB* intersectTestBox, std::vector<AABB>& inoutBoxes, Entity* entity) const override;
	bool getCollisionShape(AABB& outAABB, BlockSource& region, const BlockPos& pos, Entity* entity) const override;

	bool checkIsPathable(Entity& entity, const BlockPos& lastPathPos, const BlockPos& pathPos) const override;

	bool mayPick() const override;
	bool mayPick(BlockSource& region, int data, bool liquid) const override;
	bool mayPlace(BlockSource& region, const BlockPos& pos, FacingID face) const override;
	bool mayPlace(BlockSource& region, const BlockPos& pos) const override;
	bool tryToPlace(BlockSource& region, const BlockPos& pos, DataID data = 0) const override;

	void tick(BlockSource& region, const BlockPos& pos, Random& random) const override;
	void animateTick(BlockSource& region, const BlockPos& pos, Random& random) const override;

	void destroy(BlockSource& region, const BlockPos& pos, int data, Entity* entitySource) const override;
	bool playerWillDestroy(Player& player, const BlockPos& pos, int data) const override;

	void entityInside(BlockSource& region, const BlockPos& pos, Entity& entity) const override;

	bool canHaveExtraData() const override;

protected:
	bool canBeSilkTouched() const override;
};

