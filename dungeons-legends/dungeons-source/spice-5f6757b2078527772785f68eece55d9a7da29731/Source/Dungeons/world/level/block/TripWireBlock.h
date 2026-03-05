#pragma once

#include "Block.h"

class Level;
class AABB;
class Item;


class Player;
class Entity;
class Random;
class Entity;

class TripWireBlock : public Block {
public:
	TripWireBlock(const std::string& nameId, int id);

	int getTickDelay() const;

	virtual void tick(BlockSource& region, const BlockPos& pos, Random& random) const override;

	virtual const AABB& getAABB(BlockSource& region, const BlockPos& pos, AABB& bufferValue, int optionalData = 0, bool isClipping = false, int clipData = 0) const override;
	virtual const AABB& getVisualShape(BlockSource& region, const BlockPos& pos, AABB& bufferAABB, bool isClipping = false) const override;
	virtual const AABB& getVisualShape(DataID data, AABB& bufferAABB, bool isClipping = false) const override;

	virtual void onPlace(BlockSource& region, const BlockPos& pos) const override;
	virtual void onRemove(BlockSource& region, const BlockPos& pos) const override;
	virtual bool playerWillDestroy(Player& player, const BlockPos& pos, int data) const override;

	virtual int getResource(Random& random, int data, int bonusLootLevel = 0) const override;
	virtual ItemInstance asItemInstance(BlockSource& region, const BlockPos& pos, int blockData) const override;
	virtual ItemInstance getSilkTouchItemInstance(DataID data) const override;

	virtual void entityInside(BlockSource& region, const BlockPos& pos, Entity& entity) const override;
	static bool shouldConnectTo(BlockSource& level, const BlockPos& pos, int data, int dir);

private:
	void updateSource(BlockSource& level, const BlockPos& pos, int data) const;
	void checkPressed(BlockSource& level, const BlockPos& pos) const;
public:
	bool isPowered(DataID data) const;
	bool isSuspended(DataID data) const;
	bool isAttached(DataID data) const;
	bool isDisarmed(DataID data) const;
};
