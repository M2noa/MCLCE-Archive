/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

#include "world/level/block/Block.h"
#include "world/level/material/Material.h"
#include "world/Facing.h"

class BlockSource;
class MaterialBlock;

class EndRodBlock : public Block {
public:
	const static int ROD_DOWN = Facing::DOWN;    // End rods can be placed upside down
	const static int ROD_WEST = Facing::WEST;
	const static int ROD_EAST = Facing::EAST;
	const static int ROD_NORTH = Facing::NORTH;
	const static int ROD_SOUTH = Facing::SOUTH;
	const static int ROD_TOP = Facing::UP;

	EndRodBlock(const std::string& nameId, int id);

	virtual bool mayPlace(BlockSource& region, const BlockPos& pos) const override;
	virtual bool mayPlace(BlockSource& region, const BlockPos& pos, FacingID facing) const override;

	virtual int getPlacementDataValue(Entity& by, const BlockPos& pos, FacingID face, const Vec3& clickPos, int itemValue) const override;

	virtual void neighborChanged(BlockSource& region, const BlockPos& pos, const BlockPos& neighborPos) const override;

	virtual void animateTick(BlockSource& region, const BlockPos& pos, Random& random) const override;

	virtual bool breaksFallingBlocks(int data) const override;

	virtual bool canSurvive(BlockSource& region, const BlockPos& pos) const override;

	virtual const AABB& getVisualShape(BlockSource& region, const BlockPos& pos, AABB& bufferAABB, bool isClipping = false) const override;

	virtual void addAABBs(BlockSource& region, const BlockPos& pos, const AABB* intersectTestBox, std::vector<AABB>& inoutBoxes) const override;

protected:
	bool canStandOn(BlockSource& region, const BlockPos& pos) const;

private:
	DataID _mirror(const DataID theirs) const;                                                 // Flip a FacingID
	bool _shouldMirror(const Block& b, const DataID adjacentFace, const DataID ourFace) const; // Determine if we should mirror another end rod's direction
	void _shapeFromDirection(Vec3& first, Vec3& second, const int facing) const;
};
