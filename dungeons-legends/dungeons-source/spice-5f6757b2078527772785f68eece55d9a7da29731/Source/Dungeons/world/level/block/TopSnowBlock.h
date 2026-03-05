/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

#include "world/level/block/HeavyBlock.h"

class ItemInstance;

class TopSnowBlock : public HeavyBlock {
public:
	static const int MAX_HEIGHT;
	static const int HEIGHT_IMPASSABLE;	// if these many steps high, then can't path over it
	static const int HEIGHT_MASK;		// 3 bits for height (0-7)
	static const int HAS_COVERED_MASK;	// 4th bit for 'is covering something' true/false

	static void registerRecoverableBlock(BlockID blockId);
	static bool checkIsRecoverableBlock(BlockID block);

	static DataID buildData(BlockSource& region, BlockPos const& pos, int height, bool additive);
	static FullBlock dataIDToRecoverableFullBlock(BlockSource& region, const BlockPos& pos, DataID dataId);

	static int dataIDToHeight(DataID dataId);

	TopSnowBlock(const std::string& nameId, int id);
	virtual bool checkIsPathable(Entity& entity, const BlockPos& lastPathPos, const BlockPos& pathPos) const override;

	const AABB& getVisualShape(BlockSource& region, const BlockPos& pos, AABB& bufferAABB, bool isClipping = false) const override;
	const AABB& getVisualShape(DataID data, AABB& bufferAABB, bool isClipping = false) const override;

	const AABB& getAABB(BlockSource& region, const BlockPos& pos, AABB& bufferValue, int optionalData = 0, bool isClipping = false, int clipData = 0) const override;
	bool getCollisionShape(AABB& outAABB, BlockSource& region, const BlockPos& pos, Entity* entity) const override;

	void onPlace(BlockSource& region, const BlockPos& pos) const override;
	bool mayPlace(BlockSource& region, const BlockPos& pos) const override;
	bool tryToPlace(BlockSource& region, const BlockPos& pos, DataID data) const override;
	int getPlacementDataValue(Entity& by, const BlockPos& pos, FacingID face, const Vec3& clickPos, int itemValue) const override;

	void neighborChanged(BlockSource& region, const BlockPos& pos, const BlockPos& neighborPos) const override;
	void playerDestroy(Player* player, const BlockPos& pos, int data) const override;
	bool playerWillDestroy(Player& player, const BlockPos& pos, int data) const override;

	void onRemove(BlockSource& region, const BlockPos& pos) const override;

	int getResource(Random& random, int data, int bonusLootLevel = 0) const override;

	int getResourceCount(Random& random, int data, int bonusLootLevel = 0) const override;

	ItemInstance asItemInstance(BlockSource& region, const BlockPos& pos, int blockData) const override;

	void tick(BlockSource& region, const BlockPos& pos, Random& random) const override;
	bool shouldRenderFace(BlockSource& region, const BlockPos& pos, FacingID face, const AABB& shape) const override;

	Color getDustColor(DataID data) const override;
	bool shouldStopFalling(Entity& entity) const override;

	float calcGroundFriction(Mob& mob, BlockPos const& blockPos) const override;
	float calcSnowThickness(Mob* mob, BlockPos const& blockPos) const;

	bool canBeBuiltOver(BlockSource& region, const BlockPos& pos) const override;

	bool canHaveExtraData() const override;

	virtual bool waterSpreadCausesSpawn() const override {
		return false;
	}

protected:
	bool canBeSilkTouched() const override;

	bool isFree(BlockSource& region, const BlockPos& pos) const override;
	void checkSlide(BlockSource& region, const BlockPos& pos) const;
	void startFalling(BlockSource& region, const BlockPos& pos, const Block* oldBlock, bool creative) const override;

private:
	static std::unordered_set<int> mRecoverableLookupSet;// <FullBlock.toInt()> set to lookup the FullBlock's 'recoverableLookupTable' index

	bool isCovering(const DataID& data) const;
	static int getHeight(const DataID& data);
};
