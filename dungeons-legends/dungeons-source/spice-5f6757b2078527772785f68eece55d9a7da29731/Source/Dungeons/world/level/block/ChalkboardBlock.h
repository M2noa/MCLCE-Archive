/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

#include "world/level/block/EntityBlock.h"
#include "util/Random.h"

enum class ChalkboardSize : uint8_t;
class ChalkboardBlockEntity;

class ChalkboardBlock : public EntityBlock {
public:
	ChalkboardBlock(const std::string& nameId, int id);

	virtual bool playerWillDestroy(Player& player, const BlockPos& pos, int data) const override;

	virtual const AABB& getVisualShape(BlockSource& region, const BlockPos& pos, AABB& bufferAABB, bool isClipping = false) const override;
	virtual const AABB& getAABB(BlockSource& region, const BlockPos& pos, AABB& bufferValue, int optionalData = 0, bool isClipping = false, int clipData = 0) const override;
	
	virtual bool checkIsPathable(Entity& entity, const BlockPos& lastPathPos, const BlockPos& pathPos) const override;
	
	virtual ItemInstance asItemInstance(BlockSource& region, const BlockPos& pos, int blockData) const override;
	virtual void spawnResources(BlockSource& region, const BlockPos& pos, int data, float odds = 1, int bonusLootLevel = 0) const override;

	virtual void neighborChanged(BlockSource& region, const BlockPos& pos, const BlockPos& neighborPos) const override;
	virtual bool use(Player& player, const BlockPos& pos) const override;

	virtual bool isWaterBlocking() const override {
		return true;
	}

	virtual bool isInteractiveBlock() const override {
		return true;
	}
	
	virtual int getVariant(int data) const override;

	static ChalkboardBlockEntity* getBlockEntity(BlockSource& region, const BlockPos& pos);

protected:
	virtual bool canBeSilkTouched() const override;

private:
	void _getShape(ChalkboardSize size, bool isOnGround, int face, AABB& bufferValue) const;
};
