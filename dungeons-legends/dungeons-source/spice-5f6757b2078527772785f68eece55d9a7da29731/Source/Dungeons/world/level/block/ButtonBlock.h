/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

#include "world/level/block/Block.h"
#include <string>
#include "world/Facing.h"
#include "Core/Math/Math.h"

class Player;
class BlockSource; 
class Random;
class Entity;
class Mob;
class AABB;

class ButtonBlock : public Block {
protected:
	ButtonBlock(const std::string& nameId, int id, bool sensitive);

public:
	bool isButtonPressed(int data) const;

	bool getCollisionShape(AABB& outAABB, BlockSource& region, const BlockPos& pos, Entity* entity) const override;
	const AABB& getVisualShape(DataID data, AABB& bufferAABB, bool isClipping = false) const override;

	int getTickDelay() const;

	void onLoaded(BlockSource& region, const BlockPos& pos) const override;
	void onPlace(BlockSource& region, const BlockPos& pos) const override;
	void onRedstoneUpdate(BlockSource& region, const BlockPos& pos, int strength, bool isFirstTime) const override;
	bool mayPlace(BlockSource& region, const BlockPos& pos, FacingID face) const override;
	bool mayPlace(BlockSource& region, const BlockPos& pos) const override;
	void neighborChanged(BlockSource& region, const BlockPos& pos, const BlockPos& neighborPos) const override;
	int getPlacementDataValue(Entity& by, const BlockPos& pos, FacingID face, const Vec3& clickPos, int itemValue) const override;

	bool use(Player& player, const BlockPos& pos) const override;
	void tick(BlockSource& region, const BlockPos& pos, Random& random) const override;
	void entityInside(BlockSource& region, const BlockPos& pos, Entity& entity) const override;
	ItemInstance asItemInstance(BlockSource& region, const BlockPos& pos, int blockData) const override;

	static bool canAttachTo(BlockSource& region, const BlockPos& pos, FacingID facing);
	virtual bool isAttachedTo(BlockSource& region, const BlockPos& pos, BlockPos& outAttachedTo) const override;
	int getResourceCount(Random& random, int data, int bonusLootLevel = 0) const override;
	int getResource(Random& random, int data, int bonusLootLevel = 0) const override;
	DataID getSpawnResourcesAuxValue(DataID data) const override;

	bool shouldConnectToRedstone(BlockSource& region, const BlockPos& pos, int direction) const override;
	bool isInteractiveBlock() const override;
	int getVariant(int data) const override;

private:
	void _getShape(AABB& bufferValue, DataID data, bool ignorePressedState = false) const;
	bool _checkCanSurvive(BlockSource& region, const BlockPos& pos) const;
	void _checkPressed(BlockSource& region, const BlockPos& pos) const;

	void _buttonPressed(BlockSource& region, FullBlock fb, const Vec3& pos, Entity* presser) const;
	void _buttonUnpressed(BlockSource& region, FullBlock fb, const Vec3& pos) const;

	const bool mSensitive;
};
