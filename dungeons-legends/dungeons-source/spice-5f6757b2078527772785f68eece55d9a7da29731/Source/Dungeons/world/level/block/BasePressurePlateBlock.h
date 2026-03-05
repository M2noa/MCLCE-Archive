/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/
#pragma once

#include "Block.h"

class Material;
class AABB;
class Tile;
class Entity;

class BasePressurePlateBlock : public Block {
private:
	std::string texture;

protected:
	BasePressurePlateBlock(const std::string& nameId, int id, const Material& material);

public:
	virtual const AABB& getVisualShape(BlockSource& region, const BlockPos& pos, AABB& bufferAABB, bool isClipping) const override;
	virtual const AABB& getVisualShape(DataID data, AABB& bufferAABB, bool isClipping) const override;

	virtual int getTickDelay() const;

	virtual const AABB& getAABB(BlockSource& region, const BlockPos& pos, AABB& bufferValue, int optionalData = 0, bool isClipping = false, int clipData = 0) const override;
	virtual bool getCollisionShape(AABB& outAABB, BlockSource& region, const BlockPos& pos, Entity* entity) const override;

	virtual bool checkIsPathable(Entity& entity, const BlockPos& lastPathPos, const BlockPos& pathPos) const override;

	virtual bool mayPlace(BlockSource& region, const BlockPos& pos) const override;
	virtual bool isAttachedTo(BlockSource& region, const BlockPos& pos, BlockPos& outAttachedTo) const override;

	virtual void tick(BlockSource& region, const BlockPos& pos, Random& random) const override;

	virtual void entityInside(BlockSource& region, const BlockPos& pos, Entity& entity) const override;

	virtual void onLoaded(BlockSource& region, const BlockPos& pos) const override;
	virtual void onPlace(BlockSource& region, const BlockPos& pos) const override;
	virtual void neighborChanged(BlockSource& region, const BlockPos& pos, const BlockPos& neighborPos) const override;
	
	virtual bool shouldConnectToRedstone(BlockSource& region, const BlockPos& pos, int direction) const override;

	virtual int getVariant(int data) const override;

protected:
	virtual int getSignalStrength(BlockSource& region, const BlockPos &pos) const = 0;
	virtual int getSignalForData(int data) const = 0;
	virtual int getDataForSignal(int signal) const = 0;

	void checkPressed(BlockSource& region, const BlockPos& pos, int oldSignal, int newSignal) const;
	virtual const AABB getSensitiveAABB(const BlockPos& pos) const;

private:
	int getRedstoneSignal(DataID data) const;
};
