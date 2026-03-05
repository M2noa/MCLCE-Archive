/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

#include "world/level/block/Block.h"

class DoorBlock : public Block {
public:
	enum DoorType {
		OAK = 0,
		SPRUCE,
		BIRCH,
		JUNGLE,
		ACACIA,
		DARKOAK,
		IRON,

		COUNT
	};

	bool isOpen(BlockSource& region, const BlockPos& pos) const;

	bool isUpper(DataID data) const;
	bool hasRightHinge(BlockSource& region, const BlockPos& pos) const;

	bool shouldFlipTexture(BlockSource& region, const BlockPos& pos, FacingID facing) const;

	DoorBlock(const std::string& nameId, int id, const Material& material, DoorType type);

	int getVariant(int data) const override;

	const AABB& getVisualShape(BlockSource& region, const BlockPos& pos, AABB& bufferAABB, bool isClipping = false) const override;

	bool use(Player& player, const BlockPos& pos) const override;

	int getDir(BlockSource& region, const BlockPos& pos) const;

	void setOpen(BlockSource& region, const BlockPos& pos, bool shouldOpen, Mob* eventSource) const;

	bool playerWillDestroy(Player& player, const BlockPos& pos, int data) const override;
	void neighborChanged(BlockSource& region, const BlockPos& pos, const BlockPos& neighborPos) const override;

	int getResource(Random& random, int data, int bonusLootLevel = 0) const override;
	ItemInstance asItemInstance(BlockSource& region, const BlockPos& pos, int blockData) const override;
	ItemInstance getSilkTouchItemInstance(DataID data) const override;

	bool getSecondPart(BlockSource& region, const BlockPos& pos, BlockPos& out) const override;

	bool mayPlace(BlockSource& region, const BlockPos& pos) const override;

	virtual void onLoaded(BlockSource& region, const BlockPos& pos) const override;
	virtual void onPlace(BlockSource& region, const BlockPos& pos) const override;
	virtual void onRedstoneUpdate(BlockSource& region, const BlockPos& pos, int strength, bool isFirstTime) const override;

	void tick(BlockSource& region, const BlockPos& pos, Random& random) const override;

	bool isInteractiveBlock() const override;
	virtual bool checkIsPathable(Entity& entity, const BlockPos& lastPathPos, const BlockPos& pathPos) const override;
	virtual bool isWaterBlocking() const override {
		return true;
	}

protected:
	bool canBeSilkTouched() const override;

private:
	void getDoorData(BlockSource& region, const BlockPos& pos, DataID& lowerData, DataID& upperData) const;
	void getDoorPosition(BlockSource& region, const BlockPos& pos, BlockPos& lowerPos, BlockPos& upperPos) const;

	DoorType mType;
};
