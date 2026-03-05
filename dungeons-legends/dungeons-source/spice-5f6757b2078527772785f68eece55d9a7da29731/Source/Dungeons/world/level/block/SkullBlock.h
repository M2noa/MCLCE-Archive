/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

#include "world/level/block/EntityBlock.h"

class Level;
class SkullBlockEntity;

class SkullBlock : public EntityBlock {
public:
	static const int PLACEMENT_MASK = 0x7;
	static const int NO_DROP_BIT = 0x8;

	explicit SkullBlock(const std::string& nameId, int id);

	void checkMobSpawn(Level& level, BlockSource& region, const BlockPos& pos, SkullBlockEntity& placedSkull) const;
	virtual std::string buildDescriptionName(DataID data) const override;
	static std::string getTypeDescriptionId(int data);

	ItemInstance asItemInstance(BlockSource& region, const BlockPos& pos, int blockData) const override;

	const AABB& getVisualShape(BlockSource& region, const BlockPos& pos, AABB& bufferAABB, bool isClipping) const override;

	bool mayPlace(BlockSource& region, const BlockPos& pos) const override;
	int getPlacementDataValue(Entity& by, const BlockPos& pos, FacingID face, const Vec3& clickPos, int itemValue) const override;

	void spawnResources(BlockSource& region, const BlockPos& pos, int data, float odds = 1, int bonusLootLevel = 0) const override;

	bool playerWillDestroy(Player& player, const BlockPos& pos, int data) const override;
	void onRemove(BlockSource& region, const BlockPos& pos) const override;

	int getResource(Random& random, int data, int bonusLootLevel = 0) const override;

	void onLoaded(BlockSource& region, const BlockPos& pos) const override;
	void onPlace(BlockSource& region, const BlockPos& pos) const override;

protected:
	virtual bool canBeSilkTouched() const override;
	DataID getSpawnResourcesAuxValue(DataID data) const override;

private:
	bool _witherSkullTester(BlockSource& region, const BlockPos& pos, BlockID id) const;
	void _updatedDragonCircuit(BlockSource& region, const BlockPos& pos) const;

};
