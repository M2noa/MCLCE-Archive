#pragma once

#include "Block.h"
class region;
class AABB;
 
class TripWireHookBlock : public Block {
public:
	static const int WIRE_DIST_MIN = 1;
	static const int WIRE_DIST_MAX = 2 + 40; // 2 hooks + x string

	TripWireHookBlock(const std::string& nameId, int id);

	const AABB& getAABB(BlockSource& region, const BlockPos& pos, AABB& bufferValue, int optionalData = 0, bool isClipping = false, int clipData = 0) const override;
	const AABB& getVisualShape(BlockSource& region, const BlockPos& pos, AABB& bufferAABB, bool isClipping = false) const override;
	const AABB& getVisualShape(DataID data, AABB& bufferAABB, bool isClipping = false) const override;

	void onRemove(BlockSource& region, const BlockPos& pos) const override;
	void onPlace(BlockSource& region, const BlockPos& pos) const override;
	void onLoaded(BlockSource& region, const BlockPos& pos) const override;
	bool mayPlace(BlockSource& region, const BlockPos& pos, FacingID face) const override;
	bool mayPlace(BlockSource& region, const BlockPos& pos) const override;
	int getPlacementDataValue(Entity& by, const BlockPos& pos, FacingID face, const Vec3& clickPos, int itemValue) const override;
	void neighborChanged(BlockSource& region, const BlockPos& pos, const BlockPos& neighborPos) const override;

	void calculateState(BlockSource &region, const BlockPos& pos, bool isBeingDestroyed, int data, bool canUpdate, int wireSource, int wireSourceData) const;
	int getTickDelay() const;
	void tick(BlockSource& region, const BlockPos& pos, Random& random) const override;

	ItemInstance asItemInstance(BlockSource& region, const BlockPos& pos, int blockData) const override;

	int getResource(Random& random, int data, int bonusLootLevel = 0) const override;
	int getResourceCount(Random& random, int data, int bonusLootLevel = 0) const override;

	bool shouldConnectToRedstone(BlockSource& region, const BlockPos& pos, int direction) const override;

private:
	void _getShape(BlockSource& region, const BlockPos& pos, AABB& bufferValue) const;
	void _getShape(DataID data, AABB& bufferValue) const;
	void playSound(BlockSource &region, const BlockPos& pos, bool attached, bool powered, bool wasAttached, bool wasPowered) const;
	bool checkCanSurvive(BlockSource &region, const BlockPos& pos) const;

	int getDirection(DataID data) const;
	bool isPowered(DataID data) const;
	bool isAttached(DataID data) const;
};
